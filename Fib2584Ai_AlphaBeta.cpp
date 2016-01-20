#include "Fib2584Ai.h"
#include <climits>
#include <cmath>
#include <functional>
#include <sys/time.h>

Fib2584Ai::AlphaBeta::AlphaBeta(int maxTime_us, const TDLearningNew &td)
:	maxTime_us(maxTime_us), td(td)
{
	maxNodeTT = new MoveDirection [TT_SIZE]();
	minNodeTT = new int [TT_SIZE]();
}

Fib2584Ai::AlphaBeta::~AlphaBeta()
{
	delete [] maxNodeTT;
	delete [] minNodeTT;
}

inline int elapsedTime(const timeval &startTime, const timeval &endTime)
{
	return (endTime.tv_sec - startTime.tv_sec) * 1000000 + 
		endTime.tv_usec - startTime.tv_usec;
}

MoveDirection Fib2584Ai::AlphaBeta::generateMove(const int board[4][4], 
	int moveCount) const
{
	GameBoard convertedBoard(board);
	MoveDirection dir = (MoveDirection)0;
	timeval startTime, endTime;

	gettimeofday(&startTime, 0);
	for (int i = 0; ; i++) {
		maxNode(dir, convertedBoard, INT_MIN, INT_MAX, 0, 
			i, moveCount);
		gettimeofday(&endTime, 0);
		if (elapsedTime(startTime, endTime) > maxTime_us)
			break;
	}

	return dir;
}

int Fib2584Ai::AlphaBeta::generateEvilMove(const int board[4][4], 
	int moveCount) const
{
	GameBoard convertedBoard(board);
	int evilPos = 0;
	timeval startTime, endTime;

	gettimeofday(&startTime, 0);
	for (int i = 0; ; i++) {
		minNode(evilPos, convertedBoard, INT_MIN, INT_MAX, 0, 
			i, moveCount);
		gettimeofday(&endTime, 0);
		if (elapsedTime(startTime, endTime) > maxTime_us)
			break;
	}
	return evilPos;
}

int Fib2584Ai::AlphaBeta::maxNode(MoveDirection &dir, GameBoard &board, 
	int alpha, int beta, int totalReward, int depth, int moveCount) const
{
	int hashBoard = getHashBoard(board);

	if (depth == 0) {
		int m = alpha;
		bool canMove = false;

		for (int i = 0; i < 4; i++) {
			GameBoard nextBoard(board);
			int reward = nextBoard.move((MoveDirection)i);
			if (nextBoard == board)
				continue;
			canMove = true;
		
			int score = totalReward + reward + td.evaluateBoard(nextBoard);
			if (score > m) {
				dir = (MoveDirection)i;
				m = score;
			}
			if (m >= beta) {
				maxNodeTT[hashBoard] = (MoveDirection)i;
				return m;
			}
		}
			
		maxNodeTT[hashBoard] = dir;
		return canMove ? m : INT_MIN + totalReward;
	}
	else {
		int m = alpha;
		bool canMove = false;

		// Try last dir first
		MoveDirection oldDir = maxNodeTT[hashBoard];
		{
			GameBoard nextBoard(board);
			int reward = nextBoard.move(oldDir);
			if (!(nextBoard == board)) {
				canMove = true;

				int useless = 0;
				int score = minNode(useless, nextBoard, m, beta, 
					totalReward + reward, depth - 1, moveCount + 1);
				if (score > m) {
					dir = oldDir;
					m = score;
				}
				if (m >= beta)
					return m;
			}
		}

		// Try other dir
		for (int i = 0; i < 4; i++) {
			if (i == oldDir)
				continue;
				
			GameBoard nextBoard(board);
			int reward = nextBoard.move((MoveDirection)i);
			if (nextBoard == board)
				continue;
			canMove = true;

			int useless = 0;
			int score = minNode(useless, nextBoard, m, beta, 
				totalReward + reward, depth - 1, moveCount + 1);
			if (score > m) {
				dir = (MoveDirection)i;
				m = score;
			}
			if (m >= beta) {
				maxNodeTT[hashBoard] = (MoveDirection)i;
				return m;
			}
		}

		maxNodeTT[hashBoard] = dir;
		return canMove ? m : INT_MIN + totalReward;
	}
}

int Fib2584Ai::AlphaBeta::minNode(int &evilPos, GameBoard &board, 
	int alpha, int beta, int totalReward, int depth, int moveCount) const
{
	int m = beta;
	int hashBoard = getHashBoard(board);

	// Try old evilPos first
	int oldEvilPos = minNodeTT[hashBoard];
	{
		GameBoard nextBoard(board);
		bool canPutEvil = nextBoard.addRandomTile(oldEvilPos, moveCount);
		if (canPutEvil) {
			MoveDirection useless = (MoveDirection)0;
			int score = maxNode(useless, nextBoard, alpha, m, totalReward, 
				depth, moveCount);
			if (score < m) {
				evilPos = oldEvilPos;
				m = score;
			}
			if (m <= alpha)
				return m;
		}
	}

	for (int i = 0; i < 16; i++) {
		if (i == oldEvilPos)
			continue;

		GameBoard nextBoard(board);
		bool canPutEvil = nextBoard.addRandomTile(i, moveCount);
		if (!canPutEvil)
			continue;

		MoveDirection useless = (MoveDirection)0;
		int score = maxNode(useless, nextBoard, alpha, m, totalReward, 
			depth, moveCount);
		if (score < m) {
			evilPos = i;
			m = score;
		}
		if (m <= alpha) {
			minNodeTT[hashBoard] = i;
			return m;
		}
	}

	minNodeTT[hashBoard] = evilPos;
	return m;
}

int Fib2584Ai::AlphaBeta::getHashBoard(GameBoard &board) const
{
	size_t result = 0;
	std::hash<int> hash_int;

	result ^= hash_int((int)board.getRow(0));
	result ^= hash_int((int)board.getRow(1));
	result ^= hash_int((int)board.getRow(2));
	result ^= hash_int((int)board.getRow(3));

	return result % TT_SIZE;
}

#include "Fib2584Ai.h"
#include <climits>
#include <cmath>
#include <sys/time.h>

Fib2584Ai::AlphaBeta::AlphaBeta(int maxTime_us, const TDLearningNew &td)
:	maxTime_us(maxTime_us), td(td)
{}
/*
int Fib2584Ai::AlphaBeta::getNewDepth(GameBoard &board) const
{
	int emptyTile = board.countEmptyTile();
	int newDepth;

	if (emptyTile <= 5)
		newDepth = depth * log(12.0) / log(5.0);
	else if (emptyTile >= 12)
		newDepth = depth;
	else
		newDepth = depth * log(12.0) / log(emptyTile);

	return newDepth;
}
*/

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
	for (int i = 0; i < 32; i++) {
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
	for (int i = 0; i < 32; i++) {
		minNode(evilPos, convertedBoard, INT_MIN, INT_MAX, 0, 
			i, moveCount);
		gettimeofday(&endTime, 0);
		if (elapsedTime(startTime, endTime) > maxTime_us)
			break;
	}
	return evilPos;
}

int Fib2584Ai::AlphaBeta::maxNode(MoveDirection &dir, const GameBoard &board, 
	int alpha, int beta, int totalReward, int depth, int moveCount) const
{
	if (depth == 0) {
		int m = alpha;
		for (int i = 0; i < 4; i++) {
			GameBoard nextBoard(board);
			int reward = nextBoard.move((MoveDirection)i);
			if (nextBoard == board)
				continue;

			int score = totalReward + reward + td.evaluateBoard(nextBoard);
			if (score > m) {
				dir = (MoveDirection)i;
				m = score;
			}
			if (m >= beta)
				return m;
		}
		return m;
	}
	else {
		int m = alpha;
		// Try last dir first
		MoveDirection oldDir = dir;
		{
			GameBoard nextBoard(board);
			int reward = nextBoard.move(oldDir);
			if (!(nextBoard == board)) {
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

			int useless = 0;
			int score = minNode(useless, nextBoard, m, beta, 
				totalReward + reward, depth - 1, moveCount + 1);
			if (score > m) {
				dir = (MoveDirection)i;
				m = score;
			}
			if (m >= beta)
				return m;
		}
		return m;
	}
}

int Fib2584Ai::AlphaBeta::minNode(int &evilPos, const GameBoard &board, 
	int alpha, int beta, int totalReward, int depth, int moveCount) const
{
	int m = beta;

	// Try old evilPos first
	int oldEvilPos = evilPos;
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
		if (m <= alpha)
			return m;
	}
	return m;
}

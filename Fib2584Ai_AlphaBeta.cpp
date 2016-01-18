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
	MoveDirection dir;
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
	int evilPos;
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
		for (int i = 0; i < 4; i++) {
			GameBoard nextBoard(board);
			int reward = nextBoard.move((MoveDirection)i);
			if (nextBoard == board)
				continue;

			int useless;
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
	for (int i = 0; i < 16; i++) {
		GameBoard nextBoard(board);
		bool canPutEvil = nextBoard.addRandomTile(i, moveCount);
		if (!canPutEvil)
			continue;

		MoveDirection useless;
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

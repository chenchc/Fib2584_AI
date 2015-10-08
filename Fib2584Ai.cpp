#include "Fib2584Ai.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <cassert>

Fib2584Ai::Fib2584Ai()
{
}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	greedy.initialize();
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4])
{
	//MoveDirection randomMove = static_cast<MoveDirection>(rand() % 4);
	//return randomMove;
	return greedy(board);
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{
	return;
}

Fib2584Ai::Greedy::Greedy()
{}

const int Fib2584Ai::Greedy::positionWeight[4][4] = {
	{15, 14, 11, 7},
	{13, 12, 9, 4},
	{10, 8, 5, 2},
	{6, 3, 1, 0}
};
void Fib2584Ai::Greedy::initialize()
{
	recornerEnable = 0;
}

MoveDirection Fib2584Ai::Greedy::operator()(int board[4][4])
{
	buildInvBoard(board);
	buildTileQueue();

	bool firstRowStuck = isRowStuck(0);
	bool firstColStuck = isColStuck(0);
	bool secondRowStuck = isRowStuck(1);
	bool secondColStuck = isColStuck(1);

	MoveDirection thirdDir;

	if (firstRowStuck && allCanMove(MOVE_RIGHT))
		thirdDir = MOVE_RIGHT;
	else if (firstColStuck && allCanMove(MOVE_DOWN))
		thirdDir = MOVE_DOWN;
	else
		thirdDir = allCanMove(MOVE_RIGHT) ? MOVE_RIGHT : MOVE_DOWN;
	/*
	if (recornerEnable) {
		recornerEnable = false;
		if (allCanMove(recornerSecond))
			return recornerSecond;
	}
	*/

	for (;;) {
		if (tileQueue.size() == 0) {
			return thirdDir;
		}
		Cor largest = tileQueue.back();
		tileQueue.pop_back();
		if (largest.num == 0) {
			return thirdDir;
		}

		/*	
		if (tileQueue.size() == 15 && largest.num >= 4 && largest.num <= 5) {
			if (recornerTest(largest.row, largest.col)) {
				return recornerFirst;
			}
		}
		*/

		// Merge first
		if (canMerge(MOVE_UP, largest.row, largest.col)) {
			return MOVE_UP;
		}
		if (canMerge(MOVE_LEFT, largest.row, largest.col)) {
			return MOVE_LEFT;
		}

		// Normal stategic move
		if (largest.num >= 5 || 
			firstColStuck && secondColStuck && largest.num >= 3) {
			if (canStrategicMove(MOVE_UP, largest.row, largest.col))
				return MOVE_UP;
		}
		if (largest.num >= 5 || 
			firstRowStuck && secondRowStuck && largest.num >= 3) {
			if (canStrategicMove(MOVE_LEFT, largest.row, largest.col))
				return MOVE_LEFT;
		}

		// Aggressive stategic move
		if (firstRowStuck && (largest.num >= 5 || 
			secondRowStuck && largest.num >= 3)) 
		{
			if (canStrategicMove(MOVE_RIGHT, largest.row, largest.col))
				return MOVE_RIGHT;
		}
		if (firstColStuck && (largest.num >= 5 || 
			secondColStuck && largest.num >= 3)) 
		{
			if (canStrategicMove(MOVE_DOWN, largest.row, largest.col))
				return MOVE_DOWN;
		}

		// Left it on edge or corner
		if (canMove(MOVE_UP, largest.row, largest.col)) {
			return MOVE_UP;
		}
		if (canMove(MOVE_LEFT, largest.row, largest.col)) {
			return MOVE_LEFT;
		}
	}	
}

void Fib2584Ai::Greedy::buildTileQueue()
{
	tileQueue.clear();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Cor cor;

			cor.row = i;
			cor.col = j;
			cor.num = invBoard[i][j];
			tileQueue.push_back(cor);
		}
	}
	std::sort(tileQueue.begin(), tileQueue.end());
}

void Fib2584Ai::Greedy::buildInvBoard(int board[4][4])
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int k;

			for (k = 0; k < 32; k++) {
				if (board[i][j] == fibonacci[k])
					break;
			}
			invBoard[i][j] = k;
		}
	}
}

inline bool invFibNeighbor(int a, int b)
{
	if (a == 0 || b == 0)
		return false;
	else if (a == 1 && b == 1)
		return true;
	else if (a - b == 1)
		return true;
	else
		return b - a == 1;
}

bool Fib2584Ai::Greedy::canMove(MoveDirection dir, int row, int col) const
{
	int i;
	
	switch (dir) {
	case MOVE_LEFT:
	case MOVE_RIGHT:
		// Look leftward
		for (i = col - 1; i >= 0; i--) {
			if (invBoard[row][i] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
				return true;
			break;
		}
		if (dir == MOVE_LEFT && i != col - 1)
			return true;

		// Look rightward
		for (i = col + 1; i < 4; i++) {
			if (invBoard[row][i] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
				return true;
			break;
		}
		if (dir == MOVE_RIGHT && i != col + 1)
			return true;

		break;
	case MOVE_UP:
	case MOVE_DOWN:
		// Look upward
		for (i = row - 1; i >= 0; i--) {
			if (invBoard[i][col] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
				return true;
			break;
		}
		if (dir == MOVE_UP && i != row - 1)
			return true;

		// Look downward
		for (i = row + 1; i < 4; i++) {
			if (invBoard[i][col] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
				return true;
			break;
		}
		if (dir == MOVE_DOWN && i != row + 1)
			return true;
		
		break;
	default:
		assert(0);
	}

	return false;
}

bool Fib2584Ai::Greedy::canMerge(MoveDirection dir, int row, int col) const
{
	int i;
	
	switch (dir) {
	case MOVE_LEFT:
	case MOVE_RIGHT:
		// Look leftward
		for (i = col - 1; i >= 0; i--) {
			if (invBoard[row][i] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
				return true;
			break;
		}

		// Look rightward
		for (i = col + 1; i < 4; i++) {
			if (invBoard[row][i] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
				return true;
			break;
		}

		break;
	case MOVE_UP:
	case MOVE_DOWN:
		// Look upward
		for (i = row - 1; i >= 0; i--) {
			if (invBoard[i][col] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
				return true;
			break;
		}

		// Look downward
		for (i = row + 1; i < 4; i++) {
			if (invBoard[i][col] == 0)
				continue;
			if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
				return true;
			break;
		}
		
		break;
	default:
		assert(0);
	}

	return false;
}

bool Fib2584Ai::Greedy::canStrategicMove(MoveDirection dir, int row, int col) 
	const
{
	int tileCount;	

	switch (dir) {
	case MOVE_UP:
	case MOVE_DOWN:
		// Count non-mergeable tile
		tileCount = countNonMergeableTile(dir, row, col);
		if (tileCount == -1)
			return false;
		// Look left col
		if (col != 0) {
			for (int i = 0; i < 4; i++) {
				if (invFibNeighbor(invBoard[row][col], invBoard[i][col - 1]) &&
					tileCount == countNonMergeableTile(dir, i, col - 1))
				{
					return true;
				}
			}
		}
		// Look right col
		if (col != 3) {
			for (int i = 0; i < 4; i++) {
				if (invFibNeighbor(invBoard[row][col], invBoard[i][col + 1]) &&
					tileCount == countNonMergeableTile(dir, i, col + 1))
				{
					return true;
				}
			}
		}
		break;
	case MOVE_LEFT:
	case MOVE_RIGHT:
		// Count non-mergeable tile
		tileCount = countNonMergeableTile(dir, row, col);
		if (tileCount == -1)
			return false;
		// Look upper col
		if (row != 0) {
			for (int i = 0; i < 4; i++) {
				if (invFibNeighbor(invBoard[row][col], invBoard[row - 1][i]) &&
					tileCount == countNonMergeableTile(dir, row - 1, i))
				{
					return true;
				}
			}
		}
		// Look lower col
		if (row != 3) {
			for (int i = 0; i < 4; i++) {
				if (invFibNeighbor(invBoard[row][col], invBoard[row + 1][i]) &&
					tileCount == countNonMergeableTile(dir, row + 1, i))
				{
					return true;
				}
			}
		}
		break;
	default:
		assert(0);
	}

	return false;
}

bool Fib2584Ai::Greedy::recornerTest(int row, int col) 
{
	if (row == 0 && col == 1 && invBoard[0][0] != 0) {
		// Count col 1 non-mergeable tile count
		int tileCount1 = countNonMergeableTile(MOVE_DOWN, 0, 1);
		int tileCount0 = 0;

		for (int i = 0; i < 4; i++) {
			if (invBoard[i][0] != 0)
				tileCount0++;
		}
		if (tileCount1 >= tileCount0) {
			recornerEnable = true;
			recornerFirst = MOVE_DOWN;
			recornerSecond = MOVE_LEFT;
			return true;
		}
	}
	else if (row == 1 && col == 0 && invBoard[0][0] != 0) {
		// Count col 1 non-mergeable tile count
		int tileCount1 = countNonMergeableTile(MOVE_RIGHT, 0, 1);
		int tileCount0 = 0;

		for (int i = 0; i < 4; i++) {
			if (invBoard[0][i] != 0)
				tileCount0++;
		}
		if (tileCount1 >= tileCount0) {
			recornerEnable = true;
			recornerFirst = MOVE_RIGHT;
			recornerSecond = MOVE_UP;
			return true;
		}
	}
	return false;
}

bool Fib2584Ai::Greedy::allCanMove(MoveDirection dir) const
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (invBoard[i][j] == 0)
				continue;
			if (canMove(dir, i, j))
				return true;
		}
	}
	return false;
}

bool Fib2584Ai::Greedy::isRowStuck(int row) const
{
	bool firstRowStuck = true;
	{
		for (int i = 0; i < 4; i++) {
			if (invBoard[row][i] == 0)
				firstRowStuck = false;
			if (canMove(MOVE_LEFT, row, i))
				firstRowStuck = false;
		}
	}
	return firstRowStuck;
}

bool Fib2584Ai::Greedy::isColStuck(int col) const
{
	bool firstColStuck = true;
	{
		for (int i = 0; i < 4; i++) {
			if (invBoard[i][col] == 0)
				firstColStuck = false;
			if (canMove(MOVE_UP, i, col))
				firstColStuck = false;
		}
	}
	return firstColStuck;
}

int Fib2584Ai::Greedy::countNonMergeableTile(MoveDirection dir, int row, 
	int col) const
{
	int tileCount = 0;

	switch (dir) {
	case MOVE_UP:
		for (int i = row - 1; i >= 0; i--) {
			if (invBoard[i][col] == 0)
				continue;
			if (canMerge(MOVE_UP, i, col))
				return -1;
			tileCount++;
		}
		break;
	case MOVE_DOWN:
		for (int i = row + 1; i < 4; i++) {
			if (invBoard[i][col] == 0)
				continue;
			if (canMerge(MOVE_DOWN, i, col))
				return -1;
			tileCount++;
		}
		break;
	case MOVE_LEFT:
		for (int i = col - 1; i >= 0; i--) {
			if (invBoard[row][i] == 0)
				continue;
			if (canMerge(MOVE_LEFT, row, i))
				return -1;
			tileCount++;
		}
		break;
	case MOVE_RIGHT:
		for (int i = col + 1; i < 4; i++) {
			if (invBoard[row][i] == 0)
				continue;
			if (canMerge(MOVE_RIGHT, row, i))
				return -1;
			tileCount++;
		}
		break;
	}

	return tileCount;
}

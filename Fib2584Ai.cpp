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

void Fib2584Ai::Greedy::initialize()
{
}

MoveDirection Fib2584Ai::Greedy::operator()(int board[4][4])
{
	buildInvBoard(board);
	buildTileQueue();

	bool firstRowStuck = isFirstRowStuck();
	bool firstColStuck = isFirstColStuck();

	MoveDirection thirdDir;

	if (firstRowStuck && allCanMove(MOVE_RIGHT))
		thirdDir = MOVE_RIGHT;
	else if (firstColStuck && allCanMove(MOVE_DOWN))
		thirdDir = MOVE_DOWN;
	else
		thirdDir = allCanMove(MOVE_RIGHT) ? MOVE_RIGHT : MOVE_DOWN;

	for (;;) {
		if (tileQueue.size() == 0) {
			return thirdDir;
		}
		Cor largest = tileQueue.back();
		tileQueue.pop_back();
		if (largest.num == 0) {
			return thirdDir;
		}
		// Merge first
		if (canMerge(MOVE_UP, largest.row, largest.col)) {
			return MOVE_UP;
		}
		if (canMerge(MOVE_LEFT, largest.row, largest.col)) {
			return MOVE_LEFT;
		}

		// Try strategic move (if the node is big enough)
		if (largest.num >= 5) {
			// Normal stategic move
			if (canStrategicMove(MOVE_UP, largest.row, largest.col))
				return MOVE_UP;
			if (canStrategicMove(MOVE_LEFT, largest.row, largest.col))
				return MOVE_LEFT;

			// Aggressive stategic move
			if (firstRowStuck && canStrategicMove(MOVE_RIGHT, largest.row, 
				largest.col))
			{
				//std::cout << "row: " << largest.row << "col: " << largest.col << std::endl;
				return MOVE_RIGHT;
			}
			if (firstColStuck && canStrategicMove(MOVE_DOWN, largest.row, 
				largest.col))
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

bool Fib2584Ai::Greedy::isFirstRowStuck() const
{
	bool firstRowStuck = true;
	{
		for (int i = 0; i < 4; i++) {
			if (invBoard[0][i] == 0)
				firstRowStuck = false;
			if (canMove(MOVE_LEFT, 0, i))
				firstRowStuck = false;
		}
	}
	return firstRowStuck;
}

bool Fib2584Ai::Greedy::isFirstColStuck() const
{
	bool firstColStuck = true;
	{
		for (int i = 0; i < 4; i++) {
			if (invBoard[i][0] == 0)
				firstColStuck = false;
			if (canMove(MOVE_UP, 0, i))
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

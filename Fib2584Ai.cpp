#include "Fib2584Ai.h"
#include <cstring>
#include <algorithm>
#include <iostream>

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
	buildTileQueue(board);
	buildInvBoard(board);

	for (;;) {
		if (tileQueue.size() == 0) {
			return static_cast<MoveDirection>(
				allCanMoveRight() ? MOVE_RIGHT : MOVE_DOWN);
		}
		Cor largest = tileQueue.back();
		tileQueue.pop_back();
		if (largest.num == 0) {
			return static_cast<MoveDirection>(
				allCanMoveRight() ? MOVE_RIGHT : MOVE_DOWN);
		}
			if (canMoveUp(largest.row, largest.col)) {
				//std::cout << largest.row << " " << largest.col << " up" << std::endl;
				return MOVE_UP;
			}
			if (canMoveLeft(largest.row, largest.col)) {
				//std::cout << largest.row << " " << largest.col << " left" << std::endl;
				return MOVE_LEFT;
			}
	}	
}

void Fib2584Ai::Greedy::buildTileQueue(int board[4][4])
{
	tileQueue.clear();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Cor cor;

			cor.row = i;
			cor.col = j;
			cor.num = board[i][j];
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
	if (a == 1 && b == 1)
		return true;
	else if (a - b == 1)
		return true;
	else
		return b - a == 1;
}

bool Fib2584Ai::Greedy::canMoveLeft(int row, int col) const
{
	int i;

	// Look leftward
	for (i = col - 1; i >= 0; i--) {
		if (invBoard[row][i] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
			return true;
		break;
	}
	if (i != col - 1)
		return true;

	// Look rightward
	for (i = col + 1; i < 4; i++) {
		if (invBoard[row][i] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
			return true;
		break;
	}

	return false;
}

bool Fib2584Ai::Greedy::canMoveRight(int row, int col) const
{
	int i;

	// Look rightward
	for (i = col + 1; i < 4; i++) {
		if (invBoard[row][i] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
			return true;
		break;
	}
	if (i != col + 1)
		return true;

	// Look leftward
	for (i = col - 1; i >= 0; i--) {
		if (invBoard[row][i] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[row][i]))
			return true;
		break;
	}

	return false;
}

bool Fib2584Ai::Greedy::allCanMoveRight() const
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (invBoard[i][j] == 0)
				continue;
			if (canMoveRight(i, j))
				return true;
		}
	}
	return false;
}

bool Fib2584Ai::Greedy::canMoveUp(int row, int col) const
{
	int i;

	// Look upward
	for (i = row - 1; i >= 0; i--) {
		if (invBoard[i][col] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
			return true;
		break;
	}
	if (i != row - 1)
		return true;

	// Look rightward
	for (i = row + 1; i < 4; i++) {
		if (invBoard[i][col] == 0)
			continue;
		if (invFibNeighbor(invBoard[row][col], invBoard[i][col]))
			return true;
		break;
	}

	return false;
}


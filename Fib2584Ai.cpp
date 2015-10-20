#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai()
{
}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	heuristic.initialize();
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4])
{
	//MoveDirection randomMove = static_cast<MoveDirection>(rand() % 4);
	//return randomMove;
	return heuristic(board);
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{
	return;
}


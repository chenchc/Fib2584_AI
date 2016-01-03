#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai(bool trainMode, bool softmaxMode, 
	const std::string &weight)
:	td(trainMode, softmaxMode, weight)
{
}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	td.initialize();
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4], int moveCount)
{
	//MoveDirection randomMove = static_cast<MoveDirection>(rand() % 4);
	//return randomMove;
	return td(board);
}

int Fib2584Ai::generateEvilMove(int board[4][4], int moveCount)
{
	/*
	int random;

	do {
		random = rand() % 16;
	} while (board[random / 4][random % 4] != 0);

	return random;
	*/
	return td.generateEvilMove(board, moveCount);
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{
	td.gameover(board);
}


#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai(const std::string &weight)
:	td(false, false, weight), ab(3, td), moveCountForPlay(0), 
	moveCountForEvil(0)
{}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	td.initialize();
	moveCountForPlay = 1;
	moveCountForEvil = 0;
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4], int moveCount)
{
	//MoveDirection randomMove = static_cast<MoveDirection>(rand() % 4);
	//return randomMove;
	return ab.generateMove(board, moveCountForPlay++);
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
	
	if (moveCountForEvil == 0) {
		moveCountForEvil++;
		return 5; // First random tile
	}
	else
		return ab.generateEvilMove(board, moveCountForEvil++);
	
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{
	//td.gameover(board);
}


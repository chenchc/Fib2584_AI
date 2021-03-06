#include <iostream>
#include <cstdlib>
#include "Fib2584/GameBoard.h"
#include "Fib2584/Typedefs.h"
#include "Fib2584/Statistic.h"
#include "Fib2584Ai.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc == 1) {
		cerr << "usage: play_game rounds [other arguments which your AI needs]" << endl;
		return 1;
	}
	int iPlayRounds = atoi(argv[1]);
	// create and initialize AI
	Fib2584Ai ai(true);
	Fib2584Ai ai_evil(false, true, "weight.dat");
	ai.initialize(argc, argv);
	ai_evil.initialize(argc, argv);

	// initialize statistic data
	Statistic statistic;
	statistic.setStartTime();
	// play each round
	for(int i = 0;i < iPlayRounds;i++) {
		GameBoard gameBoard;
		gameBoard.initialize();
		int iScore = 0;
		int arrayBoard[4][4];
		int moveIndex = 0;

		// First random
		gameBoard.getArrayBoard(arrayBoard);
		gameBoard.addRandomTile(ai_evil.generateEvilMove(arrayBoard, moveIndex), 
			moveIndex);
		// Second random
		moveIndex++;
		gameBoard.getArrayBoard(arrayBoard);
		gameBoard.addRandomTile(ai_evil.generateEvilMove(arrayBoard, moveIndex), 
			moveIndex);
		
		while(!gameBoard.terminated()) {
			gameBoard.getArrayBoard(arrayBoard);
			MoveDirection moveDirection = ai.generateMove(arrayBoard, 
				moveIndex);

			GameBoard originalBoard = gameBoard;
			iScore += gameBoard.move(moveDirection);
			if(originalBoard == gameBoard) {
				printf("Repeated\n");
				exit(1);
			}
			statistic.increaseOneMove();

			moveIndex++;
			gameBoard.getArrayBoard(arrayBoard);
			gameBoard.addRandomTile(ai_evil.generateEvilMove(arrayBoard, moveIndex), 
				moveIndex);
		}
		gameBoard.getArrayBoard(arrayBoard);
		ai.gameOver(arrayBoard, iScore);
		statistic.increaseOneGame();

		// update statistic data
		statistic.updateScore(iScore);
		statistic.updateMaxTile(gameBoard.getMaxTile());
	}
	statistic.setFinishTime();

	// output statistic data
	statistic.show();

	return 0;
}

// This class is based on GameBoard, obviously.
// I have to create a new copy but not inherit that because EVERY DATA MEMBER
// IS PRIVATE and some essential member functions need adding.

#ifndef __GAMEBOARDFORAI_H__
#define __GAMEBOARDFORAI_H__

#include <iostream>
#include "Fib2584/Typedefs.h"
#include "Fib2584/MoveTable.h"
#include "Fib2584/BitBoard.h"
#include "Fib2584/Random.h"

using namespace std;

class GameBoardForAI
{
public:
	static const int fibonacci_[32];
private:
	static Random random_;
public:
	GameBoardForAI();
	GameBoardForAI(const GameBoardForAI &src); // Added
	GameBoardForAI(const int board[4][4]); // Added

	void initialize();
	int move(MoveDirection moveDirection);
	void addRandomTile();
	bool terminated();
	void getArrayBoard(int board[4][4]);
	int getMaxTile();
	void showBoard();
	bool operator==(GameBoardForAI gameBoard);
	BitBoard getRow(int row); // Move to public
	BitBoard getColumn(int column); // Move to public
private:
	BitBoard restoreRow(BitBoard rowBits, int row);
	BitBoard restoreColumn(BitBoard columnBits, int column);
	int countEmptyTile();
	int getTile(int row, int column);
	int getFibonacci(int index);
private:
	BitBoard board_;
};

#endif

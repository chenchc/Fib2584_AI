#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

#include <iostream>
#include "Fib2584/Typedefs.h"
#include "Fib2584/MoveTable.h"
#include "Fib2584/BitBoard.h"
#include "Fib2584/Random.h"

using namespace std;

class GameBoard
{
public:
	static const int fibonacci_[32];
private:
	static Random random_;
public:
	GameBoard();
	GameBoard(const GameBoard &src); // Added
	GameBoard(const int board[4][4]); // Added

	void initialize();
	int move(MoveDirection moveDirection);
	bool addRandomTile(int pos, int moveIndex); // Not random any more
	bool terminated();
	void getArrayBoard(int board[4][4]);
	int getMaxTile();
	void showBoard();
	bool operator==(GameBoard gameBoard);
	BitBoard getRow(int row); // Move to public
	BitBoard getColumn(int column); // Move to public
	int countEmptyTile();
private:
	BitBoard restoreRow(BitBoard rowBits, int row);
	BitBoard restoreColumn(BitBoard columnBits, int column);
	int getTile(int row, int column);
	int getFibonacci(int index);
private:
	BitBoard board_;
};

#endif

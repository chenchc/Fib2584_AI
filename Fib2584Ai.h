#ifndef __FIB2584AI_H__
#define __FIB2584AI_H__

#include <cstdlib>
#include <ctime>
#include <vector>
#include "Fib2584/MoveDirection.h"

class Fib2584Ai
{
public:
	Fib2584Ai();
	// initialize ai
	void initialize(int argc, char* argv[]);
	// generate one move
	MoveDirection generateMove(int board[4][4]);
	// do some action when game over
	void gameOver(int board[4][4], int iScore);

private:
	class Greedy
	{
	public:
		Greedy();
		void initialize();
		MoveDirection operator()(int board[4][4]);
	private:
		struct Cor {
			int row, col;
			int num;

			bool operator<(const Cor &rhs) const
			{
				return num < rhs.num;
			}
		};

		const int positionWeight[4][4] = 
			{{4, 3, 2, 1},
			 {3, 0, 0, 0},
			 {2, 0, 0, 0},
			 {1, 0, 0, 0}};
		const int fibonacci[32]	= {0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 
			233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 
			46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 
			2178309};

		int invBoard[4][4];
		std::vector<Cor> tileQueue;

		void buildTileQueue();
		void buildInvBoard(int board[4][4]);
		bool canMoveLeft(int row, int col) const;
		bool canMoveRight(int row, int col) const;
		bool allCanMoveRight() const;
		bool canMoveUp(int row, int col) const;
		bool canMoveDown(int row, int col) const;
		bool allCanMoveDown() const;
	};
	
	Greedy greedy;
};

#endif

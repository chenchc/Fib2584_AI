#ifndef __FIB2584AI_H__
#define __FIB2584AI_H__

#include "Fib2584/MoveDirection.h"
#include "Fib2584/BitBoard.h"
#include "GameBoardForAI.h"
#include <cstdlib>
#include <ctime>
#include <stack>
#include <string>
#include <vector>

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
	class Heuristic
	{
	public:
		Heuristic();
		void initialize();
		MoveDirection operator()(int board[4][4]);
	private:
		static const int positionWeight[4][4];

		struct Cor {
			int row, col;
			int num;

			bool operator<(const Cor &rhs) const
			{
				if (num == rhs.num)
					return positionWeight[row][col] <
						positionWeight[rhs.row][rhs.col];
				else
					return num < rhs.num;
			}
		};

		const int fibonacci[32]	= {0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 
			89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 
			28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 
			1346269, 2178309};

		int invBoard[4][4];
		std::vector<Cor> tileQueue;
		bool recornerEnable;
		MoveDirection recornerFirst, recornerSecond;

		void buildTileQueue();
		void buildInvBoard(int board[4][4]);
		bool canMove(MoveDirection dir, int row, int col) const;
		bool canMerge(MoveDirection dir, int row, int col) const;
		bool canStrategicMove(MoveDirection dir, int row, int col) const;
		bool recornerTest(int row, int col); 
		bool allCanMove(MoveDirection dir) const;
		bool isRowStuck(int row) const;
		bool isColStuck(int col) const;
		int countNonMergeableTile(MoveDirection dir, int row, int col) const;
	};

	class TDLearning
	{
	public:
		TDLearning(bool trainMode = true, 
			const std::string &filename = std::string("weight.dat"));
		~TDLearning();
		void initialize();
		MoveDirection operator()(const int board[4][4]);
		void gameover(const int board[4][4]);
	private:
		struct FeatureBoard {
			FeatureBoard() {}
			FeatureBoard(GameBoardForAI &board, int reward);
			FeatureBoard(const FeatureBoard &src);

			unsigned int outer[4];
			unsigned int inner[4];
			int reward;	// Scaled reward
		};

		const int SCALE = 32768;	// alpha = 1 / 128

		std::string filename;
		bool trainMode;
		int *weightOuter;
		int *weightInner;
		std::stack<FeatureBoard> record;

		int getFeatureBoardValue(const FeatureBoard &feature) const;
		unsigned int reverseFeature(unsigned int a) const;
	};

	TDLearning td;
};


#endif

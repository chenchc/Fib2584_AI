#ifndef __FIB2584AI_H__
#define __FIB2584AI_H__

#include "Fib2584/Typedefs.h"
#include "Fib2584/BitBoard.h"
#include "Fib2584/GameBoard.h"
#include <cstdlib>
#include <ctime>
#include <stack>
#include <string>
#include <vector>

const int NUM_BAND = 8;

class Fib2584Ai
{
public:
	Fib2584Ai(bool trainMode = false, bool softmaxMode = false, 
		const std::string &weight = "weight.dat");
	// initialize ai
	void initialize(int argc, char* argv[]);
	// generate one move
	MoveDirection generateMove(int board[4][4], int moveCount);
	int generateEvilMove(int board[4][4], int moveCount);
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
			FeatureBoard(GameBoard &board, int reward);
			FeatureBoard(const FeatureBoard &src);

			unsigned int outer[4];
			unsigned int inner[4];
			int reward;	// Scaled reward
		};

		const int SCALE = 32768;

		std::string filename;
		bool trainMode;
		int *weightOuter;
		int *weightInner;
		std::stack<FeatureBoard> record;

		int getFeatureBoardValue(const FeatureBoard &feature) const;
		unsigned int reverseFeature(unsigned int a) const;
	};

	class TDLearningNew
	{
	public:
		TDLearningNew(bool trainMode = false, bool softmaxMode = false,
			const std::string &filename = std::string("weight.dat"));
		~TDLearningNew();
		void initialize();
		MoveDirection operator()(const int board[4][4]);
		int generateEvilMove(int board[4][4], int moveCount);
		void gameover(const int board[4][4]);
	private:

		struct FeatureBoard {
			FeatureBoard() {}
			FeatureBoard(GameBoard &board, int reward);
			FeatureBoard(const FeatureBoard &src);
			void applyBandMaskOnFeature(unsigned int *bandSet, 
				unsigned int first, unsigned int second) const;

			//int numTile[32];					// [tileType]
			unsigned int outer[4][NUM_BAND];	// [dir][band]
			unsigned int inner[2][NUM_BAND];	// [dir][band]
			int reward;	// Scaled reward
		};

		const int SCALE = 32768;

		std::string filename;
		bool trainMode;
		bool softmaxMode;
		//int weightNumTile[32][16];
		int *weightOuter[NUM_BAND];
		int *weightInner[NUM_BAND];
		std::stack<FeatureBoard> record;

		long long getFeatureBoardValue(const FeatureBoard &feature) const;
		inline unsigned int reverseFeature_rotate(unsigned int a) const;
		inline unsigned int reverseFeature_44(unsigned int a) const;
		inline unsigned int reverseFeature_2222(unsigned int a) const;
		void adjustWeight(const FeatureBoard &feature, int adjust);
		int softmaxSelect(int *score, int size) const;
		int maxSelect(int *score, int size) const;
	};

	TDLearningNew td;
};


#endif

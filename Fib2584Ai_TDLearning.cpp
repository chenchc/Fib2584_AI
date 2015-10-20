#include "Fib2584Ai.h"
#include <fstream>
using namespace std;

Fib2584Ai::TDLearning::TDLearning(bool trainMode, const std::string &filename)
:	filename(filename), 
	trainMode(trainMode)
{
	const int featureNum = 32 * 32 * 32 * 32;
	ifstream fin(filename.c_str(), ifstream::in | ifstream::binary);

	weightOuter = new int[featureNum](); 
	weightInner = new int[featureNum](); 
	if (!fin.fail()) {
		fin.read((char *)weightOuter, featureNum * sizeof(int));
		fin.read((char *)weightInner, featureNum * sizeof(int));
	}
}

Fib2584Ai::TDLearning::~TDLearning()
{
	const int featureNum = 32 * 32 * 32 * 32;

	if (trainMode) {
		ofstream fout(filename.c_str(), ifstream::out | ifstream::binary | 
			ifstream::trunc);

		fout.write((char *)weightOuter, featureNum * sizeof(int));
		fout.write((char *)weightInner, featureNum * sizeof(int));
	}
	delete [] weightOuter;
	delete [] weightInner;
}

MoveDirection Fib2584Ai::TDLearning::operator()(const int board[4][4])
{
	
}

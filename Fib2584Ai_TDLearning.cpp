#include "Fib2584Ai.h"
#include <fstream>
#include <iostream>
#include <climits>
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

void Fib2584Ai::TDLearning::initialize()
{
}

MoveDirection Fib2584Ai::TDLearning::operator()(const int board[4][4])
{
	GameBoardForAI initBoard(board);
	MoveDirection bestDir;
	int bestValuePlusReward = INT_MIN;
	int bestReward;
	FeatureBoard bestFeature;

	// Find the best direction to move
	for (int dir = 0; dir < 4; dir++) {
		GameBoardForAI newBoard(initBoard);
		int reward = newBoard.move((MoveDirection)dir) * SCALE;
		if (newBoard == initBoard)
			continue;
		FeatureBoard newFeature(newBoard, reward);
		int valuePlusReward = getFeatureBoardValue(newFeature) + reward;
		if (valuePlusReward > bestValuePlusReward) {
			bestDir = (MoveDirection)dir;
			bestValuePlusReward = valuePlusReward;
			bestReward = reward;
			bestFeature = newFeature;
		}
	}
	
	// Push the direction into record
	if (trainMode) {
		record.push(bestFeature);
	}

	return bestDir;
}

void Fib2584Ai::TDLearning::gameover()
{
	const int alpha = 1;

	if (trainMode) {
		FeatureBoard nextFeature = record.top();
		record.pop();

		while (!record.empty()) {
			FeatureBoard feature = record.top();
			record.pop();

			int delta = getFeatureBoardValue(nextFeature) + nextFeature.reward -
				getFeatureBoardValue(feature);

			for (int i = 0; i < 4; i++) {
				weightOuter[feature.outer[i]] += alpha * delta / SCALE;
				
				unsigned int rev = reverseFeature(feature.outer[i]);
				if (rev != feature.outer[i]) {
					weightOuter[rev] += alpha * delta / SCALE;
				}
			}
			for (int i = 0; i < 4; i++) {
				weightInner[feature.inner[i]] += alpha * delta / SCALE;
				
				unsigned int rev = reverseFeature(feature.inner[i]);
				if (rev != feature.inner[i]) {
					weightInner[rev] += alpha * delta / SCALE;
				}
			}
			nextFeature = feature;
		}
	}
}

Fib2584Ai::TDLearning::FeatureBoard::FeatureBoard(GameBoardForAI &board, 
	int reward)
:	reward(reward)
{
	outer[0] = (unsigned int)board.getRow(0);
	inner[0] = (unsigned int)board.getRow(1);
	inner[1] = (unsigned int)board.getRow(2);
	outer[1] = (unsigned int)board.getRow(3);
	outer[2] = (unsigned int)board.getColumn(0);
	inner[2] = (unsigned int)board.getColumn(1);
	inner[3] = (unsigned int)board.getColumn(2);
	outer[3] = (unsigned int)board.getColumn(3);
}

Fib2584Ai::TDLearning::FeatureBoard::FeatureBoard(const FeatureBoard &src)
:	reward(src.reward)
{
	for (int i = 0; i < 4; i++) {
		outer[i] = src.outer[i];
		inner[i] = src.inner[i];
	}
}

int Fib2584Ai::TDLearning::getFeatureBoardValue(const FeatureBoard &feature) 
	const
{
	int value = 0;

	for (int i = 0; i < 4; i++) {
		value += weightOuter[feature.outer[i]];
	}
	for (int i = 0; i < 4; i++) {
		value += weightInner[feature.inner[i]];
	}

	return value;
}

unsigned int Fib2584Ai::TDLearning::reverseFeature(unsigned int a)
{
	unsigned int result = 0;

	for (int i = 0; i < 4; i++) {
		result = (result << 5) + (a & 0x1f);
		a >>= 5;
	}
	
	return result;
}

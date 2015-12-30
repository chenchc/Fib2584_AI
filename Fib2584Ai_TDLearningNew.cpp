#include "Fib2584Ai.h"
#include <fstream>
#include <iostream>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
using namespace std;

Fib2584Ai::TDLearningNew::TDLearningNew(bool trainMode, bool softmaxMode,
	const std::string &filename)
:	filename(filename), 
	trainMode(trainMode),
	softmaxMode(softmaxMode)
{
	const int featureNum = 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8;
	ifstream fin(filename.c_str(), ifstream::in | ifstream::binary);

	/*
	for (int tileType = 0; tileType < 32; tileType++) {
		memset(weightNumTile[tileType], 0, 16 * sizeof(int));
		if (!fin.fail()) {
			fin.read((char *)weightNumTile[tileType], 16 * sizeof(int));
		}
	}
	*/
	for (int band = 0; band < NUM_BAND; band++) {
		weightOuter[band] = new int[featureNum](); 
		if (!fin.fail()) {
			fin.read((char *)weightOuter[band], featureNum * sizeof(int));
		}
	}
	for (int band = 0; band < NUM_BAND; band++) {
		weightInner[band] = new int[featureNum](); 
		if (!fin.fail()) {
			fin.read((char *)weightInner[band], featureNum * sizeof(int));
		}
	}
}

Fib2584Ai::TDLearningNew::~TDLearningNew()
{
	const int featureNum = 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8;

	if (trainMode) {
		ofstream fout(filename.c_str(), ifstream::out | ifstream::binary | 
			ifstream::trunc);
		/*
		for (int tileType = 0; tileType < 32; tileType++) {
			fout.write((char *)weightNumTile[tileType], 16 * sizeof(int));
		}
		*/
		for (int band = 0; band < NUM_BAND; band++) {
			fout.write((char *)weightOuter[band], featureNum * sizeof(int));
			delete [] weightOuter[band];
		}
		for (int band = 0; band < NUM_BAND; band++) {
			fout.write((char *)weightInner[band], featureNum * sizeof(int));
			delete [] weightInner[band];
		}
	}
}

void Fib2584Ai::TDLearningNew::initialize()
{
}

MoveDirection Fib2584Ai::TDLearningNew::operator()(const int board[4][4])
{
	GameBoard initBoard(board);
	FeatureBoard initFeature(initBoard, 0);
	long long initValue = getFeatureBoardValue(initFeature);
	FeatureBoard feature[4];
	int score[4];
	
	// Find the best direction to move
	for (int dir = 0; dir < 4; dir++) {
		GameBoard newBoard(initBoard);
		int reward = newBoard.move((MoveDirection)dir) * SCALE;
		if (newBoard == initBoard) {
			score[dir] = INT_MIN;
			continue;
		}
		feature[dir] = FeatureBoard(newBoard, reward);
		long long valuePlusReward = getFeatureBoardValue(feature[dir]) + 
			reward;
		score[dir] = valuePlusReward - initValue;
	}

	MoveDirection bestDir;
	if (softmaxMode)
		bestDir = (MoveDirection)softmaxSelect(score, 4);
	else
		bestDir = (MoveDirection)maxSelect(score, 4);

	const FeatureBoard &bestFeature = feature[bestDir];

	// Push the direction into record
	if (trainMode) {
		record.push(bestFeature);
	}

	return bestDir;
}

int Fib2584Ai::TDLearningNew::generateEvilMove(int board[4][4], int moveCount)
{
	GameBoard initBoard(board);
	FeatureBoard initFeature(initBoard, 0);
	long long initValue = getFeatureBoardValue(initFeature);
	int score[16];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (board[i][j] != 0) {
				score[i * 4 + j] = INT_MIN;
				continue;
			}

			// Place the random tile
			GameBoard beforeBoard(initBoard);
			beforeBoard.addRandomTile(i * 4 + j, moveCount);

			// Select the best for opponent
			long long bestValuePlusReward = INT_MIN;
			for (int dir = 0; dir < 4; dir++) {
				GameBoard afterBoard(beforeBoard);
				int reward = afterBoard.move((MoveDirection)dir) * SCALE;
				if (afterBoard == initBoard)
					continue;
				FeatureBoard afterFeature(afterBoard, reward);
				long long valuePlusReward = 
					getFeatureBoardValue(afterFeature) + reward;
				if (valuePlusReward > bestValuePlusReward) {
					bestValuePlusReward = valuePlusReward;
				}
			}

			// Renew score
			score[i * 4 + j] = -(bestValuePlusReward - initValue);
		}
	}

	int worstPlace;
	if (softmaxMode)
		worstPlace = softmaxSelect(score, 16);
	else
		worstPlace = maxSelect(score, 16);

	return worstPlace;
}

void Fib2584Ai::TDLearningNew::adjustWeight(const FeatureBoard &feature, 
	int adjust)
{
	/*
	for (int i = 0; i < 32; i++) {
		weightNumTile[i][feature.numTile[i]] += adjust;
	}
	*/
	for (int i = 0; i < 4; i++) {
		for (int band = 0; band < NUM_BAND; band++) {
			unsigned int ori = feature.outer[i][band];
			weightOuter[band][ori] += adjust;

			unsigned int rev2222 = reverseFeature_2222(feature.outer[i][band]);
			if (rev2222 != ori)
				weightOuter[band][rev2222] += adjust;
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int band = 0; band < NUM_BAND; band++) {
			unsigned int ori = feature.inner[i][band];
			unsigned int rev44 = reverseFeature_44(feature.inner[i][band]);
			unsigned int rev2222 = reverseFeature_rotate(rev44);
			unsigned int revRotate = reverseFeature_44(rev2222);

			weightInner[band][ori] += adjust;
			if (ori == rev44) {
				if (ori == rev2222) 
					;	// Do nothing
				else {
					weightInner[band][rev2222] += adjust;
				}
			}
			else {
				if (ori == rev2222) {
					weightInner[band][rev44] += adjust;
				}
				else if (ori == revRotate) {
					weightInner[band][rev44] += adjust;
				}
				else {
					weightInner[band][rev44] += adjust;
					weightInner[band][rev2222] += adjust;
					weightInner[band][revRotate] += adjust;
				}
			}
		}
	}
}

void Fib2584Ai::TDLearningNew::gameover(const int board[4][4])
{
	const int alpha = 4;

	if (trainMode) {
		FeatureBoard nextFeature;

		// Value of endBoard should be 0
		{
			GameBoard endBoard(board);
			FeatureBoard feature(endBoard, 0);
			long long delta = -getFeatureBoardValue(feature);
						
			adjustWeight(feature, alpha * delta / SCALE);			

			nextFeature = feature;	
		}

		while (!record.empty()) {
			FeatureBoard feature = record.top();
			record.pop();

			long long delta = getFeatureBoardValue(nextFeature) + nextFeature.reward 
				- getFeatureBoardValue(feature);

			adjustWeight(feature, alpha * delta / SCALE);

			nextFeature = feature;
		}
	}
}

Fib2584Ai::TDLearningNew::FeatureBoard::FeatureBoard(GameBoard &board, 
	int reward)
:	reward(reward)
{
	unsigned int row[4];
	unsigned int col[4];

	for (int i = 0; i < 4; i++) {
		row[i] = board.getRow(i);
	}
	for (int i = 0; i < 4; i++) {
		col[i] = board.getColumn(i);
	}

	// numTile
	/*
	memset((char *)numTile, 0, 32 * sizeof(int));
	for (int i = 0; i < 4; i++) {
		unsigned int rowCopy = row[i];
		for (int j = 0; j < 4; j++) {
			unsigned char tile = (rowCopy & 0x1f);
			numTile[tile]++;
			rowCopy >>= 5;
		}
	}
	*/

	// outer & inner
	applyBandMaskOnFeature(outer[0], row[0], row[1]);
	applyBandMaskOnFeature(outer[1], row[3], row[2]);
	applyBandMaskOnFeature(outer[2], col[0], col[1]);
	applyBandMaskOnFeature(outer[3], col[3], col[2]); 
	applyBandMaskOnFeature(inner[0], row[1], row[2]);
	applyBandMaskOnFeature(inner[1], col[1], col[2]);
}

Fib2584Ai::TDLearningNew::FeatureBoard::FeatureBoard(const FeatureBoard &src)
:	reward(src.reward)
{
	//memcpy(numTile, src.numTile, 32 * sizeof(int));

	for (int i = 0; i < 4; i++)
		for (int band = 0; band < NUM_BAND; band++)
			outer[i][band] = src.outer[i][band];
	for (int i = 0; i < 2; i++)
		for (int band = 0; band < NUM_BAND; band++)
			inner[i][band] = src.inner[i][band];
}

long long Fib2584Ai::TDLearningNew::getFeatureBoardValue(
	const FeatureBoard &feature) const
{
	long long value = 0;

	/*
	for (int i = 0; i < 32; i++) {
		value += weightNumTile[i][feature.numTile[i]];
	}
	*/
	for (int i = 0; i < 4; i++) {
		for (int band = 0; band < NUM_BAND; band++)
			value += weightOuter[band][feature.outer[i][band]];
	}
	for (int i = 0; i < 2; i++) {
		for (int band = 0; band < NUM_BAND; band++)
			value += weightInner[band][feature.inner[i][band]];
	}

	return value;
}

inline unsigned int Fib2584Ai::TDLearningNew::reverseFeature_rotate
	(unsigned int a) const
{
	unsigned int result = 0;

	for (int i = 0; i < 8; i++) {
		result = (result << 3) + (a & 0x7);
		a >>= 3;
	}

	return result;
}

inline unsigned int Fib2584Ai::TDLearningNew::reverseFeature_44(unsigned int a) 
	const
{
	return (a >> 12) + ((a & 0xfff) << 12);
}

inline unsigned int Fib2584Ai::TDLearningNew::reverseFeature_2222
	(unsigned int a) const
{
	return reverseFeature_rotate(
		reverseFeature_44(a));
}

inline unsigned char applyBandMask(unsigned char tile, int bandIndex)
{
	int temp = bandIndex * 2;

	if (tile == 0)
		return 0;
	else if (tile <= temp + 1)
		return 1;
	else if (tile >= temp + 7)
		return 7;
	else
		return tile - temp;
}

void Fib2584Ai::TDLearningNew::FeatureBoard::applyBandMaskOnFeature(
	unsigned int *bandSet, unsigned int first, unsigned int second) const 
{
	for (int band = 0; band < NUM_BAND; band++)
		bandSet[band] = 0;

	for (int i = 0; i < 4; i++, first <<= 5) {
		unsigned char tile = ((first & 0xf8000) >> 15);

		for (int band = 0; band < NUM_BAND; band++) 
			bandSet[band] = (bandSet[band] << 3) + applyBandMask(tile, band);
	}
	for (int i = 0; i < 4; i++, second <<= 5) {
		unsigned char tile = ((second & 0xf8000) >> 15);

		for (int band = 0; band < NUM_BAND; band++) 
			bandSet[band] = (bandSet[band] << 3) + applyBandMask(tile, band);
	}
}

int Fib2584Ai::TDLearningNew::softmaxSelect(int *score, int size) const
{
	double *expScore = new double[size];
	const double tau = 2.0;
	double expScoreTotal = 0.0;
	
	for (int i = 0; i < size; i++) {
		if (score[i] == INT_MIN)
			expScore[i] = 0;
		else {
			expScore[i] = exp((double)score[i] / (double)SCALE / tau);
			if (expScore[i] == 0)
				expScore[i] = DBL_MIN;
			//cout << expScore[i] << endl;
			expScoreTotal += expScore[i];
		}
	}
	//cin.get();

	double random = (double)rand() / RAND_MAX * expScoreTotal;
	double expScoreCum = 0.0;
	for (int i = 0; i < size; i++) {
		if (expScore[i] == 0)
			continue;
		expScoreCum += expScore[i];
		if (expScoreCum >= random) {
			delete [] expScore;
			return i;
		}
	}

	assert(false);
	return -1;
}

int Fib2584Ai::TDLearningNew::maxSelect(int *score, int size) const
{
	int result;
	int maxScore = INT_MIN;
	
	for (int i = 0; i < size; i++) {
		if (score[i] > maxScore) {
			maxScore = score[i];
			result = i;
		}
	}

	return result;
}

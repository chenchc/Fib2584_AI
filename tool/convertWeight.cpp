#include <iostream>
#include <cstdlib>
#include <climits>
using namespace std;

int main(int argc, char **argv)
{
	int *input;
	int *output;

	if (argc != 3)
		return 1;

	input = new int[32 * 32 * 32 * 32 * 2];
	output = new int[32 * 32 * 32 * 32 * 2];
	cin.read((char *)input, (int)(32 * 32 * 32 * 32 * 2 * sizeof(int)));
	for (int i = 0; i < 32 * 32 * 32 * 32 * 2; i++) {
		double rel;
		rel = (double)input[i];
		rel = rel * (double)atoi(argv[2]) / (double)atoi(argv[1]);
		output[i] = (int)rel;
	}
	cout.write((char *)output, (int)(32 * 32 * 32 * 32 * 2 * sizeof(int)));

	delete [] input;
	delete [] output;

	return 0;
}

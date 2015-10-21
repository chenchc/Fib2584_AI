all:
	g++ -std=c++11 -O3 -march=native Fib2584/*.cpp *.cpp -o play_game

debug:
	g++ -std=c++11 -g Fib2584/*.cpp *.cpp -o play_game

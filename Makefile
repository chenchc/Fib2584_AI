all:
	g++ -std=c++11 -Ofast -march=native -I . Fib2584/*.cpp *.cpp main/old.cpp -o old
	g++ -std=c++11 -Ofast -march=native -I . Fib2584/*.cpp *.cpp main/client_main.cpp -o ai_client
	g++ -std=c++11 -Ofast -march=native -I . Fib2584/*.cpp *.cpp main/main.cpp -o ai_stdio

debug:
	g++ -std=c++11 -g -march=native Fib2584/*.cpp *.cpp main/old.cpp -o old
	g++ -std=c++11 -g -march=native Fib2584/*.cpp *.cpp main/client_main.cpp -o ai_client
	g++ -std=c++11 -g -march=native Fib2584/*.cpp *.cpp main/main.cpp -o ai_stdio

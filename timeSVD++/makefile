all: run.sh
clear:
	rm *.o
run.sh: main.o
	g++ -g main.o -o run.sh
main.o: main.cpp SVD.h SVD.cpp
	g++ -g -std=c++0x -c main.cpp SVD.cpp
	


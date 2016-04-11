all: hw1

hw1: hw1.cpp
	g++ hw1.cpp -Wall -ohw1 -lX11 -lGL -lGLU -lglut -lm

clean:
	rm -f hw1
	rm -f *.o

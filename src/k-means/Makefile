CC=g++ 
CC_WIN=i586-mingw32msvc-g++
LN=g++ 
LN_WIN=i586-mingw32msvc-g++
PARAM=-std=c++0x -Wall -Wfatal-errors -O2
all:
	$(CC) $(PARAM) -c kmeans.cc
	$(LN) -o kmeans kmeans.o
clean:
	rm *.o

CC=g++ 
CC_WIN=i586-mingw32msvc-g++
LN=g++ 
LN_WIN=i586-mingw32msvc-g++
#Check if system is 64 bits
#If so, use the 64bits libraries
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	LIBS=../lib/glnxa64
else
	LIBS=../lib/glnx86
endif
INCLUDE=../include
PARAM=-std=c++0x -I$(INCLUDE) -Wall -Wfatal-errors -O2
#Standard version
all:
	$(CC) $(PARAM) -c main.cc
	$(CC) $(PARAM) -c tools/math.cc
	$(CC) $(PARAM) -c k-means/kmeans.cc
	$(CC) $(PARAM) -c tools/Bitmap.cc
	$(CC) $(PARAM) -c tools/Movie.cc
	$(CC) $(PARAM) -c lsh/lsh.cc
	$(LN) -o vash main.o math.o kmeans.o lsh.o Movie.o Bitmap.o -L$(LIBS) -Wl,-rpath=$(LIBS) -lvl -lavformat -lavcodec -lswscale -lavutil
#This one is for DEBUG purposes only (includes viewer)
debug_gl:
	$(CC) $(PARAM) -DDEBUG -c main.cc
	$(CC) $(PARAM) -DDEBUG -c tools/math.cc
	$(CC) $(PARAM) -DDEBUG -c k-means/kmeans.cc
	$(CC) $(PARAM) -DDEBUG -c tools/viewer.cc
	$(CC) $(PARAM) -DDEBUG -c tools/Bitmap.cc
	$(CC) $(PARAM) -DDEBUG -c tools/Movie.cc
	$(CC) $(PARAM) -DDEBUG -c lsh/lsh.cc
	$(LN) -o vash main.o math.o kmeans.o lsh.o viewer.o Movie.o Bitmap.o -L$(LIBS) -Wl,-rpath=$(LIBS) -lvl -lGL -lglut -lavformat -lavcodec -lswscale
clean:
	rm *.o

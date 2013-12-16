CC=g++ 
CC_WIN=i586-mingw32msvc-g++
LN=g++ 
LN_WIN=i586-mingw32msvc-g++
#Check if system is 64 bits
#If so, use the 64bits libraries
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	LIBS=../../lib/glnxa64
else
	LIBS=../../lib/glnx86
endif
INCLUDE=../../include
PARAM=-std=c++0x -I$(INCLUDE) -Wall -Wfatal-errors -g
#Standard version
all:
	$(CC) $(PARAM) -c kmeans.cc
	$(CC) $(PARAM) -c ../tools/Bitmap.cc
	$(CC) $(PARAM) -c ../lsh/lsh.cc
	$(LN) -o kmeans kmeans.o lsh.o Bitmap.o -L$(LIBS) -Wl,-rpath=$(LIBS) -lvl
#This one is for DEBUG purposes only (includes viewer)
debug_gl:
	$(CC) $(PARAM) -DDEBUG -c kmeans.cc
	$(CC) $(PARAM) -DDEBUG -c ../tools/viewer.cc
	$(CC) $(PARAM) -DDEBUG -c ../tools/Bitmap.cc
	$(CC) $(PARAM) -DDEBUG -c ../lsh/lsh.cc
	$(LN) -o kmeans kmeans.o lsh.o viewer.o Bitmap.o -L$(LIBS) -Wl,-rpath=$(LIBS) -lvl -lGL -lglut
clean:
	rm *.o

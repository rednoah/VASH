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
PARAM=-std=c++0x -I$(INCLUDE) -Wall -Wfatal-errors -O2
all:
	$(CC) $(PARAM) -c kmeans.cc
	$(LN) -o kmeans kmeans.o -L$(LIBS) -Wl,-rpath=$(LIBS) -lvl
clean:
	rm *.o

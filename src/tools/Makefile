#Example on how to compile decodeVideo.c
CC=gcc 
CC_WIN=i586-mingw32msvc-gcc
LN=gcc 
LN_WIN=i586-mingw32msvc-gcc
PARAM=-Wall -Wfatal-errors
all:
	$(CC) $(PARAM) -c decodeVideo.c
	$(LN) -o decodeVideo decodeVideo.o -lavformat -lavcodec -lswscale
clean:
	rm *.o

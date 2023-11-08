CC=clang
INCS=-I/usr/include
LIBS=-L/usr/libs
CFLAGS=-std=c99 -lc -lcurses  -Wall -Wextra -pedantic -g

all:
	$(CC) src/main.c $(INCS) $(LIBS) $(CFLAGS) -o msxx
format:
	indent src/main.c

CC=gcc
INCS=-I/usr/include
LIBS=-L/usr/libs
CFLAGS=-std=c89 -lc -lcurses  -Wall -Wextra -pedantic -g

all:
	$(CC) src/main.c $(INCS) $(LIBS) $(CFLAGS) -o msxx
format:
	clang-format -style=GNU src/main.c > main.c
	mv main.c src/main.c

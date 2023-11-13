CC = gcc

WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith \
						-Wcast-align -Wwrite-strings -Wmissing-prototypes \
						-Wmissing-declarations -Wredundant-decls -Wnested-externs \
						-Winline -Wuninitialized -Wconversion -Wstrict-prototypes \
						-pedantic

CFLAGS = -std=c99 -O3 $(WARNINGS)

LIBS = -L/usr/lib
INCS = -I/usr/include/
LNKS = -lc -lcurses

all:
	$(CC) $(CFLAGS) src/main.c -c
	$(CC) $(CFLAGS) src/msxx.c -c
	$(CC) $(CFLAGS) src/mslog.c -c
	$(CC) $(CFLAGS) src/key.c -c
	$(CC) $(CFLAGS) src/handle.c -c
	$(CC) $(CFLAGS) src/move.c -c
	$(CC) $(CFLAGS) src/scr.c -c
	$(CC) $(CFLAGS) src/grid.c -c
	$(CC) $(CFLAGS) $(LIBS) $(INCS) $(LNKS) main.o msxx.o handle.o mslog.o scr.o grid.o move.o key.o -o msxx
  

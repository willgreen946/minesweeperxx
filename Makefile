CC    = gcc
INCS  = -I/usr/include
LIBS  = -L/usr/lib
LINKS = -lc -lcurses

WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith \
						-Wcast-align -Wwrite-strings -Wmissing-prototypes \
						-Wmissing-declarations -Wredundant-decls -Wnested-externs \
						-Winline -Wuninitialized -Wconversion -Wstrict-prototypes \
						-pedantic

CFLAGS = -std=c99 $(WARNINGS)

all: EXE 
	
EXE: OBJECTS
	$(CC) $(INCS) $(LIBS) $(LINKS) main.o move.o mslog.o screen.o config.o -o msxx 

OBJECTS:
	$(CC) $(CFLAGS) src/main.c -c
	$(CC) $(CFLAGS) src/move.c -c
	$(CC) $(CFLAGS) src/mslog.c -c
	$(CC) $(CFLAGS) src/screen.c -c
	$(CC) $(CFLAGS) src/config.c -c

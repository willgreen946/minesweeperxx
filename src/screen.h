#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <curses.h>
#include "config.h"
#include "mslog.h"

/*
 * Used to keep a map of all the chars on the grid.
 * holding data about the part of the grid,
 * Like weather it's a bomb and been selected previously
 */
struct XGRID {
	bool bomb;
	bool selected;
	bool flagged;
};

/*
 * Represents the Y axis of the game grid
 * Should be declared as an array in SCR
 * Holds an array of the X axis
 */
struct YGRID {
	struct XGRID xgrd[GRD_X];
};

typedef struct {
	int maxx;
	int maxy;
	int cury;
	int curx;
  int flagc;
  int bombc;
  float seconds;
	struct YGRID ygrd[GRD_Y];
	WINDOW *win;
  WINDOW *datawin;
} SCR;

/*
 * Draws the game grid,
 * generates bombs on the grid
 */
void drawgrid(SCR *);

/*
 * Updates the time elapsed,
 * the bomb count, and the flag count
 */
void updategamewin(SCR *);

/*
 * Creates a sub window for the game data,
 * this is the box on the side that has the time,
 * and other information inside of it
 */
int initgamewin(SCR *);

/*
 * Initialises color in curses
 */
int initcolor(void);

/*
 * Initialises SCR struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (scrdestroy)
 */
SCR * scrinit(void);

/*
 * Ends the curses session,
 * frees all memory with SCR *
 */
int scrdestroy(SCR *);

#endif /* __SCREEN_H__ */

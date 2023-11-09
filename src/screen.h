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
	struct YGRID ygrd[GRD_Y];
	WINDOW *win;
} SCR;

/*
 * Initialises SCR struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (scrdestroy)
 */
SCR * scrinit(void);

int scrdestroy(SCR *);

#endif /* __SCREEN_H__ */

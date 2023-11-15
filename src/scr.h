#ifndef __SCR_H__
#define __SCR_H__

#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include "mslog.h"
#include "config.h"

struct XGRID {
  bool mine;
  bool selected;
  bool flagged;
  int count;
};

struct YGRID {
  struct XGRID xgrd[GRD_X];
};

typedef struct {
  int maxx;
  int maxy;
  int cury;
  int curx;
  int gmwiny;
  int gmwinx;
  int minec;
  int flagc;
  WINDOW * win;
  WINDOW * gmwin;
  struct YGRID ygrd[GRD_Y];
}SCR;

/*
 * Initialises curses.
 * Allocates memory for SCR *.
 * Returns new SCR *.
 */
SCR * scrinit(void);

/*
 * Ends curses,
 * Frees memory in SCR *
 */
int scrdestroy(SCR *);

/*
 * Initialises the game window
 */
int scrinitgmwin(SCR *);

void scrdestroygmwin(WINDOW *); 

#endif /* __SCR_H__ */

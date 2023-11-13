#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include "scr.h"
#include "mslog.h"
#include "config.h"
#include "grid.h"

void
griddraw(SCR * s)
{
  int y;
  int x;
  int yrand;
  int xrand;

  srand((unsigned int) time(0));

  for (y = 0; y < GRD_Y; y++) {
    for (x = 0; x < GRD_X; x++) {
      mvwprintw(s->win, y, x, "#");
      s->ygrd[y].xgrd[x].mine = false;
      s->ygrd[y].xgrd[x].selected = false;
      s->ygrd[y].xgrd[x].flagged = false;
    }
  }

  for (y = 0; y < MINES; y++) {
    yrand = (int) rand() % GRD_Y;
    xrand = (int) rand() % GRD_X;
    s->ygrd[yrand].xgrd[xrand].mine = true;
  }

  if (wrefresh(s->win) == ERR)
    mslog("Failed to refresh (win)");
}

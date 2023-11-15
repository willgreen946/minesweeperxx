#include <curses.h>
#include "scr.h"
#include "mslog.h"
#include "msxx.h"
#include "config.h"
#include "scan.h"
#include "handle.h"

/*
 * Handles when the user hits enter on a cell 
 */
int
handlesel(SCR * s)
{
  /* If the user hits a mine die */
  if (s->ygrd[s->cury].xgrd[s->curx].mine) {
    msdeathscr(s);
    return 1;
  }

  /* Do nothing if the cells been selected */
  if (s->ygrd[s->cury].xgrd[s->curx].selected)
    return 0;

  /* Start scanning the grid */
  scangrid(s);
  return 0;
}

/*
 * A screen for when the user wins
 * This should be move out of this file
 */
static void
victoryscr(SCR * s)
{
  if (wclear(s->gmwin) == ERR)
    mslog("Failed to clear (gmwin)");

  if (box(s->gmwin, 0, 0) == ERR)
    mslog("Failed to draw box (gmwin)");

  mvwprintw(s->gmwin, 1, 1, "You Flagged All the Mines");
  
  if (wrefresh(s->gmwin) == ERR)
    mslog("Failed to refresh (gmwin)");

  wgetch(s->win);
}

/*
 * Handles when the user decides to flag a cell
 */
int
flaghandle(SCR * s)
{
  /* Undo a flag, revert a cell */
  if (s->ygrd[s->cury].xgrd[s->curx].flagged) {
    mvwprintw(s->win, s->cury, s->curx, "#");
    s->ygrd[s->cury].xgrd[s->curx].flagged = false;
    s->flagc--;

    if (wmove(s->win, s->cury, s->curx) == ERR)
      mslog("Failed to move cursor (win)");

    return 0;
  }

  if (s->ygrd[s->cury].xgrd[s->curx].selected)
    return 0; /* If the cells been selected do nothing */

  s->ygrd[s->cury].xgrd[s->curx].flagged = true;
  s->flagc++;

  /* flags match the mines */
  if (s->flagc == s->minec) {
    victoryscr(s);
    return 1;
  }

  mvwprintw(s->win, s->cury, s->curx, "F");

  if (wmove(s->win, s->cury, s->curx) == ERR)
    mslog("Failed to move cursor (win)");

  return 0;
}

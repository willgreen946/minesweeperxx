#include <curses.h>
#include "scr.h"
#include "config.h"
#include "mslog.h"
#include "scan.h"

/*
 * A horribly long function,
 * Goes along all of the grid,
 * it stops scanning when it finds a mine count
 */
void
scangrid(SCR * s)
{
  int y = s->cury;
  int x = s->curx;

  /* Go up the grid */
  for (y = s->cury; y > 0; y--) {
    /* Go <- this way, stop on a count */
    for (x = s->curx; x > 0; x--) {
      if (s->ygrd[y].xgrd[x].selected)
        ;
      else if (s->ygrd[y].xgrd[x].mine)
        ; 
      else if (s->ygrd[y].xgrd[x].count == 0)
        mvwprintw(s->win, y, x, " ");
      else {
        mvwprintw(s->win, y, x, "%d", s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
      s->ygrd[y].xgrd[x].selected = true;
    }

    /* Go -> this way, stop on a count */
    for (x = s->curx; x < GRD_X; x++) {
      if (s->ygrd[y].xgrd[x].selected)
        ;
      else if (s->ygrd[y].xgrd[x].mine)
        ;
      else if (s->ygrd[y].xgrd[x].count == 0)
        mvwprintw(s->win, y, x, " ");
      else {
        mvwprintw(s->win, y, x, "%d", s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
      s->ygrd[y].xgrd[x].selected = true;
    }

    s->ygrd[y].xgrd[x].selected = true;
  } 

  /* Go down the grid */
  for (y = s->cury; y < GRD_Y; y++) {
    /* Go <- this way, stop on a count */
    for (x = s->curx; x > 0; x--) {
      if (s->ygrd[y].xgrd[x].selected)
        ;
      else if (s->ygrd[y].xgrd[x].mine)
        ;
      else if (s->ygrd[y].xgrd[x].count == 0)
        mvwprintw(s->win, y, x, " ");
      else {
        mvwprintw(s->win, y, x, "%d", s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
      s->ygrd[y].xgrd[x].selected = true;
    }

    /* Go -> this way, stop on a count */
    for (x = s->curx; x < GRD_X; x++) {
      if (s->ygrd[y].xgrd[x].selected)
        ;
      else if (s->ygrd[y].xgrd[x].mine)
        ;
      else if (s->ygrd[y].xgrd[x].count == 0)
        mvwprintw(s->win, y, x, " ");
      else {
        mvwprintw(s->win, y, x, "%d", s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
      s->ygrd[y].xgrd[x].selected = true;
    }

    s->ygrd[y].xgrd[x].selected = true;
  } 


  wmove(s->win, s->cury, s->curx);
}

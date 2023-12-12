#include <curses.h>
#include "scr.h"
#include "config.h"
#include "mslog.h"
#include "scan.h"

static void printcount(WINDOW*, int, int, int);

/*
 * Prints the count in color
 * A horrible function, I apologise
 */
static void
printcount(WINDOW * w, int y, int x, int count)
{
  switch (count) {
    case 1:
    case 4:
    case 6:
      if (has_colors()) {
        wattron(w, COLOR_PAIR(YEL));
        mvwprintw(w, y, x, "%d", count);
        wattroff(w, COLOR_PAIR(YEL));
      } else {
        mvwprintw(w, y, x, "%d", count);
      }
      break;
    case 2:
    case 7:
      if (has_colors()) {
        wattron(w, COLOR_PAIR(GRE));
        mvwprintw(w, y, x, "%d", count);
        wattroff(w, COLOR_PAIR(GRE));
      } else {
        mvwprintw(w, y, x, "%d", count);
      }
      break;
    case 3:
    case 5:
      if (has_colors()) {
        wattron(w, COLOR_PAIR(RED));
        mvwprintw(w, y, x, "%d", count);
        wattroff(w, COLOR_PAIR(RED));
      } else {
        mvwprintw(w, y, x, "%d", count);
      }
      break;
    case 8:
      if (has_colors()) {
        wattron(w, COLOR_PAIR(PUR));
        mvwprintw(w, y, x, "%d", count);
        wattroff(w, COLOR_PAIR(PUR));
      } else {
        mvwprintw(w, y, x, "%d", count);
      }
      break;
  }
}

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

      else if (s->ygrd[y].xgrd[x].count == 0) {
        mvwprintw(s->win, y, x, " ");
        s->ygrd[y].xgrd[x].selected = true;
      }

      else {
        printcount(s->win, y, x, s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
    }

    /* Go -> this way, stop on a count */
    for (x = s->curx; x < GRD_X; x++) {
      if (s->ygrd[y].xgrd[x].selected)
        ;

      else if (s->ygrd[y].xgrd[x].mine)
        ;

      else if (s->ygrd[y].xgrd[x].count == 0) {
        mvwprintw(s->win, y, x, " ");
        s->ygrd[y].xgrd[x].selected = true;
      }

      else {
        printcount(s->win, y, x, s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
    }
  } 

  /* Go down the grid */
  for (y = s->cury; y < GRD_Y; y++) {
    /* Go <- this way, stop on a count */
    for (x = s->curx; x > 0; x--) {
      if (s->ygrd[y].xgrd[x].selected)
        ;

      else if (s->ygrd[y].xgrd[x].mine)
        ;

      else if (s->ygrd[y].xgrd[x].count == 0) {
        mvwprintw(s->win, y, x, " ");
        s->ygrd[y].xgrd[x].selected = true;
      }

      else {
        printcount(s->win, y, x, s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
    }

    /* Go -> this way, stop on a count */
    for (x = s->curx; x < GRD_X; x++) {
      if (s->ygrd[y].xgrd[x].selected)
        ;

      else if (s->ygrd[y].xgrd[x].mine)
        ;

      else if (s->ygrd[y].xgrd[x].count == 0) {
        mvwprintw(s->win, y, x, " ");
        s->ygrd[y].xgrd[x].selected = true;
      }

      else {
        printcount(s->win, y, x, s->ygrd[y].xgrd[x].count);
        s->ygrd[y].xgrd[x].selected = true;
        break;
      }
    }
  } 

  if (wmove(s->win, s->cury, s->curx) == ERR)
    mslog(CURSOR, __func__);
}

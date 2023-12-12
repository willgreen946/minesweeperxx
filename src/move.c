#include <curses.h>
#include "mslog.h"
#include "config.h"
#include "move.h"

void
moveup(WINDOW * w, int * y, int x)
{
  if ((*y - 1) >= 0)
    *y -= 1;

  if (wmove(w, *y, x) == ERR)
    mslog(REFRESH, __func__);

  if (wrefresh(w) == ERR)
    mslog(REFRESH, __func__);
}

void
movedown(WINDOW * w, int * y, int x)
{
  if (*y < (GRD_Y - 1))
    *y += 1;

  if (wmove(w, *y, x) == ERR)
    mslog(MOVE, __func__);

  if (wrefresh(w) == ERR)
    mslog(REFRESH, __func__);
}

void
moveleft(WINDOW * w, int y, int * x)
{
  if ((*x - 1) >= 0)
    *x -= 1;

  if (wmove(w, y, *x) == ERR)
    mslog(MOVE, __func__);

  if (wrefresh(w) == ERR)
    mslog(REFRESH, __func__);
}

void
moveright(WINDOW * w, int y, int * x)
{
  if (*x < (GRD_X - 1))
    *x += 1;

  if (wmove(w, y, *x) == ERR)
    mslog(MOVE, __func__);

  if (wrefresh(w) == ERR)
    mslog(REFRESH, __func__);
}

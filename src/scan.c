#include <curses.h>
#include "scr.h"
#include "config.h"
#include "mslog.h"
#include "scan.h"

static int north(struct YGRID*, int, int);
static int northeast(struct YGRID*, int, int);
static int northwest(struct YGRID*, int, int);
static int south(struct YGRID*, int, int);
static int southeast(struct YGRID*, int, int);
static int southwest(struct YGRID*, int, int);
static int east(struct YGRID*, int, int);
static int west(struct YGRID*, int, int);

static int
north(struct YGRID * ygrd, int y, int x)
{
  int n = 0;  

  if ((y - 1) < 0)
    return 0;

  /* Scan directly north */
  if (ygrd[y - 1].xgrd[x].mine)
    n++;

  return n;
}

static int
northeast(struct YGRID * ygrd, int y, int x)
{
  int ne = 0; 

  if ((y - 1) < 0 || (x + 1) > GRD_X)
    return 0;

  /* Scan the north east */
  if (ygrd[y - 1].xgrd[x + 1].mine)
    ne++;

  return ne;
}

static int
northwest(struct YGRID * ygrd, int y, int x)
{
  int nw = 0; 

  if ((y - 1) < 0 || (x - 1) < 0)
    return 0;

  /* Scan the north west */
  if (ygrd[y - 1].xgrd[x - 1].mine)
    nw++;

  return nw;
}

static int
east(struct YGRID * ygrd, int y, int x)
{
  int e = 0;  

  if ((x + 1) > GRD_X)
    return 0;

  /* Scan directly east */
  if (ygrd[y].xgrd[x + 1].mine)
    e++;

  return e;
}

static int
south(struct YGRID * ygrd, int y, int x)
{
  int s = 0;  

  if ((y + 1) > GRD_Y)
    return 0;

  /* Scan directly south */
  if (ygrd[y + 1].xgrd[x].mine)
    s++;

  return s;
}

static int
southeast(struct YGRID * ygrd, int y, int x)
{
  int se = 0; 

  if ((y + 1) > GRD_Y || (x + 1) > GRD_X)
    return 0;

  /* Scan the south east */
  if (ygrd[y + 1].xgrd[x + 1].mine)
    se++;

  return se;
}

static int
southwest(struct YGRID * ygrd, int y, int x)
{
  int sw = 0;

  if ((y + 1) > GRD_Y || (x - 1) < 0)
    return 0;

  /* Scan the south west */
  if (ygrd[y + 1].xgrd[x - 1].mine)
    sw++;

  return sw;
}

static int
west(struct YGRID * ygrd, int y, int x)
{
  int w = 0;  

  if ((x - 1) < 0)
    return 0;

  /* Scan directly west */
  if (ygrd[y].xgrd[x - 1].mine)
    w++;

  return w;
}

/*
 * Scans the grid looking for mines 
 */
void
scangrid(SCR * s)
{
  int tmp;
  int count = 0; 
  int y = s->cury;
  int x = s->curx;

  /* Look around the north of the cell */
  if (!(tmp = north(s->ygrd, y, x))) {
    if ((y - 1) > 0) {
      s->ygrd[y - 1].xgrd[x].selected = true;
      mvwprintw(s->win, y - 1, x, "~");
    }
  }

  count += tmp;

  /* Look around the north east of the cell */
  if (!(tmp = northeast(s->ygrd, y, x))) {
    if ((y - 1) > 0 && (x + 1) < GRD_X) {
      s->ygrd[y - 1].xgrd[x + 1].selected = true;
      mvwprintw(s->win, y - 1, x + 1, "~");
    }
  }

  count += tmp;

  /* Look around the north west of the cell */
  if (!(tmp = northwest(s->ygrd, y, x))) {
    if ((y - 1) > 0 && (x - 1) > 0) {
      s->ygrd[y - 1].xgrd[x - 1].selected = true;
      mvwprintw(s->win, y - 1, x - 1, "~");
    }
  }

  count += tmp;

  /* Look around the south of the cell */
  if (!(tmp = south(s->ygrd, y, x))) {
    if ((y + 1) < GRD_Y) {
      s->ygrd[y + 1].xgrd[x].selected = true;
      mvwprintw(s->win, y + 1, x, "~");
    }
  }

  count += tmp;

  /* Look around the south east of the cell */
  if (!(tmp = southeast(s->ygrd, y, x))) {
    if ((y + 1) < GRD_Y && (x + 1) < GRD_X) {
      s->ygrd[y + 1].xgrd[x + 1].selected = true;
      mvwprintw(s->win, y + 1, x + 1, "~");
    }
  }

  count += tmp;

  /* Look around the south west of the cell */
  if (!(tmp = southwest(s->ygrd, y, x))) {
    if ((y + 1) < GRD_Y && (x - 1) > 0) {
      s->ygrd[y + 1].xgrd[x - 1].selected = true;
      mvwprintw(s->win, y + 1, x - 1, "~");
    }
  }

  count += tmp;

  /* Look around the east of the cell */
  if (!(tmp = east(s->ygrd, y, x))) {
    if ((x + 1) < GRD_X) {
      s->ygrd[y].xgrd[x + 1].selected = true;
      mvwprintw(s->win, y, x + 1, "~");
    }
  }

  count += tmp;

  /* Look around the west of the cell */
  if (!(tmp = west(s->ygrd, y, x))) {
    if ((x - 1) > 0) {
      s->ygrd[y].xgrd[x - 1].selected = true;
      mvwprintw(s->win, y, x - 1, "~");
    }
  }

  count += tmp;

  if (count > 0)
    mvwprintw(s->win, y, x, "%d", count);
  else
    mvwprintw(s->win, y, x, "~");

  if (wmove(s->win, y, x) == ERR)
    mslog("Failed to move cursor (win)");
}

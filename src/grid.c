#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include "scr.h"
#include "mslog.h"
#include "config.h"
#include "grid.h"

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

  if ((y - 1) <= 0)
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

  if ((y - 1) <= 0 || (x + 1) >= GRD_X)
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

  if ((y - 1) <= 0 || (x - 1) <= 0)
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

  if ((x + 1) >= GRD_X)
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

  if ((y + 1) >= GRD_Y)
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

  if ((y + 1) >= GRD_Y || (x + 1) >= GRD_X)
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

  if ((y + 1) >= GRD_Y || (x - 1) <= 0)
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

  if ((x - 1) <= 0)
    return 0;

  /* Scan directly west */
  if (ygrd[y].xgrd[x - 1].mine)
    w++;

  return w;
}

void
griddraw(SCR * s)
{
  int y;
  int x;
  int yrand;
  int xrand;
  int count = 0;

  srand((unsigned int) time(0));

  /* Draw the grid and set everything to false */
  for (y = 0; y < GRD_Y; y++) {
    for (x = 0; x < GRD_X; x++) {
      mvwprintw(s->win, y, x, "#");
      s->ygrd[y].xgrd[x].mine = false;
      s->ygrd[y].xgrd[x].selected = false;
      s->ygrd[y].xgrd[x].flagged = false;
      s->ygrd[y].xgrd[x].count = 0;
    }
  }

  /* Generate mines */
  for (y = 0; y < MINES; y++) {
    yrand = (int) rand() % GRD_Y;
    xrand = (int) rand() % GRD_X;
    s->ygrd[yrand].xgrd[xrand].mine = true;
  }

  /* Calculate the mine counters */
  for (y = 0; y < GRD_Y; y++) {
    for (x = 0; x < GRD_X; x++) {
      /* Get a mine count for each cell */
      if (!s->ygrd[y].xgrd[x].mine) {
        /* Scan north */
        count += north(s->ygrd, y, x);
        count += northeast(s->ygrd, y, x);
        count += northwest(s->ygrd, y, x);

        /* Scan south */
        count += south(s->ygrd, y, x);
        count += southeast(s->ygrd, y, x);
        count += southwest(s->ygrd, y, x);

        /* Scan east */
        count += east(s->ygrd, y, x);

        /* Scan west */
        count += west(s->ygrd, y, x);

        /* Set the mine count */
        s->ygrd[y].xgrd[x].count = count;
      }

      count = 0;
    }
  }
  
  if (wrefresh(s->win) == ERR)
    mslog(REFRESH, __func__);
}

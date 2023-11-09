#include <curses.h>
#include "config.h"
#include "move.h"

void
moveup(WINDOW * win, int * y, int x)
{
	if ((*y - 1) >= 0)
		*y-=1;

	wmove(win, *y, x);
	wrefresh(win);
}

void
movedown(WINDOW * win, int * y, int x)
{
	if (*y < (GRD_Y - 1))
		*y+=1;

	wmove(win, *y, x);
	wrefresh(win);
}

void
moveleft(WINDOW * win, int y, int * x)
{
	if ((*x - 1) >= 0)
		*x-=1;

	wmove(win, y, *x);
	wrefresh(win);
}

void
moveright(WINDOW * win, int y, int * x)
{
	if (*x < (GRD_X - 1))
		*x+=1;

	wmove(win, y, *x);
	wrefresh(win);
}

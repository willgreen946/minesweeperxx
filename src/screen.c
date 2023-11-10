#include <curses.h>
#include "config.h"
#include "mslog.h"
#include "screen.h"

/*
 * Draws the onscreen grid according to GRD_Y & GRD_X
 * Generates X (BOMBS) amount of bombs,
 * and places them in the grid
 */
void
drawgrid(SCR * scr)
{
	int y;
  int x;
	int yrand;
  int xrand;

	srand((unsigned int)time(0));
	
	/* Draws the grid and randomly genrates bombs */
	for (y = 0; y < GRD_Y; y++) {
		for (x = 0; x < GRD_X; x++) {
			mvwprintw(scr->win, y, x, "#");
			scr->ygrd[y].xgrd[x].bomb = false;
			scr->ygrd[y].xgrd[x].selected = false;
			scr->ygrd[y].xgrd[x].flagged = false;
		}
	}

	/* Set the bombs, using y as a counter */
	for (y = 0; y < BOMBS; y++) {
		yrand = (int)rand() % GRD_Y;
		xrand = (int)rand() % GRD_X;
		scr->ygrd[yrand].xgrd[xrand].bomb = true;
		/**mvwprintw(scr->win, yrand, xrand, "B");*/
	}

	if (wrefresh(scr->win) == ERR)
    mslog("Failed to refresh win");
}

void
updategamewin(SCR * scr)
{

}

/*
 * Creates a subwindow in the corner of the game screen
 * Decorates it and goes from there
 */
int
initgamewin(SCR * scr)
{
  if (!(scr->datawin
      = subwin(scr->win, WIN_Y - 2, WIN_X / 2,
        (scr->maxy / 2) - ((WIN_Y / 2) - 1),
        (scr->maxx / 2) - ((WIN_X / 2) - 36)))) {

    mslog("FATAL:Failed to init datawin");
    return EXIT_FAILURE;
  }

  /* Set the background colors */
  if (has_colors()) {
		if (wbkgd(scr->win, COLOR_PAIR(WIN_BOX)))
			mslog("Failed to set background color of win");

    if (wbkgd(scr->datawin, COLOR_PAIR(ERR_BOX)))
			mslog("Failed to set background color of datawin");
	}

  if (box(scr->datawin, 0, 0))
    mslog("Failed to draw a box around datawin");

  drawgrid(scr);
 
  if (wrefresh(scr->datawin) == ERR)
    mslog("Failed to refresh datawin");

  if (wrefresh(scr->win) == ERR)
    mslog("Failed to refresh win");

  return EXIT_SUCCESS;
}

/*
 * Initialises all of the color pairs for the program
 * It also initialises color as a whole for curses
 */
int
initcolor(void)
{
	if (has_colors()) {
		if (start_color() == ERR)
			return EXIT_FAILURE;

		if (init_pair(WIN_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
			return EXIT_FAILURE;

		if (init_pair(MSG_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
			return EXIT_FAILURE;

		if (init_pair(ERR_BOX, COLOR_WHITE, COLOR_RED) == ERR)
			return EXIT_FAILURE;

		if (init_pair(BAK_BOX, COLOR_WHITE, COLOR_RED) == ERR)
			return EXIT_FAILURE;
	} else {
		mslog("SCR does not support color");
	}

	return EXIT_SUCCESS;
}
/*
 * Initialises SCR struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (scrdestroy)
 */
SCR *
scrinit(void)
{
	SCR *tmp = (SCR *) 0;

	if (!(tmp = (SCR *) malloc(sizeof(*tmp))))
		return (SCR *) 0;

	if (!(stdscr = initscr())) {
		mslog("FATAL:Failed to init curses");
		return (SCR *) 0;
	}

	getmaxyx(stdscr, tmp->maxy, tmp->maxx);

	if (tmp->maxy < WIN_Y || tmp->maxx < WIN_X) {
		mslog("FATAL:SCR too small to function, need at least 80x24");
		return (SCR *) 0;
	}

	/* Should print the new window in the center of the screen */
	if (!(tmp->win
	    = newwin(WIN_Y, WIN_X, (tmp->maxy / 2) - (WIN_Y / 2),
	    (tmp->maxx / 2) - (WIN_X / 2)))) {

		mslog("FATAL:Failed to create window");
		return (SCR *) 0;
	}

	return tmp;
}

/*
 * Ends curses and frees memory with SCR struct
 */
int
scrdestroy(SCR * scr)
{
	if (endwin() == ERR) {
		mslog("Failed to end curses session");
		return EXIT_FAILURE;
	}

	if (!scr) {
		mslog("SCR * is not NULL");
		return EXIT_FAILURE;
	}

	free(scr);
	return EXIT_SUCCESS;
}

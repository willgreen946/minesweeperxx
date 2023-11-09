#include <curses.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "screen.h"
#include "mslog.h"
#include "move.h"

int mainmenu(SCR *);

bool
resizecheck(SCR * scr)
{
	int y, x;

	getmaxyx(scr->win, y, x);

	if (y < WIN_Y || x < WIN_X)
		err(EXIT_FAILURE, "SCR too small to function");

	if (y != scr->maxy || x != scr->maxy)
		return true;

	return false;
}

/*
 * 8 if statments to see if the 8 surrounding blocks
 * are bombs, if they are bombs the count is incremented by 1
 * the count is printed to the current cursor position
 * The count is returned by the function
 */
int
bombsurroundcount(SCR * scr, int y, int x)
{
	int count = 0;

	if (scr->ygrd[y].xgrd[x + 1].bomb)
		count++;

	if (scr->ygrd[y].xgrd[x - 1].bomb)
		count++;

	if (scr->ygrd[y + 1].xgrd[x].bomb)
		count++;

	if (scr->ygrd[y + 1].xgrd[x + 1].bomb)
		count++;

	if (scr->ygrd[y + 1].xgrd[x - 1].bomb)
		count++;

	if (scr->ygrd[y - 1].xgrd[x].bomb)
		count++;

	if (scr->ygrd[y - 1].xgrd[x + 1].bomb)
		count++;

	if (scr->ygrd[y - 1].xgrd[x - 1].bomb)
		count++;

	return count;
}

void
scanxgrid(SCR * scr, int y, int min, bool inc)
{
	int x;
	int count = 0;

	/* Go along the X axis <- this way */
	for (x = scr->curx; x != min;) {
		if (scr->ygrd[y].xgrd[x].selected)
			break;	/* Do nothing if its already been selected */

		else if (scr->ygrd[y].xgrd[x].bomb)
			break;	/* Do nothing if its a bomb */

		else if ((count = bombsurroundcount(scr, y, x)) > 0) {
			scr->ygrd[y].xgrd[x].selected = true;
			mvwprintw(scr->win, y, x, "%d", count);
			return;
		} else {
			scr->ygrd[y].xgrd[x].selected = true;
			mvwprintw(scr->win, y, x, "~");

			if (inc)
				x++;
			else
				x--;
		}
	}
}
/*
 * Goes up and down the grid scanning each char right to left
 * If the char has already been selected do nothing,
 * If the scan encounters a bomb it will not change the char
 * If the char has no bombs around it, it will print a #
 * If the char has bombs around it, it will print the count
 */
void
scanygrid(SCR * scr, int min, bool inc)
{
	int y;

	for (y = scr->cury; y != min;) {
		/* Go along the X axis <- this way */
		scanxgrid(scr, y, 0, false);

		/* Go along the X axis -> this way */
		scanxgrid(scr, y, GRD_X, true);

		if (inc)
			y++;
		else
			y--;
	}
}
/*
 * If the user selects a bomb, go back to the main menu
 * Else it should scan the grid to replace chars
 */
int
selecthandle(SCR * scr)
{
	/* If the user selects a bomb */
	if (scr->ygrd[scr->cury].xgrd[scr->curx].bomb) {
		mvwprintw(scr->win, WIN_Y / 2, WIN_X / 2, "BOOM!!!!!!");
		mvwprintw(scr->win, scr->cury, scr->curx, "*");
		wrefresh(scr->win);
		wgetch(scr->win);
		return EXIT_FAILURE;
	} else {
		/* Go down the grid */
		scanygrid(scr, GRD_Y, true);

		/* Go up the grid */
		scanygrid(scr, 0, false);

		wrefresh(scr->win);
	}

	return EXIT_SUCCESS;
}
/*
 * Handles the processing of marking a block a bomb
 */
int
flaghandle(SCR * scr)
{
	if (!scr->ygrd[scr->cury].xgrd[scr->curx].flagged) {
		mvwprintw(scr->win, scr->cury, scr->curx, "F");
		wrefresh(scr->win);
		scr->ygrd[scr->cury].xgrd[scr->curx].flagged = true;
	}
	 /* If the sqaure is already flagged do nothing */ 
	else
		return EXIT_SUCCESS;

	return EXIT_SUCCESS;
}

int
parsekey(SCR * scr, int c)
{
	switch (c) {
	case KEY_UP:
	case 'k':
		moveup(scr->win, &scr->cury, scr->curx);
		return EXIT_SUCCESS;
	case KEY_DOWN:
	case 'j':
		movedown(scr->win, &scr->cury, scr->curx);
		return EXIT_SUCCESS;
	case KEY_LEFT:
	case 'h':
		moveleft(scr->win, scr->cury, &scr->curx);
		return EXIT_SUCCESS;
	case KEY_RIGHT:
	case 'l':
		moveright(scr->win, scr->cury, &scr->curx);
		return EXIT_SUCCESS;
	case RET:
	case 'e':
		return selecthandle(scr);
	case SPC:
	case 'f':
		return flaghandle(scr);
	}

	return EXIT_SUCCESS;
}
/*
 * Draws the onscreen grid according to GRD_Y & GRD_X
 * Generates X (BOMBS) amount of bombs,
 * and places them in the grid
 */
int
drawgrid(SCR * scr)
{
	int y, x;
	int yrand, xrand;

	srand(time(0));

	if (has_colors()) {
		if (wbkgd(scr->win, COLOR_PAIR(WIN_BOX))) {
			mslog("Failed to set background color");
			return EXIT_FAILURE;
		}
	}

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

	/* Reset cursor position */
	scr->cury = GRD_Y / 2;
	scr->curx = GRD_X / 2;
	wmove(scr->win, scr->cury, scr->curx);
	wrefresh(scr->win);
	return EXIT_SUCCESS;
}
/*
 * The main loop of the program.
 * Takes input from getch and passes it off to the key parser
 * It reads until an escape char is sent.
 */
void
eventloop(SCR * scr)
{
	int c;

	if (drawgrid(scr))
		return;

	while ((c = wgetch(scr->win)) != ESC) {
		if (parsekey(scr, c))
			break;
	}

	if (wclear(scr->win) == ERR) {
		mslog("Failed to clear window");
		return;
	}
	mainmenu(scr);
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
 * Prints the game title to the screen
 * Looks in a hardcoded path
 */
void
printtitle(SCR * scr)
{
	int y;
	char buf[1024];
	FILE *fp = (FILE *) NULL;

	if (!(fp = fopen("./assets/title.txt", "r")))
		return;

	for (y = 2; fgets(buf, 1024, fp); y++) {
		mvwprintw(scr->win, y, (GRD_X / 10), "%s", buf);
	}

	wrefresh(scr->win);
	fclose(fp);
}

int
mainmenu(SCR * scr)
{
	int c;

	if (has_colors()) {
		if (wbkgd(scr->win, COLOR_PAIR(WIN_BOX)))
			mslog("Failed to set background color");
	}
	if (curs_set(0) == ERR)
		mslog("Failed to hide cursor");

	printtitle(scr);

	mvwprintw(scr->win, (WIN_Y / 2) - (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(1) PLAY"), "(1) PLAY");

	mvwprintw(scr->win, (WIN_Y / 2) + (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(2) QUIT"), "(2) QUIT");

	wrefresh(scr->win);

	box(scr->win, 0, 0);

	while ((c = wgetch(scr->win)))
		if (c == '1' || c == '2')
			break;

	if (curs_set(1) == ERR)
		mslog("Failed to show cursor");

	if (wclear(scr->win) == ERR)
		mslog("Failed to clear window");

	if (c == '1')
		eventloop(scr);

	return EXIT_SUCCESS;
}

int
setup(void)
{
	SCR *scr = (SCR *) NULL;

	if (!(scr = scrinit())) {
		mslog("FATAL:Failed to init SCR struct");
    return EXIT_FAILURE;
  }

	if (raw() == ERR)
		mslog("Failed to enter raw mode");

	if (noecho() == ERR)
		mslog("Failed to disable echoing");

	if (initcolor())
		mslog("Failed to init color");

	if (keypad(scr->win, TRUE) == ERR)
		mslog("Failed to init keypad");

	if (has_colors()) {
		if (wbkgd(stdscr, COLOR_PAIR(BAK_BOX)))
			mslog("Failed to set background color");
	}

	if (box(stdscr, 0, 0) == ERR)
		mslog("Failed to draw box");

	if (wrefresh(stdscr))
		mslog("Failed to refresh window");

	if (mainmenu(scr))
		return EXIT_FAILURE;

	return scrdestroy(scr);
}

int
main(void)
{
	return setup();
}

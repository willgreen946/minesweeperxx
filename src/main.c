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
int bombsurroundcount(SCR *, int, int);
void scanxgrid(SCR *, int, int, bool);
void scanygrid(SCR *, int, bool);
int selecthandle(SCR *);
int flaghandle(SCR *);
int parsekey(SCR *, int);
int printtitle(WINDOW *);
void eventloop(SCR *);
int setup(void);

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
 * The main loop of the program.
 * Takes input from getch and passes it off to the key parser
 * It reads until an escape char is sent.
 */
void
eventloop(SCR * scr)
{
	int c;

  if (initgamewin(scr))
    return;

  /*
   * Reset cursor position
   */
	scr->cury = GRD_Y / 2;
	scr->curx = GRD_X / 2;
	wmove(scr->win, scr->cury, scr->curx);

	while ((c = wgetch(scr->win)) != ESC) {
		if (parsekey(scr, c))
			break;

    updategamewin(scr);
	}

	if (wclear(scr->win) == ERR)
		mslog("Failed to clear window");

	mainmenu(scr);
}
/*
 * Prints the game title to the screen
 * Looks in a hardcoded path
 */
int
printtitle(WINDOW * win)
{
	int y;
	char buf[1024];
	FILE *fp = (FILE *) 0;

	if (!(fp = fopen("./assets/title.txt", "r"))) {
    mslog("Failed to open file ./assets/title.txt");
		return EXIT_FAILURE;
  }

	for (y = 2; fgets(buf, 1024, fp); y++)
		mvwprintw(win, y, (GRD_X / 10), "%s", buf);

	fclose(fp);
	wrefresh(win);
  return EXIT_SUCCESS;
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

	if (printtitle(scr->win)) {
    mvwprintw(scr->win, (WIN_Y / 2) - ((WIN_Y / 8) + 2),
	      (WIN_X / 2) - strlen("MINESWEEPE"), "MINESWEEPER++");
  }

	mvwprintw(scr->win, (WIN_Y / 2) - (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(1) PLAY"), "(1) PLAY");

	mvwprintw(scr->win, (WIN_Y / 2) + (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(2) QUIT"), "(2) QUIT");

	if (wrefresh(scr->win) == ERR)
    mslog("Failed to refresh win");

	if (box(scr->win, 0, 0) == ERR)
    mslog("Failed to draw box around win");

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
		mslog("Failed to draw box around stdscr");

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

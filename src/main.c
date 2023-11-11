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
void deathscreen(SCR *);
int north(SCR *);
int northwest(SCR *);
int northeast(SCR *);
int south(SCR *);
int southwest(SCR *);
int southeast(SCR *);
int west(SCR *);
int east(SCR *);
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
deathscreen(SCR * scr)
{
  if (wclear(scr->datawin) == ERR)
    mslog("Failed to clear datawin");

  mvwprintw(scr->datawin, 1, 1, "YOU DIED!");
  mvwprintw(scr->datawin, 2, 1, "Press any key to continue");
	mvwprintw(scr->win, scr->cury, scr->curx, "*");

  if (box(scr->datawin, 0, 0) == ERR)
    mslog("Failed to draw box around datawin");

  if (wmove(scr->win, scr->cury, scr->curx) == ERR)
    mslog("Failed to move the cursor in win");

  if (wrefresh(scr->win) == ERR)
    mslog("Failed to refresh win");

  wgetch(scr->win);
}

int
north(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury+1].xgrd[scr->curx].bomb)
      count++;
  else mvwprintw(scr->win, scr->cury+1, scr->curx, "~");

  return count;
}

int
northwest(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury+1].xgrd[scr->curx+1].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury+1, scr->curx+1, "~");

  return count;
}

int
northeast(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury+1].xgrd[scr->curx-1].bomb)
      count++;
  else mvwprintw(scr->win, scr->cury+1, scr->curx-1, "~");

  return count;
}

int
south(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury-1].xgrd[scr->curx].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury-1, scr->curx, "~");

  return count;
}

int
southwest(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury-1].xgrd[scr->curx+1].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury-1, scr->curx+1, "~");

  return count;
}

int
southeast(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury-1].xgrd[scr->curx-1].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury-1, scr->curx-1, "~");
 
  return count;
}

int
west(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury].xgrd[scr->curx-1].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury, scr->curx-1, "~");

  return count;
}

int
east(SCR * scr)
{
  int count = 0;

  if (scr->ygrd[scr->cury].xgrd[scr->curx+1].bomb)
    count++;
  else mvwprintw(scr->win, scr->cury, scr->curx+1, "~");

  return count;
}

/*
 * If the user selects a bomb, go back to the main menu
 * Else it should scan the grid to replace chars
 */
int
selecthandle(SCR * scr)
{
  int x;
  int y;
  int count = 0;

	/* If the user selects a bomb */
	if (scr->ygrd[scr->cury].xgrd[scr->curx].bomb) {
    deathscreen(scr);
		return EXIT_FAILURE;
	} else {
    scr->ygrd[scr->cury].xgrd[scr->curx].selected = true;

    count += north(scr);
    count += northwest(scr);
    count += northeast(scr);
    count += south(scr);
    count += southwest(scr);
    count += southeast(scr);
    count += west(scr);
    count += east(scr);

    if (count > 0)
      mvwprintw(scr->win, scr->cury, scr->curx, "%d", count);
    else
      mvwprintw(scr->win, scr->cury, scr->curx, "~");

    wmove(scr->win, scr->cury, scr->curx);
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
    /*
     * Decrease the bomb counter
     */
    scr->bombc--;
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

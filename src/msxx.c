#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "scr.h"
#include "grid.h"
#include "key.h"
#include "config.h"
#include "msxx.h"

static int msprinttitle(WINDOW *);
static void msmainmenu(SCR*);
static void msupdategmwin(SCR *);
static void mseventloop(SCR*);

/*
 * Prints out a file
 */
static int
msprinttitle(WINDOW * w)
{
  int y;
  char buf[1024];
  FILE * fp = (FILE*) 0;

  if (!(fp = fopen("./assets/title.txt", "r"))) {
		mslog(TITLE, __func__);
    return 1;
  }

  for (y = 2; fgets(buf, 1024, fp); y++)
    mvwprintw(w, y, (GRD_X / 10), "%s", buf);

  if (fclose(fp)) {
		mslog(TITLE, __func__);
    return 1;
  }

  if (wrefresh(w) == ERR) {
    mslog(REFRESH, __func__);
    return 1;
  }

  return 0;
}

/*
 * A simple screen that will show upon death
 */
void
msdeathscr(SCR * s)
{
  if (wclear(s->gmwin) == ERR)
    mslog(CLEAR, __func__);

  mvwprintw(s->gmwin, 1, 1, "YOU DIED!");
  mvwprintw(s->gmwin, 2, 1, "Press any key to continue");

  if (has_colors()) {
    wattron(s->win, COLOR_PAIR(RED));
    mvwprintw(s->win, s->cury, s->curx, "*");
    wattroff(s->win, COLOR_PAIR(RED));
  } else {
    mvwprintw(s->win, s->cury, s->curx, "*");
  }

  if (box(s->gmwin, 0, 0) == ERR)
		mslog(BOX, __func__);

  if (wmove(s->win, s->cury, s->curx) == ERR)
		mslog(CURSOR, __func__);

  if (wrefresh(s->gmwin) == ERR)
		mslog(REFRESH, __func__);

  if (wrefresh(s->win) == ERR)
		mslog(REFRESH, __func__);

  wgetch(s->win);
}

/*
 * Updates the game window with information.
 * For example the amount of mines remaining
 */
static void
msupdategmwin(SCR * s)
{
  /* TODO Causing a memory fault */
  mvwprintw(s->gmwin, 1, 1, "%d MINES REMAINING",
    (s->minec - s->flagc));

	if (wrefresh(s->gmwin) == ERR)
		mslog(REFRESH, __func__);

	if (wmove(s->win, s->cury, s->curx) == ERR)
		mslog(MOVE, __func__);
}

static void 
msmainmenu(SCR * s)
{
  int c = 0;

  if (wclear(s->win) == ERR)
		mslog(CLEAR, __func__);

  if (curs_set(0) == ERR)
		mslog(HIDE, __func__);

  if (has_colors()) {
    /* Set background color for main window */
    if (wbkgd(s->win, COLOR_PAIR(WIN_BOX)))
			mslog(BACKGROUND, __func__);
  }

  if (msprinttitle(s->win)) {
    mvwprintw(s->win, (WIN_Y / 2) - ((WIN_Y / 8) + 2),
      (WIN_X / 2) - strlen("MINESWEEPE"), "MINESWEEPER++");
  }

  mvwprintw(s->win, (WIN_Y / 2) - (WIN_Y / 12),
    (WIN_X / 2) - strlen("(1) PLAY"), "(1) PLAY");

  mvwprintw(s->win, (WIN_Y / 2) + (WIN_Y / 12),
    (WIN_X / 2) - strlen("(2) QUIT"), "(2) QUIT");

  if (box(s->win, 0, 0) == ERR)
		mslog(BOX, __func__);

  while ((c = wgetch(s->win)))
    if (c == '1' || c == '2')
      break;

  if (curs_set(1) == ERR)
		mslog(SHOW, __func__);

  if (c == '1')
    mseventloop(s);
}

/*
 * The main loop of the program
 */
static void
mseventloop(SCR * s)
{
  int c = 0;

  if (wclear(s->win) == ERR)
		mslog(CLEAR, __func__);

  if (scrinitgmwin(s))
    return;

  griddraw(s);

  /*
   * Reset cursor position
   */
  s->cury = GRD_Y / 2;
  s->curx = GRD_X / 2;
  
  if (wmove(s->win, s->cury, s->curx) == ERR)
		mslog(MOVE, __func__);

  while ((c = wgetch(s->win)) != ESC) {
    if (keyparse(s, c))
      break;

    msupdategmwin(s);
  }

  scrdestroygmwin(s->gmwin);

  msmainmenu(s);
}

/*
 * Setup function for the entire game
 */
int
mssetup(void)
{
  SCR * s = (SCR*) 0;

  if (!(s = scrinit()))
    return 1;

  /*
   * Apply settings for curses
   */
  if (noecho() == ERR)
		mslog(NOECHO, __func__);

  if (raw() == ERR)
		mslog(RAW, __func__);

  if (keypad(s->win, TRUE) == ERR)
		mslog(KEYPAD, __func__);

  msmainmenu(s);

  return scrdestroy(s);
}

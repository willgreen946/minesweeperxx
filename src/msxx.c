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
static void eventloop(SCR*);

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
    mslog("Failed to open file ./assets/title.txt");
    return 1;
  }

  for (y = 2; fgets(buf, 1024, fp); y++)
    mvwprintw(w, y, (GRD_X / 10), "%s", buf);

  if (fclose(fp)) {
    mslog("Failed to close file ./assets/title.txt");
    return 1;
  }

  if (wrefresh(w) == ERR) {
    mslog("Failed to refresh (win)");
    return 1;
  }

  return 0;
}

void
msdeathscr(SCR * s)
{
  if (wclear(s->gmwin) == ERR)
    mslog("Failed to clear (gmwin)");

  mvwprintw(s->gmwin, 1, 1, "YOU DIED!");
  mvwprintw(s->gmwin, 2, 1, "Press any key to continue");
  mvwprintw(s->win, s->cury, s->curx, "*");

  if (box(s->gmwin, 0, 0) == ERR)
    mslog("Failed to draw box (gmwin)");

  if (wmove(s->win, s->cury, s->curx) == ERR)
    mslog("Failed to move cursor (win)");

  if (wrefresh(s->gmwin) == ERR)
    mslog("Failed to refresh (gmwin)");

  if (wrefresh(s->win) == ERR)
    mslog("Failed to refresh (win)");

  wgetch(s->win);
}

static void 
msmainmenu(SCR * s)
{
  int c = 0;

  if (wclear(s->win) == ERR)
    mslog("Failed to clear (win)");

  if (curs_set(0) == ERR)
    mslog("Failed to hide cursor");

  if (has_colors()) {
    /* Set background color for main window */
    if (wbkgd(s->win, COLOR_PAIR(WIN_BOX)))
      mslog("Failed to set background color (win)");
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
    mslog("Failed to draw box (win)");

  while ((c = wgetch(s->win)))
    if (c == '1' || c == '2')
      break;

  if (curs_set(1) == ERR)
    mslog("Failed to show cursor");

  if (c == '1')
    eventloop(s);
}

/*
 * The main loop of the program
 */
static void
eventloop(SCR * s)
{
  int c = 0;

  if (wclear(s->win) == ERR)
    mslog("Failed to clear (win)");

  if (scrinitgmwin(s))
    return;

  griddraw(s);

  /*
   * Reset cursor position
   */
  s->cury = GRD_Y / 2;
  s->curx = GRD_X / 2;
  
  if (wmove(s->win, s->cury, s->curx) == ERR)
    mslog("Failed to move cursor (win)");

  while ((c = wgetch(s->win)) != ESC) {
    if (keyparse(s, c))
      break;
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
    mslog("Failed to disable echoing");

  if (raw() == ERR)
    mslog("Failed to enter rawmode");

  if (keypad(s->win, TRUE) == ERR)
    mslog("Failed to init the keypad");

  msmainmenu(s);

  return scrdestroy(s);
}

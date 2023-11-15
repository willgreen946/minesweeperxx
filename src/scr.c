#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include "mslog.h"
#include "config.h"
#include "scr.h"

static void scrinitcolor(void);

/*
 * Initialises the game window.
 */
int
scrinitgmwin(SCR * s)
{
  s->gmwiny = (s->maxy / 2) - ((WIN_Y / 2) - 1);
  s->gmwinx = (s->maxx / 2) - ((WIN_X / 2) - 36);

  if (wclear(s->win) == ERR)
    mslog("Failed to clear (win)");

  if (!(s->gmwin
    = subwin(s->win, WIN_Y - 2, WIN_X / 2, 
    s->gmwiny, s->gmwinx))) {
    mslog("[FATAL] Failed to create game window (gmwin)");
    return 1;
  }

  if (has_colors()) {
    /* Set background color for main window */
    if (wbkgd(s->win, COLOR_PAIR(WIN_BOX)))
      mslog("Failed to set background color (win)");

    /* Set background color for game data window */
    if (wbkgd(s->gmwin, COLOR_PAIR(BAK_BOX)))
      mslog("Failed to set background color (gmwin)");
  }

  if (box(stdscr, 0, 0) == ERR)
    mslog("Failed to draw box (stdscr)");

  if (box(s->gmwin, 0, 0) == ERR)
    mslog("Failed to draw box (gmwin)");

  if (wrefresh(stdscr) == ERR)
    mslog("Failed to refresh (stdscr)");

  if (wrefresh(s->win) == ERR)
    mslog("Failed to refresh (win)");

  if (wrefresh(s->gmwin) == ERR)
    mslog("Failed to refresh (gmwin)");

  s->minec = MINES;
  s->flagc = 0;

  return 0;
}

void
scrdestroygmwin(WINDOW * w)
{
  if (delwin(w) == ERR)
    mslog("Failed to destroy (gmwin)");
}

/*
 * Attempts to init color in curses
 */
static void 
scrinitcolor(void)
{
  if (has_colors()) {
    if (start_color() == ERR)
      mslog("Failed to init color (start_color)");

    if (init_pair(WIN_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (WIN_BOX)");

    if (init_pair(BAK_BOX, COLOR_WHITE, COLOR_RED) == ERR)
      mslog("Failed to init color pair (BAK_BOX)");

   if (init_pair(BLU, COLOR_BLACK, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (BLU)");

   if (init_pair(PUR, COLOR_MAGENTA, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (PUR)");

   if (init_pair(YEL, COLOR_YELLOW, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (YEL)");

   if (init_pair(RED, COLOR_RED, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (RED)");

   if (init_pair(GRE, COLOR_GREEN, COLOR_BLUE) == ERR)
      mslog("Failed to init color pair (GRE)");

  } else {
    mslog("Terminal does not support color");
  }
}

/*
 * Initialises curses.
 * Allocates memory for SCR *.
 * Returns new SCR *.
 */
SCR *
scrinit(void)
{
  SCR * s = (SCR*) 0;

  if (!(s = (SCR*) malloc(sizeof(*s)))) {
    mslog("[FATAL] Failed to allocate memory for SCR *");
    return (SCR*) 0;
  }

  if (!(stdscr = initscr())) {
    mslog("[FATAL] Failed to init curses");
    return (SCR*) 0;
  }

  getmaxyx(stdscr, s->maxy, s->maxx);

  if (s->maxy < WIN_Y || s->maxx < WIN_X) {
    mslog("[FATAL] Window needs to be at least 80x24");
    return (SCR*) 0;
  }

  /* Should print new window in the center */
  if (!(s->win
    = newwin(WIN_Y, WIN_X, (s->maxy / 2) - (WIN_Y / 2),
    (s->maxx / 2) - (WIN_X / 2)))) {
    mslog("[FATAL] Failed to create window (win)");
    return (SCR*) 0;
  }

  scrinitcolor();

  return s;
}

/*
 * Ends curses,
 * Frees memory in SCR *
 */
int
scrdestroy(SCR * s)
{
  if (s)
    free(s);
  else
    mslog("SCR * is not NULL");

  if (endwin() == ERR) {
    mslog("Failed to end curses");
    return 1;
  }

  return 0;
}

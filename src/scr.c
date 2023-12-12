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
	/* Get the Center of the screen */
  s->gmwiny = (s->maxy / 2) - ((WIN_Y / 2) - 1);
  s->gmwinx = (s->maxx / 2) - ((WIN_X / 2) - 36);

  if (wclear(s->win) == ERR)
    mslog(CLEAR, __func__);

  if (!(s->gmwin
    = subwin(s->win, WIN_Y - 2, WIN_X / 2, 
    s->gmwiny, s->gmwinx))) {
    mslog(NEWWIN, __func__);
    return 1;
  }

  if (has_colors()) {
    /* Set background color for main window */
    if (wbkgd(s->win, COLOR_PAIR(WIN_BOX)))
      mslog(BACKGROUND, __func__);

    /* Set background color for game data window */
    if (wbkgd(s->gmwin, COLOR_PAIR(BAK_BOX)))
      mslog(BACKGROUND, __func__);
  }

  if (box(stdscr, 0, 0) == ERR)
    mslog(BOX, __func__);

  if (box(s->gmwin, 0, 0) == ERR)
    mslog(BOX, __func__);

  if (wrefresh(stdscr) == ERR)
    mslog(REFRESH, __func__);

  if (wrefresh(s->win) == ERR)
    mslog(REFRESH, __func__);

  if (wrefresh(s->gmwin) == ERR)
    mslog(REFRESH, __func__);

  s->minec = MINES;
  s->flagc = 0;

  return 0;
}

void
scrdestroygmwin(WINDOW * w)
{
  if (delwin(w) == ERR)
    mslog(DESTWIN, __func__);
}

/*
 * Attempts to init color in curses
 */
static void 
scrinitcolor(void)
{
  if (has_colors()) {
    if (start_color() == ERR)
      mslog(INITPAIR, __func__);

    if (init_pair(WIN_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

    if (init_pair(BAK_BOX, COLOR_WHITE, COLOR_RED) == ERR)
      mslog(INITPAIR, __func__);

   if (init_pair(BLU, COLOR_BLACK, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

   if (init_pair(PUR, COLOR_MAGENTA, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

   if (init_pair(YEL, COLOR_YELLOW, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

   if (init_pair(RED, COLOR_RED, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

   if (init_pair(GRE, COLOR_GREEN, COLOR_BLUE) == ERR)
      mslog(INITPAIR, __func__);

  } else {
    mslog(COLOR, __func__);
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
    fprintf(stderr,
			"[FATAL] Failed to allocate memory for SCR *");
    return (SCR*) 0;
  }

  if (!(stdscr = initscr())) {
    fprintf(stderr,
			"[FATAL] Failed to init curses");
		mslog(STARTCURSES, __func__);
    return (SCR*) 0;
  }

  getmaxyx(stdscr, s->maxy, s->maxx);

  if (s->maxy < WIN_Y || s->maxx < WIN_X) {
    fprintf(stderr,
			"[FATAL] Window needs to be at least %dx%d",
			WIN_X, WIN_Y);
    return (SCR*) 0;
  }

  /* Should print new window in the center */
  if (!(s->win
    = newwin(WIN_Y, WIN_X, (s->maxy / 2) - (WIN_Y / 2),
    (s->maxx / 2) - (WIN_X / 2)))) {
    mslog(NEWWIN, __func__);
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

  if (endwin() == ERR) {
    mslog(ENDCURSES, __func__);
    return 1;
  }

  return 0;
}

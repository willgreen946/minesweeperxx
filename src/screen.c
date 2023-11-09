#include <curses.h>
#include "config.h"
#include "mslog.h"
#include "screen.h"

/*
 * Initialises SCR struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (scrdestroy)
 */
SCR *
scrinit(void)
{
	SCR *tmp = (SCR *) NULL;

	if (!(tmp = (SCR *) malloc(sizeof(*tmp))))
		return (SCR *) NULL;

	if (!(stdscr = initscr())) {
		mslog("FATAL:Failed to init curses");
		return (SCR *) NULL;
	}

	getmaxyx(stdscr, tmp->maxy, tmp->maxx);

	if (tmp->maxy < WIN_Y || tmp->maxx < WIN_X) {
		mslog("FATAL:SCR too small to function, need at least 80x24");
		return (SCR *) NULL;
	}

	/* Should print the new window in the center of the screen */
	if (!(tmp->win
	    = newwin(WIN_Y, WIN_X, (tmp->maxy / 2) - (WIN_Y / 2),
	    (tmp->maxx / 2) - (WIN_X / 2)))) {

		mslog("FATAL:Failed to create window");
		return (SCR *) NULL;
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


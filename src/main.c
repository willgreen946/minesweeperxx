#include <curses.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/*
 * Color pair numbers
 */
enum {
	WIN_BOX = 1,
	MSG_BOX = 2,
	ERR_BOX = 3,
	BAK_BOX = 4,
};
/*
 * Some dimensions for the screen
 */
enum {
	WIN_Y = 24,
	WIN_X = 80,
	GRID_Y = 16,
	GRID_X = 30,
	BOMB_COUNT = 99
};
/*
 * Some key ascii codes
 */
enum {
	ESC = 27,
	RET = 10,
	SPC = 32
};
/*
 * Used to keep a map of all the chars on the grid.
 * holding data about the part of the grid,
 * Like weather it's a bomb and been selected previously
 */
struct XGRID {
	bool bomb;
	bool selected;
	bool flagged;
};
/*
 * Represents the Y axis of the game grid
 * Should be declared as an array in TTY
 * Holds an array of the X axis
 */
struct YGRID {
	struct XGRID xgrd[GRID_X];
};

typedef struct {
	int maxx;
	int maxy;
	int cury;
	int curx;
	struct YGRID ygrd[GRID_Y];
	WINDOW *win;
} TTY;

int mainmenu(TTY *);

bool
resizecheck(TTY * tty)
{
	int y, x;

	getmaxyx(tty->win, y, x);

	if (y < WIN_Y || x < WIN_X)
		err(EXIT_FAILURE, "TTY too small to function");

	if (y != tty->maxy || x != tty->maxy)
		return true;

	return false;
}

/*
 * Prints error messages to the mserrlog.txt file
 */
void
mslog(const char *str)
{
	FILE *fp;
  time_t curtime;
  char strtime[9];
  struct tm * timeinfo;

	fp = fopen("mserrlog.txt", "a");

	if (!fp) {
		fprintf(stderr, "Failed to open log file:%s\n", strerror(errno));
		return;
	}

  time(&curtime);
  timeinfo = localtime(&curtime);
  strftime(strtime, sizeof(strtime), "%H:%M:%S", timeinfo);

  fprintf(fp, "[%s]:%s\n", strtime, str);

	if (fclose(fp)) {
		fprintf(stderr, "%s\n", strerror(errno));
		return;
	}
}

void
moveup(TTY * tty)
{
	if ((tty->cury - 1) >= 0)
		tty->cury--;

	wmove(tty->win, tty->cury, tty->curx);
	wrefresh(tty->win);
}

void
movedown(TTY * tty)
{
	if (tty->cury < (GRID_Y - 1))
		tty->cury++;

	wmove(tty->win, tty->cury, tty->curx);
	wrefresh(tty->win);
}

void
moveleft(TTY * tty)
{
	if ((tty->curx - 1) >= 0)
		tty->curx--;

	wmove(tty->win, tty->cury, tty->curx);
	wrefresh(tty->win);
}

void
moveright(TTY * tty)
{
	if (tty->curx < (GRID_X - 1))
		tty->curx++;

	wmove(tty->win, tty->cury, tty->curx);
	wrefresh(tty->win);
}
/*
 * 8 if statments to see if the 8 surrounding blocks
 * are bombs, if they are bombs the count is incremented by 1
 * the count is printed to the current cursor position
 * The count is returned by the function
 */
int
bombsurroundcount(TTY * tty, int y, int x)
{
	int count = 0;

	if (tty->ygrd[y].xgrd[x + 1].bomb)
		count++;

	if (tty->ygrd[y].xgrd[x - 1].bomb)
		count++;

	if (tty->ygrd[y + 1].xgrd[x].bomb)
		count++;

	if (tty->ygrd[y + 1].xgrd[x + 1].bomb)
		count++;

	if (tty->ygrd[y + 1].xgrd[x - 1].bomb)
		count++;

	if (tty->ygrd[y - 1].xgrd[x].bomb)
		count++;

	if (tty->ygrd[y - 1].xgrd[x + 1].bomb)
		count++;

	if (tty->ygrd[y - 1].xgrd[x - 1].bomb)
		count++;

	return count;
}

void
scanxgrid(TTY * tty, int y, int min, bool inc)
{
	int x;
	int count = 0;

	/* Go along the X axis <- this way */
	for (x = tty->curx; x != min;) {
		if (tty->ygrd[y].xgrd[x].selected)
			break;	/* Do nothing if its already been selected */

		else if (tty->ygrd[y].xgrd[x].bomb)
			break;	/* Do nothing if its a bomb */

		else if ((count = bombsurroundcount(tty, y, x)) > 0) {
			tty->ygrd[y].xgrd[x].selected = true;
			mvwprintw(tty->win, y, x, "%d", count);
			return;
		} else {
			tty->ygrd[y].xgrd[x].selected = true;
			mvwprintw(tty->win, y, x, "~");

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
scanygrid(TTY * tty, int min, bool inc)
{
	int y;

	for (y = tty->cury; y != min;) {
		/* Go along the X axis <- this way */
		scanxgrid(tty, y, 0, false);

		/* Go along the X axis -> this way */
		scanxgrid(tty, y, GRID_X, true);

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
selecthandle(TTY * tty)
{
	/* If the user selects a bomb */
	if (tty->ygrd[tty->cury].xgrd[tty->curx].bomb) {
		mvwprintw(tty->win, WIN_Y / 2, WIN_X / 2, "BOOM!!!!!!");
		mvwprintw(tty->win, tty->cury, tty->curx, "*");
		wrefresh(tty->win);
		wgetch(tty->win);
		return EXIT_FAILURE;
	} else {
		/* Go down the grid */
		scanygrid(tty, GRID_Y, true);

		/* Go up the grid */
		scanygrid(tty, 0, false);

		wrefresh(tty->win);
	}

	return EXIT_SUCCESS;
}
/*
 * Handles the processing of marking a block a bomb
 */
int
flaghandle(TTY * tty)
{
	if (!tty->ygrd[tty->cury].xgrd[tty->curx].flagged) {
		mvwprintw(tty->win, tty->cury, tty->curx, "F");
		wrefresh(tty->win);
		tty->ygrd[tty->cury].xgrd[tty->curx].flagged = true;
	}
	 /* If the sqaure is already flagged do nothing */ 
	else
		return EXIT_SUCCESS;

	return EXIT_SUCCESS;
}

int
parsekey(TTY * tty, int c)
{
	switch (c) {
	case KEY_UP:
	case 'k':
		moveup(tty);
		return EXIT_SUCCESS;
	case KEY_DOWN:
	case 'j':
		movedown(tty);
		return EXIT_SUCCESS;
	case KEY_LEFT:
	case 'h':
		moveleft(tty);
		return EXIT_SUCCESS;
	case KEY_RIGHT:
	case 'l':
		moveright(tty);
		return EXIT_SUCCESS;
	case RET:
	case 'e':
		return selecthandle(tty);
	case SPC:
	case 'f':
		return flaghandle(tty);
	}

	return EXIT_SUCCESS;
}
/*
 * Draws the onscreen grid according to GRID_Y & GRID_X
 * Generates X (BOMB_COUNT) amount of bombs,
 * and places them in the grid
 */
int
drawgrid(TTY * tty)
{
	int y, x;
	int yrand, xrand;

	srand(time(0));

	if (has_colors()) {
		if (wbkgd(tty->win, COLOR_PAIR(WIN_BOX))) {
			mslog("Failed to set background color");
			return EXIT_FAILURE;
		}
	}

	/* Draws the grid and randomly genrates bombs */
	for (y = 0; y < GRID_Y; y++) {
		for (x = 0; x < GRID_X; x++) {
			mvwprintw(tty->win, y, x, "#");
			tty->ygrd[y].xgrd[x].bomb = false;
			tty->ygrd[y].xgrd[x].selected = false;
			tty->ygrd[y].xgrd[x].flagged = false;
		}
	}

	/* Set the bombs, using y as a counter */
	for (y = 0; y < BOMB_COUNT; y++) {
		yrand = (int)rand() % GRID_Y;
		xrand = (int)rand() % GRID_X;
		tty->ygrd[yrand].xgrd[xrand].bomb = true;
		mvwprintw(tty->win, yrand, xrand, "B");
	}

	/* Reset cursor position */
	tty->cury = GRID_Y / 2;
	tty->curx = GRID_X / 2;
	wmove(tty->win, tty->cury, tty->curx);
	wrefresh(tty->win);
	return EXIT_SUCCESS;
}
/*
 * The main loop of the program.
 * Takes input from getch and passes it off to the key parser
 * It reads until an escape char is sent.
 */
void
eventloop(TTY * tty)
{
	int c;

	if (drawgrid(tty))
		return;

	while ((c = wgetch(tty->win)) != ESC) {
		if (parsekey(tty, c))
			break;
	}

	if (wclear(tty->win) == ERR) {
		mslog("Failed to clear window");
		return;
	}
	mainmenu(tty);
}
/*
 * Initialises TTY struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (ttydestroy)
 */
TTY *
ttyinit(void)
{
	TTY *tmp = (TTY *) NULL;

	if (!(tmp = (TTY *) malloc(sizeof(*tmp))))
		return (TTY *) NULL;

	if (!(stdscr = initscr())) {
		mslog("FATAL:Failed to init curses");
		return (TTY *) NULL;
	}
	getmaxyx(stdscr, tmp->maxy, tmp->maxx);

	if (tmp->maxy < WIN_Y || tmp->maxx < WIN_X) {
		mslog("FATAL:TTY too small to function, need at least 80x24");
		return (TTY *) NULL;
	}
	/* Should print the new window in the center of the screen */
	if (!(tmp->win
	    = newwin(WIN_Y, WIN_X, (tmp->maxy / 2) - (WIN_Y / 2),
	    (tmp->maxx / 2) - (WIN_X / 2)))) {

		mslog("FATAL:Failed to create window");
		return (TTY *) NULL;
	}
	return tmp;
}
/*
 * Ends curses and frees memory with TTY struct
 */
int
ttydestroy(TTY * tty)
{
	if (endwin() == ERR) {
		mslog("Failed to end curses session");
		return EXIT_FAILURE;
	}
	if (!tty) {
		mslog("TTY * is not NULL");
		return EXIT_FAILURE;
	}
	free(tty);
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
		mslog("TTY does not support color");
	}

	return EXIT_SUCCESS;
}
/*
 * Prints the game title to the screen
 * Looks in a hardcoded path
 */
void
printtitle(TTY * tty)
{
	int y;
	char buf[1024];
	FILE *fp = (FILE *) NULL;

	if (!(fp = fopen("./assets/title.txt", "r")))
		return;

	for (y = 2; fgets(buf, 1024, fp); y++) {
		mvwprintw(tty->win, y, (GRID_X / 10), "%s", buf);
	}

	wrefresh(tty->win);
	fclose(fp);
}

int
mainmenu(TTY * tty)
{
	int c;

	if (has_colors()) {
		if (wbkgd(tty->win, COLOR_PAIR(WIN_BOX)))
			mslog("Failed to set background color");
	}
	if (curs_set(0) == ERR)
		mslog("Failed to hide cursor");

	printtitle(tty);

	mvwprintw(tty->win, (WIN_Y / 2) - (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(1) PLAY"), "(1) PLAY");

	mvwprintw(tty->win, (WIN_Y / 2) + (WIN_Y / 12),
	    (WIN_X / 2) - strlen("(2) QUIT"), "(2) QUIT");

	wrefresh(tty->win);

	box(tty->win, 0, 0);

	while ((c = wgetch(tty->win)))
		if (c == '1' || c == '2')
			break;

	if (curs_set(1) == ERR)
		mslog("Failed to show cursor");

	if (wclear(tty->win) == ERR)
		mslog("Failed to clear window");

	if (c == '1')
		eventloop(tty);

	return EXIT_SUCCESS;
}

int
setup(void)
{
	TTY *tty = (TTY *) NULL;

	if (!(tty = ttyinit())) {
		mslog("FATAL:Failed to init TTY struct");
    return EXIT_FAILURE;
  }

	if (raw() == ERR)
		mslog("Failed to enter raw mode");

	if (noecho() == ERR)
		mslog("Failed to disable echoing");

	if (initcolor())
		mslog("Failed to init color");

	if (keypad(tty->win, TRUE) == ERR)
		mslog("Failed to init keypad");

	if (has_colors()) {
		if (wbkgd(stdscr, COLOR_PAIR(BAK_BOX)))
			mslog("Failed to set background color");
	}

	if (box(stdscr, 0, 0) == ERR)
		mslog("Failed to draw box");

	if (wrefresh(stdscr))
		mslog("Failed to refresh window");

	if (mainmenu(tty))
		return EXIT_FAILURE;

	return ttydestroy(tty);
}

int
main(void)
{
	return setup();
}

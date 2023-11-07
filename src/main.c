#include <curses.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Color pair numbers
 */
enum
{
  WIN_BOX = 1,
  MSG_BOX = 2,
  ERR_BOX = 3,
  BAK_BOX = 4
};

/*
 * Some dimensions for the screen
 */
enum
{
  MIN_TTY_Y = 24,
  MIN_TTY_X = 80,
  GM_WIN_Y = 24,
  GM_WIN_X = 80,
  GRID_SIZE = 24,
  BOMB_COUNT = 99
};

/*
 * Some key ascii codes
 */
enum
{
  ESC = 27,
  RET = 10
};

/*
 * Used to keep a map of all the chars on the grid.
 * holding data about the part of the grid,
 * Like weather it's a bomb and been selected previously
 */
struct XGRID
{
  bool bomb;
  bool selected;
};

/*
 * Represents the Y axis of the game grid
 * Should be declared as an array in TTY 
 * Holds an array of the X axis
 */
struct YGRID
{
  struct XGRID xgrd[GRID_SIZE];
};

typedef struct
{
  int maxx;
  int maxy;
  int cury;
  int curx;
  struct YGRID ygrd[GRID_SIZE];
  WINDOW *win;
} TTY;

int mainmenu (TTY *);

bool
resizecheck (TTY *tty)
{
  int y, x;

  getmaxyx (tty->win, y, x);

  if (y < MIN_TTY_Y || x < MIN_TTY_X)
    err (EXIT_FAILURE, "TTY too small to function");

  if (y != tty->maxy || x != tty->maxy)
    return true;

  return false;
}

void
moveup (TTY *tty)
{
  if ((tty->cury - 1) >= 0)
    tty->cury--;

  wmove (tty->win, tty->cury, tty->curx);
  wrefresh (tty->win);
}

void
movedown (TTY *tty)
{
  if (tty->cury < (GRID_SIZE - 1))
    tty->cury++;

  wmove (tty->win, tty->cury, tty->curx);
  wrefresh (tty->win);
}

void
moveleft (TTY *tty)
{
  if ((tty->curx - 1) >= 0)
    tty->curx--;

  wmove (tty->win, tty->cury, tty->curx);
  wrefresh (tty->win);
}

void
moveright (TTY *tty)
{
  if (tty->curx < (GRID_SIZE - 1))
    tty->curx++;

  wmove (tty->win, tty->cury, tty->curx);
  wrefresh (tty->win);
}

/*
 * 8 if statments to see if the 8 surrounding blocks
 * are bombs, if they are bombs the count is incremented by 1
 * the count is printed to the current cursor position
 * The count is returned by the function
 */
int
bombsurroundcount (TTY *tty, int y, int x)
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

/*
 * Goes up and down the grid scanning each char right to left
 * If the char has already been selected do nothing,
 * If the scan encounters a bomb it will not change the char
 * If the char has no bombs around it, it will print a #
 * If the char has bombs around it, it will print the count
 */
void
scangrid (TTY *tty, int min, bool inc)
{
  int y, x;
  int count = 0;

  for (y = tty->cury; y != min;)
    {
      /*
       * Go along the X axis <- this way
       */
      for (x = tty->curx; x > 0;)
        {
          if (tty->ygrd[y].xgrd[x].selected)
            break; /* Do nothing if its already been selected */

          else if (tty->ygrd[y].xgrd[x].bomb)
            break; /* Do nothing if its a bomb */

          else if ((count = bombsurroundcount (tty, y, x)) > 0)
            {
              tty->ygrd[y].xgrd[x].selected = true;
              mvwprintw (tty->win, y, x, "%d", count);
              break;
            }

          else
            {
              tty->ygrd[y].xgrd[x].selected = true;
              mvwprintw (tty->win, y, x, "#");

              if (x > 0)
                x--;
            }
        }

      /*
       * Go along the X axis -> this way
       */
      for (x = tty->curx; x < (GRID_SIZE - 1);)
        {
          if (tty->ygrd[y].xgrd[x].selected)
            break; /* Do nothing if its already been selected */

          else if (tty->ygrd[y].xgrd[x].bomb)
            break; /* Do nothing if its a bomb */

          else if ((count = bombsurroundcount (tty, y, x)) > 0)
            {
              tty->ygrd[y].xgrd[x].selected = true;
              mvwprintw (tty->win, y, x, "%d", count);
              break;
            }

          else
            {
              tty->ygrd[y].xgrd[x].selected = true;
              mvwprintw (tty->win, y, x, "#");
              if (x < (GRID_SIZE - 1))
                x++;
            }
        }

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
selecthandle (TTY *tty)
{
  /* If the user selects a bomb */
  if (tty->ygrd[tty->cury].xgrd[tty->curx].bomb)
    {
      mvwprintw (tty->win, GM_WIN_Y / 2, GM_WIN_X / 2, "BOOM!!!!!!");
      mvwprintw (tty->win, tty->cury, tty->curx, "*");
      wrefresh (tty->win);
      wgetch (tty->win);
      return EXIT_FAILURE;
    }

  else
    {
      /*
       * Go down the grid
       */
      scangrid (tty, GRID_SIZE, true);

      /*
       * Go up the grid
       */
      scangrid (tty, 0, false);

      wrefresh (tty->win);
    }

  return EXIT_SUCCESS;
}

int
parsekey (TTY *tty, int c)
{
  switch (c)
    {
    case KEY_UP:
    case 'k':
      moveup (tty);
      return EXIT_SUCCESS;
    case KEY_DOWN:
    case 'j':
      movedown (tty);
      return EXIT_SUCCESS;
    case KEY_LEFT:
    case 'h':
      moveleft (tty);
      return EXIT_SUCCESS;
    case KEY_RIGHT:
    case 'l':
      moveright (tty);
      return EXIT_SUCCESS;
    case RET:
    case 'e':
      return selecthandle (tty);
    }

  return EXIT_SUCCESS;
}

int
drawgrid (TTY *tty)
{
  int y, x;
  int bombc = 0;

  srand (time (0));

  if (wbkgd (tty->win, COLOR_PAIR (WIN_BOX)))
    {
      fprintf (stderr, "(drawgrid) Couldnt set background color\n");
      return EXIT_FAILURE;
    }

  /*
   * Draws the grid and randomly genrates bombs
   */
  for (y = 0; y < GRID_SIZE; y++)
    {
      for (x = 0; x < GRID_SIZE; x++)
        {
          mvwprintw (tty->win, y, x, "~");
          if (((float)rand () / RAND_MAX) < 0.5 && bombc < BOMB_COUNT)
            {
              tty->ygrd[y].xgrd[x].bomb = true;
              bombc++;
            }

          else
            tty->ygrd[y].xgrd[x].bomb = false;

          tty->ygrd[y].xgrd[x].selected = false;
        }
    }

  tty->cury = tty->curx = GRID_SIZE / 2;
  wmove (tty->win, tty->cury, tty->curx);
  wrefresh (tty->win);
  return EXIT_SUCCESS;
}

void
eventloop (TTY *tty)
{
  int c;

  if (drawgrid (tty))
    return;

  while ((c = wgetch (tty->win)) != ESC)
    {
      if (parsekey (tty, c))
        break;
    }

  if (wclear (tty->win) == ERR)
    {
      fprintf (stderr, "(mainmenu) Couldnt clear window\n");
      return;
    }

  mainmenu (tty);
}

/*
 * Initialises TTY struct
 * Returns a pointer to the initialised struct
 * Memory needs to be freed elsewhere (ttydestroy)
 */
TTY *
ttyinit (void)
{
  TTY *tmp = (TTY *)NULL;

  if (!(tmp = (TTY *)malloc (sizeof (*tmp))))
    return (TTY *)NULL;

  if (!(stdscr = initscr ()))
    {
      fprintf (stderr, "(ttyinit) Couldnt init screen\n");
      return (TTY *)NULL;
    }

  getmaxyx (stdscr, tmp->maxy, tmp->maxx);

  if (tmp->maxy < MIN_TTY_Y || tmp->maxx < MIN_TTY_X)
    {
      fprintf (stderr, "(ttyinit) TTY too small to function\n");
      return (TTY *)NULL;
    }

  /*
   * Should print the new window
   * in the center of the screen
   */
  if (!(tmp->win
        = newwin (GM_WIN_Y, GM_WIN_X, (tmp->maxy / 2) - (GM_WIN_Y / 2),
                  (tmp->maxx / 2) - (GM_WIN_X / 2))))
    {

      fprintf (stderr, "(ttyinit) Failed to create window\n");
      return (TTY *)NULL;
    }

  return tmp;
}

/*
 * Ends curses and frees memory with TTY struct
 */
int
ttydestroy (TTY *tty)
{
  if (endwin () == ERR)
    {
      fprintf (stderr, "(ttydestroy) Couldnt end curses\n");
      return EXIT_FAILURE;
    }

  if (!tty)
    {
      fprintf (stderr, "(ttydestory) TTY * is not NULL\n");
      return EXIT_FAILURE;
    }

  free (tty);
  return EXIT_SUCCESS;
}

/*
 * Initialises all of the color pairs for the program
 * It also initialises color as a whole for curses
 */
int
initcolor (void)
{
  if (!has_colors ())
    {
      fprintf (stderr, "TTY does not support color\n");
      return EXIT_FAILURE;
    }

  if (start_color () == ERR)
    return EXIT_FAILURE;

  if (init_pair (WIN_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
    return EXIT_FAILURE;

  if (init_pair (MSG_BOX, COLOR_WHITE, COLOR_BLUE) == ERR)
    return EXIT_FAILURE;

  if (init_pair (ERR_BOX, COLOR_WHITE, COLOR_RED) == ERR)
    return EXIT_FAILURE;

  if (init_pair (BAK_BOX, COLOR_WHITE, COLOR_RED) == ERR)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int
mainmenu (TTY *tty)
{
  int c;

  if (wbkgd (tty->win, COLOR_PAIR (WIN_BOX)))
    {
      fprintf (stderr, "(mainmenu) Couldnt set background color\n");
      return EXIT_FAILURE;
    }

  if (curs_set (0) == ERR)
    {
      fprintf (stderr, "(mainmenu) Couldnt hide the cursor\n");
      return EXIT_FAILURE;
    }

  box (tty->win, 0, 0);

  mvwprintw (tty->win, (GM_WIN_Y / 2) - (GM_WIN_Y / 3),
             (GM_WIN_X / 2) - strlen ("MineSweeper"), "MineSweeper++");

  mvwprintw (tty->win, (GM_WIN_Y / 2) - (GM_WIN_Y / 12),
             (GM_WIN_X / 2) - strlen ("(1) PLAY"), "(1) PLAY");

  mvwprintw (tty->win, (GM_WIN_Y / 2) + (GM_WIN_Y / 12),
             (GM_WIN_X / 2) - strlen ("(2) QUIT"), "(2) QUIT");

  wrefresh (tty->win);

  while ((c = wgetch (tty->win)))
    if (c == '1' || c == '2')
      break;

  if (curs_set (1) == ERR)
    {
      fprintf (stderr, "(mainmenu) Couldnt unhide the cursor\n");
      return EXIT_FAILURE;
    }

  if (wclear (tty->win) == ERR)
    {
      fprintf (stderr, "(mainmenu) Couldnt clear window\n");
      return EXIT_FAILURE;
    }

  if (c == '1')
    eventloop (tty);

  return EXIT_SUCCESS;
}

int
setup (void)
{
  TTY *tty = (TTY *)NULL;

  if (!(tty = ttyinit ()))
    err (EXIT_FAILURE, "Couldnt init TTY");

  if (raw () == ERR)
    err (EXIT_FAILURE, "Couldnt enter raw mode");

  if (noecho () == ERR)
    err (EXIT_FAILURE, "Couldnt disable echoing");

  if (initcolor ())
    err (EXIT_FAILURE, "Couldnt init color");

  if (keypad (tty->win, TRUE) == ERR)
    err (EXIT_FAILURE, "Couldnt init keypad");

  if (wbkgd (stdscr, COLOR_PAIR (BAK_BOX)))
    {
      fprintf (stderr, "(setup) Couldnt set background color\n");
      return EXIT_FAILURE;
    }

  box (stdscr, 0, 0);

  wrefresh (stdscr);

  if (mainmenu (tty))
    return EXIT_FAILURE;

  return ttydestroy (tty);
}

int
main (void)
{
  return setup ();
}

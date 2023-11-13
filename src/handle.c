#include <curses.h>
#include "scr.h"
#include "mslog.h"
#include "msxx.h"
#include "config.h"
#include "handle.h"

/*
 * Handles when the user hits enter on a char
 */
int
handlesel(SCR * s)
{
  if (s->ygrd[s->cury].xgrd[s->curx].mine) {
    msdeathscr(s);
    return 1;
  }

  return 0;
}

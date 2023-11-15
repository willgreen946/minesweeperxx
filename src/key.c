#include <curses.h>
#include "scr.h"
#include "move.h"
#include "handle.h"
#include "config.h"
#include "key.h"

int
keyparse(SCR * s, int c)
{
  switch (c) {
    case KEY_UP:
    case 'k':
      moveup(s->win, &s->cury, s->curx);
      break;
    case KEY_DOWN:
    case 'j':
      movedown(s->win, &s->cury, s->curx);
      break;
    case KEY_LEFT:
    case 'h':
      moveleft(s->win, s->cury, &s->curx);
      break;
    case KEY_RIGHT:
    case 'l':
      moveright(s->win, s->cury, &s->curx);
      break;
    case RET:
    case 'e':
      return handlesel(s);
    case SPC:
    case 'f':
      return flaghandle(s);
  }

  return 0;
}

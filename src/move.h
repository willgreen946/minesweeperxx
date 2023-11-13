#ifndef __MOVE_H__
#define __MOVE_H__

#include <curses.h>
#include "mslog.h"
#include "config.h"

void moveup(WINDOW*, int*, int);

void movedown(WINDOW*, int*, int);

void moveleft(WINDOW*, int, int*);

void moveright(WINDOW*, int, int*);

#endif /* __MOVE_H__ */

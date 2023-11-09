#ifndef __MOVE_H__
#define __MOVE_H__

#include <curses.h>

/*
 * Moves the cursor up one char
 */
void moveup (WINDOW *, int *, int);

/*
 * Moves the cursor down one char
 */
void movedown (WINDOW *, int *, int);

/*
 * Moves the cursor left one char
 */
void moveleft (WINDOW *, int , int *);

/*
 * Moves the cursor right one char
 */
void moveright (WINDOW *, int , int *);

#endif /* __MOVE_H__ */

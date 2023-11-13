#ifndef __MSXX_H__
#define __MSXX_H__

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "scr.h"
#include "grid.h"
#include "key.h"
#include "config.h"

/*
 * Shows a death screen
 */
void msdeathscr(SCR*);

/*
 * Setup function for the entire game
 */
int mssetup(void);

#endif /* __MSXX_H__ */

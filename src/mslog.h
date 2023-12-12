#ifndef __MSLOG_H__
#define __MSLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MSLOG_PATH "mslog.txt"

/*
 * MSLog error numbers
 */
enum MSERRNO {
	REFRESH,
	NEWWIN,
	MOVE,
	COLOR,
	INITPAIR,
	BOX,
	BACKGROUND,
	CLEAR,
	CURSOR,
	TITLE,
	HIDE,
	SHOW,
	ECHO,
	NOECHO,
	KEYPAD,
	RAW,
	ENDCURSES,
	STARTCURSES,
	DESTWIN
};

extern const char * mserrstrs[];

/*
 * Like strerror but with mslog errnos
 */
const char * msstrerror(int);

/*
 * Alters char * to hold the current time
 */
void msgettime(char *);

/*
 * Prints formatted error message to text file
 */
void mslog(int, const char *);

#endif /* __MSLOG_H__ */

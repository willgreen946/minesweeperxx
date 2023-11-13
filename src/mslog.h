#ifndef __MSLOG_H__
#define __MSLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MSLOG_PATH "mslog.txt"

/*
 * Prints formatted error message to text file
 */
void mslog(const char *);

#endif /* __MSLOG_H__ */

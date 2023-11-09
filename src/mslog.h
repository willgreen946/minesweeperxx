#ifndef __MSLOG_H__
#define __MSLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define MSLOG_PATH "mserrlog.txt"

/*
 * Prints formatted error messages to a text file
 */
void mslog(const char *str);

#endif /* __MSLOG_H__ */

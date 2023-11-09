#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "mslog.h"

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

	fp = fopen(MSLOG_PATH, "a");

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



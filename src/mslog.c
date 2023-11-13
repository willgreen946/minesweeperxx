#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "mslog.h"

void
mslog(const char * s)
{
  time_t curtime;
  char strtime[9];
  struct tm * timeinfo;
  FILE * fp = (FILE*) 0;

  if (!(fp = fopen(MSLOG_PATH, "a"))) {
    fprintf(stderr,
      "Failed to open log file:%s\n",
      strerror(errno));
    return;
  }

  time(&curtime);
  timeinfo = localtime(&curtime);
  strftime(strtime, sizeof(strtime), "%H:%M:%S", timeinfo);

  fprintf(fp, "[%s]:%s\n", strtime, s);

  if (fclose(fp)) {
    fprintf(stderr,
      "Failed to close log file:%s\n",
      strerror(errno));
    return;
  }
}

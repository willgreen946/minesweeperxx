#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "mslog.h"

const char * mserrstrs[] = {
	"Failed to refresh window",
	"Failed to create a new window",
	"Failed to move cursor",
	"Failed to start color",
	"Failed to init color pairing",
	"Failed to draw box",
	"Failed to set background color",
	"Failed to clear window",
	"Failed to move cursor",
	"Failed to read ./assets/title.txt",
	"Failed to hide the cursor",
	"Failed to show the cursor",
	"Failed to enable echoing",
	"Failed to disable echoing",
	"Failed to init the keypad",
	"Failed to enter raw mode",
	"Failed to start curses",
	"Failed to end curses",
	"Failed to destroy window",
};

const char *
msstrerror(int mserrno)
{
	if (!mserrstrs[mserrno])
		return "Undefined error";

	return mserrstrs[mserrno];
}

void
msgettime(char * t)
{
	time_t curtime;
	struct tm * timeinfo;

	time(&curtime);
	timeinfo = localtime(&curtime);

	/* Convert the current time to a string */
  strftime(t, sizeof(t), "%H:%M:%S", timeinfo);
}

void
mslog(int mserrno, const char * s)
{
  char strtime[9];
  FILE * fp = (FILE*) 0;

	/* Open the mslog file for appending */
  if (!(fp = fopen(MSLOG_PATH, "a"))) {
    fprintf(stderr,
      "Failed to open log file:%s\n",
      strerror(errno));
    return;
  }

	msgettime(strtime);

  fprintf(fp, "[%s] : %s : %s\n",
		strtime, s, msstrerror(mserrno));

  if (fclose(fp)) {
    fprintf(stderr,
      "Failed to close log file:%s\n",
      strerror(errno));
    return;
  }
}

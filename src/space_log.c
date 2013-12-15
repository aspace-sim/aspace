/* space_variables.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

const char *aspace_log = "log/space.log";
char *quick_space_unparse(dbref object);

char *quick_space_unparse(dbref object)
{
  switch (object) {
	case NOTHING: return "*NOTHING*\0";
	case AMBIGUOUS: return "*VARIABLE*\0";
	case HOME: return "*HOME*\0";
	default: return tprintf("%s(#%d%s)", Name(object), object, unparse_flags(object, GOD));
  }
}

void open_spacelog()
{
	FILE *fp = fopen(aspace_log, "a");

	if (fp == 0)
	{
		fprintf(stderr, T("WARNING: cannot open %s\n"), aspace_log);
		return;
	}

	fprintf(fp, "START OF LOG.\n");
	fflush(fp);
	fclose(fp);
}

void write_logfile(char *timestring, char *unp1, char *unp2, char *message) 
{
	FILE *fp = fopen(aspace_log, "a");

	if (fp == 0)
	{
		fprintf(stderr, T("WARNING: cannot open %s\n"), aspace_log);
		return;
	}

	fprintf(fp, "%s SPACE: %s %s %s\n", timestring, unp1, unp2, message); 
	fclose(fp);
}

void write_logchannel(char *timestring, char *unp1, char *unp2, char *message)
{
	CHAN *c = NULL;
	
	if (find_channel(LOG_CHANNEL, &c, GOD) == CMATCH_NONE) {
		write_logfile(timestring, unp1, unp2, message);
	} else {
		do_cemit(GOD, LOG_CHANNEL, tprintf("%s SPACE: %s %s %s", timestring, unp1, unp2, message), 0);
	}
}

void write_spacelog(dbref executor, dbref object, char *fmt)
{
	struct tm *ttm;
	char timebuf[18];

	ttm = localtime(&mudtime);
	strftime(timebuf, sizeof timebuf, "[%m/%d %H:%M:%S]", ttm);

	char *message = mush_strdup(fmt, "space_log_message");
	char *unp1 = mush_strdup(quick_space_unparse(executor), "space_log_executor");
	char *unp2 = mush_strdup(quick_space_unparse(object), "space_log_executor");

	switch (LOG_TYPE) {
		case 1: write_logfile(timebuf, unp1, unp2, message); break;
		case 2: write_logchannel(timebuf, unp1, unp2, message); break;
		case 3: write_logfile(timebuf, unp1, unp2, message);
			 write_logchannel(timebuf, unp1, unp2, message);
			 break;
		default: write_logfile(timebuf, unp1, unp2, message); break;
	}

	mush_free(unp1, "space_log_executor");
	mush_free(unp2, "space_log_executor");
	mush_free(message, "space_log_message");
}
/* ------------------------------------------------------------------------ */

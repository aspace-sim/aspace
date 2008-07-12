/* space_variables.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

const char *aspace_log = "log/space.log";
static char *quick_space_unparse(dbref object);

static char *
quick_space_unparse(dbref object)
{
  static char buff[BUFFER_LEN], *bp;

  switch (object) {
  case NOTHING:
    strcpy(buff, T("*NOTHING*"));
    break;
  case AMBIGUOUS:
    strcpy(buff, T("*VARIABLE*"));
    break;
  case HOME:
    strcpy(buff, T("*HOME*"));
    break;
  default:
    bp = buff;
    safe_format(buff, &bp, "%s(#%d%s)",
		Name(object), object, unparse_flags(object, GOD));
    *bp = '\0';
  }

  return buff;
}

void open_spacelog()
{
	FILE *fp;
	
	fp = fopen(aspace_log, "a");

	if (fp == 0)
	{
		fprintf(stderr, T("WARNING: cannot open %s\n"), aspace_log);
		return;
	}

	fprintf(fp, "START OF LOG.\n");
	fflush(fp);
	fclose(fp);
}

void write_logfile(dbref executor, dbref object, const char *fmt) 
{
	FILE *fp;
	struct tm *ttm;
	char timebuf[18];
	char unp1[BUFFER_LEN], unp2[BUFFER_LEN];

	ttm = localtime(&mudtime);
	strftime(timebuf, sizeof timebuf, "[%m/%d %H:%M:%S]", ttm);
	strcpy(unp1, quick_space_unparse(executor));
	strcpy(unp2, quick_space_unparse(object));

	fp = fopen(aspace_log, "a");

	if (fp == 0)
	{
		fprintf(stderr, T("WARNING: cannot open %s\n"), aspace_log);
		return;
	}

	fprintf(fp, T("%s SPACE: %s %s %s\n"), timebuf, unp1, unp2, fmt);
	fclose(fp);
}

void write_logchannel(dbref executor, dbref object, const char *fmt)
{
	CHAN *c;
	struct tm *ttm;
	char timebuf[18];
	char unp1[BUFFER_LEN], unp2[BUFFER_LEN];

	c = NULL;
	ttm = localtime(&mudtime);
	strftime(timebuf, sizeof timebuf, "[%m/%d %H:%M:%S]", ttm);
	strcpy(unp1, quick_space_unparse(executor));
	strcpy(unp2, quick_space_unparse(object));
	
	if (find_channel(LOG_CHANNEL, &c, GOD) == CMATCH_NONE) {
		write_logfile(executor, object, fmt);
	} else {
		do_cemit(GOD, LOG_CHANNEL, tprintf("%s SPACE: %s %s %s", timebuf, unp1, unp2, fmt), 0);
	}
}

void write_spacelog(dbref executor, dbref object, const char *fmt)
{
	switch (LOG_TYPE) {
		case 1: write_logfile(executor, object, fmt); break;
		case 2: write_logchannel(executor, object, fmt); break;
		case 3: write_logfile(executor, object, fmt);
				write_logchannel(executor, object, fmt);
				break;
		default: write_logfile(executor, object, fmt); break;
	}
}
/* ------------------------------------------------------------------------ */

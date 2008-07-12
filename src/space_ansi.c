/* space_ansi.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *ansi_red (const char *msg)
{
	static char buffer[1000];

	snprintf(buffer, sizeof(buffer), "%s%s%s",
	  ANSI_RED, msg, ANSI_WHITE);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_alert (const char *msg)
{
	static char buffer[1000];

	snprintf(buffer, sizeof(buffer), "%s[%s%s%s]%s",
	  ANSI_BLUE, ANSI_CYAN, msg, ANSI_BLUE, ANSI_WHITE);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_cmd (dbref enactor, const char *msg)
{
	static char buffer[1000];

	snprintf(buffer, sizeof(buffer), "%s[%s%s%s%s%s]--[%s%s%s]%s",
	  ANSI_BLUE, ANSI_YELLOW, ANSI_HILITE, Name(enactor), ANSI_NORMAL,
	  ANSI_BLUE, ANSI_CYAN, msg, ANSI_BLUE, ANSI_WHITE);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_warn (const char *msg)
{
	static char buffer[1000];

	snprintf(buffer, sizeof(buffer), "%s[%s%sWARNING%s%s]--[%s%s%s]%s",
	  ANSI_BLUE, ANSI_RED, ANSI_BLINK, ANSI_NORMAL, ANSI_BLUE, ANSI_CYAN,
	  msg, ANSI_BLUE, ANSI_WHITE);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_rainbow_scale (double a, int max)
{
	static char buffer[200];
	register int i;
	int color1 = 0, color2 = 0, b = (int) (a * max + 0.5);

	if (max < 5)
		max = 5;
	if (max > 75)
		max = 75;
	if (b < 0)
		b = 0;
	if (b > max)
		b = max;

	snprintf(buffer, sizeof(buffer), "%s[", ANSI_BLUE);

	for (i = 1 ; i <= max ; ++i) {
		if (i <= b) {
			color1 = ((int) (((i * 5) / (max + 1)) + 1));
		} else
			color1 = 0;
		if (color1 != color2) {
			switch (color1) {
				case 1: strncat(buffer, ANSI_RED, sizeof(buffer) - 1); break;
				case 2: strncat(buffer, ANSI_YELLOW, sizeof(buffer) - 1); break;
				case 3: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 4: strncat(buffer, ANSI_BLUE, sizeof(buffer) - 1); break;
				case 5: strncat(buffer, ANSI_MAGENTA, sizeof(buffer) - 1); break;
				default: strncat(buffer, ANSI_BLUE, sizeof(buffer) - 1); break;
			}
			color2 = color1;
		}
		if (i <= b) {
			strncat(buffer, "=", sizeof(buffer) - 1);
		} else
			strncat(buffer, "-", sizeof(buffer) - 1);
	}
	strncat(buffer, tprintf("%s]%s", ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_red_scale (double a, int max)
{
	static char buffer[200];
	register int i;
	int color1 = 0, color2 = 0, b = (int) (a * max + 0.5);

	if (max < 5)
		max = 5;
	if (max > 75)
		max = 75;
	if (b < 0)
		b = 0;
	if (b > max)
		b = max;

	snprintf(buffer, sizeof(buffer), "%s[", ANSI_BLUE);

	for (i = 1 ; i <= max ; ++i) {
		if (i <= b) {
			color1 = 1;
		} else
			color1 = 0;
		if (color1 != color2) {
			switch (color1) {
				case 1: strncat(buffer, ANSI_RED, sizeof(buffer) - 1); break;
				default: strncat(buffer, ANSI_BLUE, sizeof(buffer) - 1); break;
			}
			color2 = color1;
		}
		if (i <= b) {
			strncat(buffer, "=", sizeof(buffer) - 1);
		} else
			strncat(buffer, "-", sizeof(buffer) - 1);
	}
	strncat(buffer, tprintf("%s]%s", ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *ansi_stoplight_scale (double a, int max)
{
	static char buffer[200];
	register int i;
	int color1 = 0, color2 = 0, b = (int) (a * max + 0.5);

	if (max < 5)
		max = 5;
	if (max > 75)
		max = 75;
	if (b < 0)
		b = 0;
	if (b > max)
		b = max;

	snprintf(buffer, sizeof(buffer), "%s[", ANSI_BLUE);

	for (i = 1 ; i <= max ; ++i) {
		if (i <= b) {
			color1 = ((int) (((i * 10) / (max + 1)) + 1));
		} else
			color1 = 0;
		if (color1 != color2) {
			switch (color1) {
				case 1: strncat(buffer, ANSI_RED, sizeof(buffer) - 1); break;
				case 2: strncat(buffer, ANSI_YELLOW, sizeof(buffer) - 1); break;
				case 3: strncat(buffer, ANSI_YELLOW, sizeof(buffer) - 1); break;
				case 4: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 5: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 6: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 7: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 8: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 9: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				case 10: strncat(buffer, ANSI_GREEN, sizeof(buffer) - 1); break;
				default: strncat(buffer, ANSI_BLUE, sizeof(buffer) - 1); break;
			}
			color2 = color1;
		}
		if (i <= b) {
			strncat(buffer, "=", sizeof(buffer) - 1);
		} else
			strncat(buffer, "-", sizeof(buffer) - 1);
	}
	strncat(buffer, tprintf("%s]%s", ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

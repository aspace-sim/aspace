/* space_unparse.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *unparse_percent (double a)
{
	static char buffer[15];

	snprintf(buffer, sizeof(buffer), "%.0f%%", a * 100.0);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_percent_3 (double a)
{
	static char buffer[15];

	snprintf(buffer, sizeof(buffer), "%.3f%%", a * 100.0);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_percent_6 (double a)
{
	static char buffer[20];

	snprintf(buffer, sizeof(buffer), "%.6f%%", a * 100.0);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_damage (double a)
{
	static char buffer[50];

	if (a == 1.0) {
		return (char *) damage_name[0];
	} else if (a > 0.95) {
		return (char *) damage_name[1];
	} else if (a > 0.90) {
		return (char *) damage_name[2];
	} else if (a > 0.75) {
		return (char *) damage_name[3];
	} else if (a > 0.50) {
		return (char *) damage_name[4];
	} else if (a > 0.25) {
		return (char *) damage_name[5];
	} else if (a > 0.0) {
		return (char *) damage_name[6];
	} else if (a > -1.0) {
		return (char *) damage_name[7];
	} else {
		return (char *) damage_name[8];
	}
}

/* ------------------------------------------------------------------------ */

char *unparse_shield (int x)
{
	if (x < 0 || x >= MAX_SHIELD_NAME) {
		return (char *) "#-1 BAD SHIELD";
	} else
		return (char *) shield_name[x];
}

/* ------------------------------------------------------------------------ */

char *unparse_beam (int x)
{
	if (x < 0 || x >= MAX_BEAM_NAME) {
		return (char *) beam_name[0];
	} else
		return (char *) beam_name[x];
}

/* ------------------------------------------------------------------------ */

char *unparse_missile (int x)
{
	if (x < 0 || x >= MAX_MISSILE_NAME) {
		return (char *) missile_name[0];
	} else
		return (char *) missile_name[x];
}

/* ------------------------------------------------------------------------ */

char *unparse_range (double r)
{
	static char buffer[15];

	if (r > 999.9 * PARSEC) {
		snprintf(buffer, sizeof(buffer), "[%5.0f]", r / PARSEC);
	} else if (r > 99.99 * PARSEC) {
		snprintf(buffer, sizeof(buffer), "[%5.1f]", r / PARSEC);
	} else if (r > 9.999 * PARSEC) {
		snprintf(buffer, sizeof(buffer), "[%5.2f]", r / PARSEC);
	} else if (r > 9999999.0) {
		snprintf(buffer, sizeof(buffer), "[%5.3f]", r / PARSEC);
	} else if (r > 99999.9) {
		snprintf(buffer, sizeof(buffer), "%7.0f", r);
	} else if (r > 9999.99) {
		snprintf(buffer, sizeof(buffer), "%7.1f", r);
	} else if (r > 999.999) {
		snprintf(buffer, sizeof(buffer), "%7.2f", r);
	} else if (r > 99.9999) {
		snprintf(buffer, sizeof(buffer), "%7.3f", r);
	} else if (r > 9.99999) {
		snprintf(buffer, sizeof(buffer), "%7.4f", r);
	} else
		snprintf(buffer, sizeof(buffer), "%7.5f", r);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_distance (double r)
{
	static char buffer[20];

	snprintf(buffer, sizeof(buffer), "%s %s", unparse_range(r),
	  (r > 9999999.0) ? "PC": "SU");

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_speed (double s)
{
	static char buffer[10];

	if (s > 0.0) {
		if (s >= 100.0) {
			snprintf(buffer, sizeof(buffer), "w%5.1f", s);
		} else if (s >= 10.0) {
			snprintf(buffer, sizeof(buffer), "w%5.2f", s);
		} else if (s >= 1.0) {
			snprintf(buffer, sizeof(buffer), "w%5.3f", s);
		} else if (s >= 0.1) {
			snprintf(buffer, sizeof(buffer), "%5.2fi", s * 100);
		} else
			snprintf(buffer, sizeof(buffer), "%5.3fi", s * 100);
	} else if (s < 0.0) {
		if (s <= -100.0) {
			snprintf(buffer, sizeof(buffer), "w%5.0f", s);
		} else if (s <= -10.0) {
			snprintf(buffer, sizeof(buffer), "w%5.1f", s);
		} else if (s <= -1.0) {
			snprintf(buffer, sizeof(buffer), "w%5.2f", s);
		} else if (s <= -0.1) {
			snprintf(buffer, sizeof(buffer), "%5.1fi", s * 100);
		} else
			snprintf(buffer, sizeof(buffer), "%5.2fi", s * 100);
	} else
		strncpy(buffer, "None", sizeof(buffer) - 1);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_movement (int x)
{
	static char buffer[20];

	if (sdb[x].move.out == 0.0) {
		strncpy(buffer, "Stationary", sizeof(buffer) - 1);
	} else if (fabs(sdb[x].move.out) >= 1.0) {
		snprintf(buffer, sizeof(buffer), "Warp %f", sdb[x].move.out);
	} else
		snprintf(buffer, sizeof(buffer), "%.3f%% Impulse", sdb[x].move.out * 100.0);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_velocity (int x)
{
	static char buffer[20];
	int t;
	double v, vx, dx, dy, dz;

	if (sdb[x].status.tractored) {
		t = sdb[x].status.tractored;
	} else if (sdb[x].status.tractoring) {
		t = sdb[x].status.tractoring;
	} else
		t = 0;

	if (fabs(sdb[x].move.out) >= 1.0) {
		v = sdb[x].move.v * sdb[x].move.cochranes;
	} else
		v = sdb[x].move.v;

	if (t) {
		if (fabs(sdb[t].move.out) >= 1.0) {
			vx = sdb[t].move.v * sdb[t].move.cochranes;
		} else
			vx = sdb[t].move.v;
		dx = v * sdb[x].course.d[0][0] + vx * sdb[t].course.d[0][0];
		dy = v * sdb[x].course.d[0][1] + vx * sdb[t].course.d[0][1];
		dz = v * sdb[x].course.d[0][2] + vx * sdb[t].course.d[0][2];
		v = sqrt(dx * dx + dy * dy + dz * dz);
	}

	if (v == 0.0) {
		strncpy(buffer, "Stationary", sizeof(buffer) - 1);
	} else {
		snprintf(buffer, sizeof(buffer), "%s/sec", unparse_distance(fabs(v)));
	}

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_bearing (int n1, int n2)
{
	static char buffer[20];

	snprintf(buffer, sizeof(buffer), "%.3f %.3f", sdb2bearing(n1, n2), sdb2elevation(n1, n2));

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_power (double a)
{
	static char buffer[15];

	if (a == 0.0) {
		strncpy(buffer, "None", sizeof(buffer) - 1);
	} else if (a < 0.001) {
		snprintf(buffer, sizeof(buffer), "%.3f KW", a * 1000000.0);
	} else if (a < 1.0) {
		snprintf(buffer, sizeof(buffer), "%.3f MW", a * 1000.0);
	} else if (a < 1000.0) {
		snprintf(buffer, sizeof(buffer), "%.3f GW", a);
	} else if (a < 1000000.0) {
		snprintf(buffer, sizeof(buffer), "%.3f TW", a / 1000.0);
	} else
		snprintf(buffer, sizeof(buffer), "%.3f PW", a / 1000000.0);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_freq (double a)
{
	static char buffer[15];

	snprintf(buffer, sizeof(buffer), "%.3f GHz", a);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_cargo (double a)
{
	static char buffer[15];

	if (a > 0) {
		snprintf(buffer, sizeof(buffer), "%.0f mt", a);
	} else
		strncpy(buffer, "None", sizeof(buffer) - 1);

	return (buffer);
}

/* ------------------------------------------------------------------------ */

char *unparse_course (int x)
{
	static char buffer[25];

	if (sdb[x].course.roll_out != 0.0) {
		snprintf(buffer, sizeof(buffer), "%.2f %.2f %.2f",
			sdb[x].course.yaw_out, sdb[x].course.pitch_out, sdb[x].course.roll_out);
	} else {
		snprintf(buffer, sizeof(buffer), "%.3f %.3f",
			sdb[x].course.yaw_out, sdb[x].course.pitch_out);
	}

	return (buffer);
}

/* -------------------------------------------------------------------- */

char *unparse_arc (int a)
{
	static char buffer[10];

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (a & 1)
		strncat(buffer, "F", sizeof(buffer) - 1);
	if (a & 4)
		strncat(buffer, "A", sizeof(buffer) - 1);
	if (a & 8)
		strncat(buffer, "P", sizeof(buffer) - 1);
	if (a & 2)
		strncat(buffer, "S", sizeof(buffer) - 1);
	if (a & 16)
		strncat(buffer, "U", sizeof(buffer) - 1);
	if (a & 32)
		strncat(buffer, "D", sizeof(buffer) - 1);

	return (buffer);
}

/* -------------------------------------------------------------------- */

char *unparse_type (int x)
{
	if (sdb[x].structure.type < 0 || sdb[x].structure.type >= MAX_TYPE_NAME) {
		return (char *) "#-1 BAD TYPE";
	} else
		return (char *) type_name[sdb[x].structure.type];
}

/* -------------------------------------------------------------------- */

char *unparse_quadrant (int x)
{
	if (sdb[x].move.quadrant < 0 || sdb[x].move.quadrant >= MAX_QUADRANT_NAME) {
		return (char *) "#-1 BAD QUADRANT";
	} else
		return (char *) quadrant_name[sdb[x].move.quadrant];
}

/* -------------------------------------------------------------------- */

char *unparse_class (int x)
{
	ATTR *a;
	static char buffer[20];

	a = atr_get(sdb[x].object, CLASS_ATTR_NAME);
	if (a == NULL)
		return (char *) "#-1 BAD CLASS";

	strncpy(buffer, atr_value(a), sizeof(buffer) - 1);

	return (buffer);
}

/* -------------------------------------------------------------------- */

char *unparse_identity (int n1, int n2)
{
	static char buffer[100];
	int slist;

	if (n1 == n2)
		return (char *) Name(sdb[n1].object);
	if (sdb[n1].location == n2)
		return (char *) Name(sdb[n2].object);
	if (n1 == sdb[n2].location)
		return (char *) Name(sdb[n2].object);

	slist = sdb2slist(n1, n2);
	if (slist == SENSOR_FAIL) {
		strncpy(buffer, "unknown contact", sizeof(buffer) - 1);
	} else {
		if (sdb[n1].slist.lev[slist] >= 0.5 && !sdb[n2].cloak.active) {
			snprintf(buffer, sizeof(buffer), "%s (%s)",
			  Name(sdb[n2].object), unparse_integer(sdb2contact(n1, n2)));
		} else if (sdb[n1].slist.lev[slist] >= 0.25 && !sdb[n2].cloak.active) {
			snprintf(buffer, sizeof(buffer), "%s class (%s)",
			  unparse_class(n2), unparse_integer(sdb2contact(n1, n2)));
		} else
			snprintf(buffer, sizeof(buffer), "contact (%s)",
			  unparse_integer(sdb2contact(n1, n2)));
	}

	return (buffer);
}

/* -------------------------------------------------------------------- */

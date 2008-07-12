/* space_border.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */
/* Border table: Format: Empire Name, Radius, X (SU), Y (SU), Z(SU)
 *
 * Please Note: The NULL at the bottom IS REQUIRED
 */
struct aspace_empire_info aspace_empire_table[MAX_EMPIRE_NAME] =
{
	{"Terran",				7.0,	-28307977141382.1,					0.0,	-37716017559.8},
	{"Vulcan",			   10.0,	-28342323618634.8,		  57217386371.0,	-26203421510.1},
	{"Andorian",			4.0,	-28150809068534.8,		 167825941181.7,	 -3671934950.1},
	{"Nausicaan",			3.0,	-28075120923666.4,		-402515040712.8,	 56893392110.6},
	{"Tellarite",			3.0,	-28286809516375.1,		-140079689860.5,	 50324022775.4},
	{"Klingon",			   20.0,	-28213327618136.5,		 794517239093.5,	-71649016423.1},
	{"Suliban",				5.0,	-27988981649658.3,		  83596690479.6,	 52672209747.7},
	{"Orion",				5.0,	-28240515365066.0,		 579934297659.4,	-71855755617.8},
	{"Romulan",			   15.0,	-28867086322252.6,		 718341560004.9,	 -5554187319.6},
	{"(Unclaimed)",		15000.0,				  0.0,					0.0,			   0.0},
	{NULL,					0.0,				  0.0,					0.0,			   0.0}
};

/* Used for returning the empire name that the space-object is in */
char *unparse_empire (int x)
{
	struct aspace_empire_info* aEmpInfo;
	aEmpInfo = aspace_empire_table;

	if (sdb[x].move.empire < 0 || sdb[x].move.empire >= MAX_EMPIRE_NAME) {
		return (char *) "#-1 BAD TERRITORY";
	} else {
		return (char *) aEmpInfo[sdb[x].move.empire].name;
		//return (char *) empire_name[sdb[x].move.empire];
	}
}

/* Alerts the ship when it has entered an empires space */
void alert_enter_empire (int x)
{
	do_console_notify(x, console_helm, 0, 0,
	  ansi_warn(tprintf("Entering %s space", unparse_empire(x))));
	return;
}

/* Alerts the ship when it has departed an empires space */
void alert_exit_empire (int x)
{
	do_console_notify(x, console_helm, 0, 0,
	  ansi_alert(tprintf("Departing %s space", unparse_empire(x))));
	return;
}

/*
 * Alerts any space-object that is in range of the inbound or outbound border crossing
 */
void alert_border_cross (int x, int a, int way)
{
	register int i;

	if (sdb[x].move.out != 0.0)
		for (i = MIN_SPACE_OBJECTS; i <= max_space_objects; ++i)
			if (sdb[i].status.active)
				 if (sdb[i].space == sdb[x].space)
					if (sdb[i].move.empire == a)
						if (i != x)
							if (sdb2range(x, i) < MAX_NOTIFICATION_DISTANCE) {
								if (way) {
									do_console_notify(i, console_helm, console_science, console_security,
									  ansi_warn(tprintf("Inbound border crossing reported at %.3f %.3f %.3f",
									  su2pc(sdb[x].coords.x - sdb[i].coords.xo),
									  su2pc(sdb[x].coords.y - sdb[i].coords.yo),
									  su2pc(sdb[x].coords.z - sdb[i].coords.zo))));
								} else
									do_console_notify(i, console_helm, console_science, console_security,
									  ansi_warn(tprintf("Outbound border crossing reported at %.3f %.3f %.3f",
									  su2pc(sdb[x].coords.x - sdb[i].coords.xo),
									  su2pc(sdb[x].coords.y - sdb[i].coords.yo),
									  su2pc(sdb[x].coords.z - sdb[i].coords.zo))));
							}
	return;
}

/* 
 * Updates the space-object to reflect what empire it is currently inside
 * announces both inbound and outbound and also when you enter and depart
 */
void up_empire (void)
{
	double dx, dy, dz;
	register int empire = 0;
	struct aspace_empire_info* aEmpInfo;

	for (aEmpInfo = aspace_empire_table; aEmpInfo->name ; aEmpInfo++)
	{
		dx = (aEmpInfo->x - sdb[n].coords.x) / PARSEC;
		dy = (aEmpInfo->y - sdb[n].coords.y) / PARSEC;
		dz = (aEmpInfo->z - sdb[n].coords.z) / PARSEC;
		if ((dx * dx + dy * dy + dz * dz) < (aEmpInfo->radius * aEmpInfo->radius)) {
			if (sdb[n].move.empire != empire) {
				if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
					alert_exit_empire(n);
					alert_border_cross (n, sdb[n].move.empire, 0);
				}
				sdb[n].move.empire = empire;
				if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
					alert_enter_empire(n);
					alert_border_cross (n, sdb[n].move.empire, 1);
				}
			}
			break;
		}
		empire++;
	}
	return;
}

/*
 * Border report for a space-object
 */
int do_border_report (dbref enactor)
{
	static char buffer[BUFFER_LEN];
	struct aspace_empire_info* aEmpInfo;

	double range;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.lrs_exist) {
		notify(enactor, ansi_red(tprintf("%s has no long-range sensors.", Name(sdb[n].object))));
	} else if (!sdb[n].sensor.lrs_active) {
		notify(enactor, ansi_red("Long-range sensors are inactive."));
	} else {
		snprintf(buffer, sizeof(buffer),
		  "%s%s--[%sBorder Report%s]--------------------------------------------------------------%s\n%sEmpire               Bearing Range   Status   Center Coordinates%s\n%s-------------------- ------- ------- -------- ---------------------------------%s\n",
		  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL,
		  ANSI_CYAN, ANSI_WHITE, ANSI_BLUE, ANSI_WHITE);

		for (aEmpInfo = aspace_empire_table; aEmpInfo->name; aEmpInfo++)
		{
			range = xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, aEmpInfo->x, aEmpInfo->y, aEmpInfo->z) / PARSEC;
			if (fabs(range - aEmpInfo->radius) >= 100.0)
				continue;
			strncat(buffer, tprintf("%-20.20s ", aEmpInfo->name), sizeof(buffer) - 1);
			if (range > aEmpInfo->radius) {
				strncat(buffer, tprintf("%3d %-3d %-7.7s Outside  ",
				  (int) xy2bearing((aEmpInfo->x - sdb[n].coords.x), (aEmpInfo->y - sdb[n].coords.y)),
				  (int) xyz2elevation((aEmpInfo->x - sdb[n].coords.x), (aEmpInfo->y - sdb[n].coords.y), (aEmpInfo->z - sdb[n].coords.z)),
				  unparse_range((range - aEmpInfo->radius) * PARSEC)), sizeof(buffer) - 1);
			} else {
				strncat(buffer, tprintf("%3d %-3d %-7.7s Inside   ",
				  (int) xy2bearing((sdb[n].coords.x - aEmpInfo->x), (sdb[n].coords.y - aEmpInfo->y)),
				  (int) xyz2elevation((sdb[n].coords.x - aEmpInfo->x), (sdb[n].coords.y - aEmpInfo->y), (sdb[n].coords.z - aEmpInfo->z)),
				  unparse_range((aEmpInfo->radius - range) * PARSEC)), sizeof(buffer) - 1);
			}
			strncat(buffer, tprintf("%10.3f %10.3f %10.3f\n",
			  (aEmpInfo->x - sdb[n].coords.xo) / PARSEC,
			  (aEmpInfo->y - sdb[n].coords.yo) / PARSEC,
			  (aEmpInfo->z - sdb[n].coords.zo) / PARSEC), sizeof(buffer) - 1);
		}

		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_Course(n), sizeof(buffer) - 1);
		strncat(buffer, format_Speed(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Location(n), sizeof(buffer) - 1);
		strncat(buffer, format_Velocity(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}
/* ------------------------------------------------------------------------ */


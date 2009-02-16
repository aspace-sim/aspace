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

/* New functions for dealing with the new border system */

void
free_borderinfo(void *ptr) {
  spaceborder_info *si = (spaceborder_info *) ptr;
  mush_free(si->data, "border_info");
  mush_free(si->border_id, "spaceborder_id");
  mush_free(si, "spaceborder_info");
}

char* addNewBorder(int border_number, const char* name, double radius, double x, double y, double z)
{
	spaceborder_info *si = NULL;
	struct aspace_border_info* newBorder;
	
	static char addBuffer[1000];
	
	newBorder = mush_malloc(sizeof(struct aspace_border_info), "border_info");
	
	newBorder->name = mush_strdup(name, "spaceborder_name");
	newBorder->empire_id = 0;
	newBorder->radius = radius;
	newBorder->x = x;
	newBorder->y = y;
	newBorder->z = z;

	si = mush_malloc(sizeof(spaceborder_info), "spaceborder_info");
	si->data = newBorder;
	si->border_id = mush_strdup(unparse_integer(border_number),"spaceborder_id");
	
	addBuffer[0] = '\0';
	
	if( hash_add(&aspace_borders, si->border_id, si) )
		strncat(addBuffer, "New Border Created.", sizeof(addBuffer) - 1);
	else
		strncat(addBuffer, "#-1 BORDER NOT CREATED.", sizeof(addBuffer) - 1);
		
	return addBuffer;
}

char *deleteBorder(const char* border)
{
	static char deleteBuffer[1000];
	
	deleteBuffer[0] = '\0';

	if (!border || !*border) {
		strncat(deleteBuffer, "#-1 BORDER NOT SUPPLIED", sizeof(deleteBuffer) - 1);
		return deleteBuffer;
	}
	
	hashdelete(border, &aspace_borders);
	
	return "";
}

char *border_line_bot (spaceborder_info *sbi)
{
	static char buffer[1000];

	snprintf(buffer, sizeof(buffer), "%d|%s|%.1f|%.1f|%.1f|%.1f",
	  parse_integer(sbi->border_id),
	  sbi->data->name,
	  sbi->data->radius,
	  sbi->data->x,
	  sbi->data->y,
	  sbi->data->z);

	return (buffer);
}

char *list_borders()
{
        spaceborder_info *sbi = NULL;
		static char listBuffer[BUFFER_LEN];
		int first = 0;
        
		listBuffer[0] = '\0';
		
        for (sbi = (spaceborder_info *) hash_firstentry(&aspace_borders); sbi != NULL; sbi = (spaceborder_info *) hash_nextentry(&aspace_borders)) {
			if (first)
				strncat(listBuffer, "~", sizeof(listBuffer) - 1);
			
			strncat(listBuffer, border_line_bot(sbi), sizeof(listBuffer) - 1);
			++first;
        }
		
		return listBuffer;
}

char *edit_border(const char* border_id, const char* setting, const char* new_value)
{
	spaceborder_info *si = NULL;
	static char editBuffer[1000];

	si = hashfind(border_id,&aspace_borders);
	
	editBuffer[0] = '\0';
	
	if (si != NULL)
	{
		switch (setting[0]) {
			case 'n': /* Name */
					si->data->name = mush_strdup(new_value, "spaceborder_name");
				break;
			case 'r': /* Radius */
					si->data->radius = parse_number(new_value);
				break;
			case 'x': /* X Coordinate of Centre */
					si->data->x = parse_number(new_value);
				break;
			case 'y': /* Y Coordinate of Centre */
					si->data->y = parse_number(new_value);
				break;
			case 'z': /* Z Coordinate of Centre */
					si->data->z = parse_number(new_value);
				break;
			default: strncat(editBuffer, "#-1 BORDER SETTING NOT FOUND.", sizeof(editBuffer) - 1);
				break;
		}
	} else {
		strncat(editBuffer, "#-1 BORDER NOT FOUND.", sizeof(editBuffer) - 1);
	}
	
	return editBuffer;
}

/* Used for returning the empire name that the space-object is in */
char *unparse_empire (int x)
{
	spaceborder_info *si;

	si = hashfind(unparse_integer(sdb[x].move.empire),&aspace_borders);
	
	if (si != NULL)
	{
		return (char *) si->data->name;
	} else {
		return (char *) "#-1 BAD TERRITORY";
	}
}

/*char *unparse_empire (int x)
{
	double dx, dy, dz;
	spaceborder_info *si;
	char* empire_name;
	
	for (si = (spaceborder_info *) hash_firstentry(&aspace_borders); si != NULL; si = (spaceborder_info *) hash_nextentry(&aspace_borders)) {
	
		dx = (si->data->x - sdb[x].coords.x) / PARSEC;
		dy = (si->data->y - sdb[x].coords.y) / PARSEC;
		dz = (si->data->z - sdb[x].coords.z) / PARSEC;
		if ((dx * dx + dy * dy + dz * dz) < (si->data->radius * si->data->radius)) {
			empire_name = (char *) si->data->name;
			break;
		}
    }
	
	return empire_name;
}*/

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
	spaceborder_info *si;

	for (si = (spaceborder_info *) hash_firstentry(&aspace_borders); si != NULL; si = (spaceborder_info *) hash_nextentry(&aspace_borders))
	{
		dx = (si->data->x - sdb[n].coords.x) / PARSEC;
		dy = (si->data->y - sdb[n].coords.y) / PARSEC;
		dz = (si->data->z - sdb[n].coords.z) / PARSEC;
		if ((dx * dx + dy * dy + dz * dz) < (si->data->radius * si->data->radius)) {
			if (sdb[n].move.empire != parse_integer(si->border_id)) {
				if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
					alert_exit_empire(n);
					alert_border_cross (n, sdb[n].move.empire, 0);
				}
				sdb[n].move.empire = parse_integer(si->border_id);
				if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
					alert_enter_empire(n);
					alert_border_cross (n, sdb[n].move.empire, 1);
				}
			}
			break;
		}
	}
	return;
}

/*
 * Border report for a space-object
 */
int do_border_report (dbref enactor)
{
	static char buffer[BUFFER_LEN];
	spaceborder_info *sbi;
	
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

		for (sbi = (spaceborder_info *) hash_firstentry(&aspace_borders); sbi != NULL; sbi = (spaceborder_info *) hash_nextentry(&aspace_borders))
		{
			range = xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, sbi->data->x, sbi->data->y, sbi->data->z) / PARSEC;
			if (fabs(range - sbi->data->radius) >= 100.0)
				continue;
			strncat(buffer, tprintf("%-20.20s ", sbi->data->name), sizeof(buffer) - 1);
			if (range > sbi->data->radius) {
				strncat(buffer, tprintf("%3d %-3d %-7.7s Outside  ",
				  (int) xy2bearing((sbi->data->x - sdb[n].coords.x), (sbi->data->y - sdb[n].coords.y)),
				  (int) xyz2elevation((sbi->data->x - sdb[n].coords.x), (sbi->data->y - sdb[n].coords.y), (sbi->data->z - sdb[n].coords.z)),
				  unparse_range((range - sbi->data->radius) * PARSEC)), sizeof(buffer) - 1);
			} else {
				strncat(buffer, tprintf("%3d %-3d %-7.7s Inside   ",
				  (int) xy2bearing((sdb[n].coords.x - sbi->data->x), (sdb[n].coords.y - sbi->data->y)),
				  (int) xyz2elevation((sdb[n].coords.x - sbi->data->x), (sdb[n].coords.y - sbi->data->y), (sdb[n].coords.z - sbi->data->z)),
				  unparse_range((sbi->data->radius - range) * PARSEC)), sizeof(buffer) - 1);
			}
			strncat(buffer, tprintf("%10.3f %10.3f %10.3f\n",
			  (sbi->data->x - sdb[n].coords.xo) / PARSEC,
			  (sbi->data->y - sdb[n].coords.yo) / PARSEC,
			  (sbi->data->z - sdb[n].coords.zo) / PARSEC), sizeof(buffer) - 1);
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


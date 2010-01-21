/* space_border.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

/* New functions for dealing with the new border system */

void
free_borderinfo(aspace_borders *ptr) {
  mush_free(ptr->name, "spaceborder_name");
  mush_free(ptr, "border_info");
}

char* addNewBorder(int border_number, const char* name, double radius, double x, double y, double z)
{
	aspace_borders* newBorder;
	
	static char addBuffer[1000];
	
	newBorder = mush_malloc(sizeof(aspace_borders), "border_info");
	
	newBorder->name = mush_strdup(name, "spaceborder_name");
	newBorder->empire_id = 0;
	newBorder->radius = radius;
	newBorder->x = x;
	newBorder->y = y;
	newBorder->z = z;
	
	addBuffer[0] = '\0';
	
	if( im_insert(border_map, border_number, newBorder ))
		strncat(addBuffer, "New Border Created.", sizeof(addBuffer) - 1);
	else
		strncat(addBuffer, "#-1 BORDER NOT CREATED.", sizeof(addBuffer) - 1);
		
	return addBuffer;
}

char *deleteBorder(int border)
{
	static char deleteBuffer[1000];
	aspace_borders *theBorder;
	
	deleteBuffer[0] = '\0';

	if (!border) {
		strncat(deleteBuffer, "#-1 BORDER NOT SUPPLIED", sizeof(deleteBuffer) - 1);
		return deleteBuffer;
	}
	
	theBorder = im_find(border_map, border);
	
	if (theBorder != NULL)
		free_borderinfo(theBorder);
		
	im_delete(border_map, border);
	
	return "";
}

char *border_line_bot (int border_id, aspace_borders *sbi)
{
	static char buffer[1000];

	if (sbi != NULL) {
		snprintf(buffer, sizeof(buffer), "%d|%s|%.1f|%.1f|%.1f|%.1f",
			border_id,
			sbi->name,
			sbi->radius,
			sbi->x,
			sbi->y,
			sbi->z);
	}
	
	return (buffer);
}

char *list_borders()
{
		static char listBuffer[BUFFER_LEN];
		int first = 0;
		int index = 0;
        
		listBuffer[0] = '\0';
		
        for (index = 1; index <= im_count(border_map); index++) {
			if (first)
				strncat(listBuffer, "~", sizeof(listBuffer) - 1);
			
			strncat(listBuffer, border_line_bot(index, im_find(border_map, index)), sizeof(listBuffer) - 1);
			++first;
        }
		
		return listBuffer;
}

char *edit_border(int border_id, const char* setting, const char* new_value)
{
	aspace_borders *si = NULL;
	static char editBuffer[1000];

	si = im_find(border_map, border_id);
	
	editBuffer[0] = '\0';
	
	if (si != NULL)
	{
		switch (setting[0]) {
			case 'n': /* Name */
					si->name = mush_strdup(new_value, "spaceborder_name");
				break;
			case 'r': /* Radius */
					si->radius = parse_number(new_value);
				break;
			case 'x': /* X Coordinate of Centre */
					si->x = parse_number(new_value);
				break;
			case 'y': /* Y Coordinate of Centre */
					si->y = parse_number(new_value);
				break;
			case 'z': /* Z Coordinate of Centre */
					si->z = parse_number(new_value);
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
	aspace_borders *si;

	si = im_find(border_map, sdb[x].move.empire);
	
	if (si != NULL)
	{
		return (char *) si->name;
	} else {
		return (char *) "#-1 BAD TERRITORY";
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
	int index = 0;
	aspace_borders *si = NULL;

	for (index = 1; index <= im_count(border_map); index++)
	{
		si = im_find(border_map, index);
		
		if (si != NULL) {
			dx = (si->x - sdb[n].coords.x) / PARSEC;
			dy = (si->y - sdb[n].coords.y) / PARSEC;
			dz = (si->z - sdb[n].coords.z) / PARSEC;
			if ((dx * dx + dy * dy + dz * dz) < (si->radius * si->radius)) {
	
				if (sdb[n].move.empire != index) {
					if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
						alert_exit_empire(n);
						alert_border_cross (n, sdb[n].move.empire, 0);
					}
					sdb[n].move.empire = index;
					if (get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
						alert_enter_empire(n);
						alert_border_cross (n, sdb[n].move.empire, 1);
					}
				}
				break;
			}
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
	int index = 0;
	aspace_borders *sbi;
	
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

		for (index = 1; index <= im_count(border_map); index++)
		{
			sbi = im_find(border_map, index);
		
			if (sbi != NULL) {
				range = xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, sbi->x, sbi->y, sbi->z) / PARSEC;
				if (fabs(range - sbi->radius) >= 100.0)
					continue;
				strncat(buffer, tprintf("%-20.20s ", sbi->name), sizeof(buffer) - 1);
				if (range > sbi->radius) {
					strncat(buffer, tprintf("%3d %-3d %-7.7s Outside  ",
					  (int) xy2bearing((sbi->x - sdb[n].coords.x), (sbi->y - sdb[n].coords.y)),
					  (int) xyz2elevation((sbi->x - sdb[n].coords.x), (sbi->y - sdb[n].coords.y), (sbi->z - sdb[n].coords.z)),
					  unparse_range((range - sbi->radius) * PARSEC)), sizeof(buffer) - 1);
				} else {
					strncat(buffer, tprintf("%3d %-3d %-7.7s Inside   ",
					  (int) xy2bearing((sdb[n].coords.x - sbi->x), (sdb[n].coords.y - sbi->y)),
					  (int) xyz2elevation((sdb[n].coords.x - sbi->x), (sdb[n].coords.y - sbi->y), (sdb[n].coords.z - sbi->z)),
					  unparse_range((sbi->radius - range) * PARSEC)), sizeof(buffer) - 1);
				}
				strncat(buffer, tprintf("%10.3f %10.3f %10.3f\n",
				  (sbi->x - sdb[n].coords.xo) / PARSEC,
				  (sbi->y - sdb[n].coords.yo) / PARSEC,
				  (sbi->z - sdb[n].coords.zo) / PARSEC), sizeof(buffer) - 1);
			}
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


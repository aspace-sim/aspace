/* space_border.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

/* New functions for dealing with the new border system */

void free_borderinfo(void *ptr) {
  aspace_borders *border = (aspace_borders *) ptr;
  mush_free(border->name, "spaceborder_name");
  mush_free(border, "border_info");
}

void addNewBorder(dbref executor, int border_number, const char* name, double radius, double x, double y, double z, char *buff, char **bp)
{
	aspace_borders* newBorder;
	newBorder = im_find(border_map, border_number);
	if (newBorder != NULL) {
		safe_str("#-1 BORDER ALREADY EXISTS", buff, bp);
		return;
	}
		
	newBorder = mush_malloc(sizeof(aspace_borders), "border_info");
	
	newBorder->name = mush_strdup(name, "spaceborder_name");
	newBorder->empire_id = 0;
	newBorder->radius = radius;
	newBorder->x = x;
	newBorder->y = y;
	newBorder->z = z;
	
	if( im_insert(border_map, border_number, newBorder )) {
		safe_str("New Border Created.", buff, bp);
		write_spacelog(executor, executor, tprintf("Border Created: %s", newBorder->name));
	} else
		safe_str("#-1 BORDER NOT CREATED.", buff, bp);
}

void deleteBorder(dbref executor, int border, char *buff, char **bp)
{
	aspace_borders *theBorder;

	if (!border) {
		safe_str("#-1 BORDER NOT SUPPLIED", buff, bp);
		return;
	}
	
	theBorder = im_find(border_map, border);
	if (theBorder !=NULL) {
		write_spacelog(executor, executor, tprintf("Border deleted: %s", theBorder->name));
		free_borderinfo(theBorder);
		im_delete(border_map, border);
	} else {
		safe_str("#-1 BORDER NOT FOUND", buff, bp);
		return;
	}
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

void list_borders(char *buff, char **bp)
{
	int first = 0;
	int index = 0;
        	
       for (index = 1; index <= im_count(border_map); index++) {
		if (first) {
			safe_str("~", buff, bp);
		}
		
		safe_str(border_line_bot(index, im_find(border_map, index)), buff, bp);
		++first;
        }
}

void edit_border(dbref executor, int border_id, const char* setting, const char* new_value, char *buff, char **bp)
{
	aspace_borders *si = NULL;

	si = im_find(border_map, border_id);
	
	if (si != NULL)
	{
		switch (setting[0]) {
			case 'n': /* Name */
				si->name = mush_strdup(new_value, "spaceborder_name");
				safe_format(buff, bp, "Border Name changed for border %u to %s", border_id, si->name);
				break;
			case 'r': /* Radius */
				si->radius = parse_number(new_value);
				safe_format(buff, bp, "Border Radius changed for border %u to %f", border_id, si->radius);
				break;
			case 'x': /* X Coordinate of Centre */
				si->x = parse_number(new_value);
				safe_format(buff, bp, "Border CoordX changed for border %u to %f", border_id, si->x);
				break;
			case 'y': /* Y Coordinate of Centre */
				si->y = parse_number(new_value);
				safe_format(buff, bp, "Border CoordY changed for border %u to %f", border_id, si->y);
				break;
			case 'z': /* Z Coordinate of Centre */
				si->z = parse_number(new_value);
				safe_format(buff, bp, "Border CoordZ changed for border %u to %f", border_id, si->z);
				break;
			default: safe_str("#-1 BORDER SETTING NOT FOUND.", buff, bp);
				break;
		}
	} else {
		safe_str("#-1 BORDER NOT FOUND.", buff, bp);
	}
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
	console_message(x, "helm", 	ansi_warn(tprintf("Entering %s space", unparse_empire(x))));
	return;
}

/* Alerts the ship when it has departed an empires space */
void alert_exit_empire (int x)
{
	console_message(x, "helm", ansi_alert(tprintf("Departing %s space", unparse_empire(x))));
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
									console_message(i, "helm science security",
									  ansi_warn(tprintf("Inbound border crossing reported at %.3f %.3f %.3f",
									  su2pc(sdb[x].coords.x - sdb[i].coords.xo),
									  su2pc(sdb[x].coords.y - sdb[i].coords.yo),
									  su2pc(sdb[x].coords.z - sdb[i].coords.zo))));
								} else
									console_message(i, "helm science security",
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
					if ((int)get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
						alert_exit_empire(n);
						alert_border_cross (n, sdb[n].move.empire, 0);
					}
					sdb[n].move.empire = index;
					if ((int)get_random_long(0,100) < ((int) (sdb[n].sensor.lrs_signature * sdb[n].sensor.visibility * 100.0))) {
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


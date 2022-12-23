/* space_status.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *contact_flags (int x)
{
	register int i;
	static char flags[20];
	char *fp = flags;

	if (sdb[x].status.active)
		*fp++ = 'A';
	if (sdb[x].move.in != sdb[x].move.out)
		*fp++ = 'a';
	if (sdb[x].beam.banks) {
		for (i = 0 ; i < sdb[x].beam.banks ; ++i)
			if (sdb[x].blist.lock[i] == n) {
				*fp++ = 'B'; break;
			} else if (sdb[x].blist.active[i]) {
				*fp++ = 'b'; break;
			}
	}
	if (sdb[x].cloak.active)
		*fp++ = 'C';
	if (sdb[x].status.connected)
		*fp++ = 'c';
	if (sdb[x].status.docked)
		*fp++ = 'd';
	if ((sdb[x].power.total * sdb[x].alloc.sensors > 0.0) && sdb[x].sensor.ew_active)
		*fp++ = 'E';
	if (sdb[x].power.total != 0.0)
		*fp++ = 'e';
	if (sdb[x].status.landed)
		*fp++ = 'l';
	if (sdb[x].missile.tubes) {
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i)
			if (sdb[x].mlist.lock[i] == n) {
				*fp++ = 'M'; break;
			} else if (sdb[x].mlist.active[i]) {
				*fp++ = 'm'; break;
			}
	}
	if (sdb[x].shield.exist)
		for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
			if (sdb[x].shield.active[i] * sdb[x].alloc.shield[i] * sdb[x].power.total * sdb[x].shield.damage[i] > 0.0) {
				*fp++ = 'S'; break; }
	if (sdb[x].status.tractoring)
		*fp++ = 'T';
	if (sdb[x].status.tractored)
		*fp++ = 't';
	if (sdb[x].status.crippled)
		*fp++ = 'X';
	*fp = '\0';

	return (flags);
}

char *contact_line (int contact)
{
	int x = sdb[n].slist.sdb[contact];
	//int l;
	//register int i;
	static char buffer[200], arc1[10], arc2[10];
	double level = sdb[n].slist.lev[contact] * 100.0;

	if (level < 0.0) {
		level = 0.0;
	} else if (level > 100.0)
		level = 100.0;

	strncpy(arc1, unparse_arc(sdb2arc(n, x)), sizeof(arc1) - 1);
	strncpy(arc2, unparse_arc(sdb2arc(x, n)), sizeof(arc1) - 1);

	if (level < 25) {
		snprintf(buffer, sizeof(buffer), "%s%3d%s %-4.4s %3.0f %3.0f %-3.0f %7.7s %-5.5s %3.0f %-3.0f %6.6s %-5.5s %s%s%s%s\n",
			ANSI_CYAN, sdb[n].slist.num[contact], ANSI_WHITE,
			unparse_type(x), level,
			sdb2bearing(n, x), sdb2elevation(n, x),
			unparse_range(sdb2range(n, x)), arc1,
			sdb[x].course.yaw_out, sdb[x].course.pitch_out,
			unparse_speed(sdb[x].move.out), arc2,
			ANSI_HILITE,(sdb[n].iff.frequency == sdb[x].iff.frequency ?  ANSI_GREEN : ANSI_RED),"*",ANSI_NORMAL);
	} else if (level < 50.0) {
		snprintf(buffer, sizeof(buffer), "%s%3d%s %-4.4s %3.0f %3.0f %-3.0f %7.7s %-5.5s %3.0f %-3.0f %6.6s %-5.5s %s%s%s%s%16.16s %-6.6s\n",
			ANSI_CYAN, sdb[n].slist.num[contact], ANSI_WHITE,
			unparse_type(x), level,
			sdb2bearing(n, x), sdb2elevation(n, x),
			unparse_range(sdb2range(n, x)), arc1,
			sdb[x].course.yaw_out, sdb[x].course.pitch_out,
			unparse_speed(sdb[x].move.out), arc2,
			ANSI_HILITE,(sdb[n].iff.frequency == sdb[x].iff.frequency ?  ANSI_GREEN : ANSI_RED),"*",ANSI_NORMAL,
			(sdb[x].cloak.active ? "(cloaked)" : unparse_class(x)), contact_flags(x));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%3d%s %-4.4s %3.0f %3.0f %-3.0f %7.7s %-5.5s %3.0f %-3.0f %6.6s %-5.5s %s%s%s%s%s%-16.16s%s %-6.6s\n",
			ANSI_CYAN, sdb[n].slist.num[contact], ANSI_WHITE,
			unparse_type(x), level,
			sdb2bearing(n, x), sdb2elevation(n, x),
			unparse_range(sdb2range(n, x)), arc1,
			sdb[x].course.yaw_out, sdb[x].course.pitch_out,
			unparse_speed(sdb[x].move.out), arc2,
			ANSI_HILITE,(sdb[n].iff.frequency == sdb[x].iff.frequency ?  ANSI_GREEN : ANSI_RED),"*",ANSI_NORMAL,
			ANSI_HILITE, (sdb[x].cloak.active ? "(cloaked)" : Name(sdb[x].object)), ANSI_NORMAL,
			contact_flags(x));
	}
	return (buffer);
}

int do_sensor_contacts (char *a, dbref enactor)
{
	static char buffer[BUFFER_LEN];
	register int contact = contact2slist(n, atoi(a));
	int x, ctype = 0, first;

	switch (a[0]) {
		case 'a': case 'A':
			ctype = 4;
			break;
		case 'b': case 'B':
			ctype = 2;
			break;
		case 'p': case 'P':
			ctype = 3;
			break;
		case 's': case 'S':
			switch (a[1]) {
				case 'h': case 'H':
					ctype = 1;
					break;
				case 't': case 'T':
					ctype = 5;
					break;
				default:
					ctype = 0;
					break;
			} break;
		default:
			ctype = 0;
			break;
	}

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.contacts) {
		snprintf(buffer, sizeof(buffer),
		  "%s%s--[%sSensor Report%s]--------------------------------------------------------------%s\n%s%s\n%s%s\n%s",
		  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL,
		  format_Course(n), format_Speed(n), format_Location(n), format_Velocity(n),
		  format_l_end());
		notify(enactor, buffer);
		return 1;
	} else if (ctype != 0) {
		snprintf(buffer, sizeof(buffer),
		  "%s%s--[%sSensor Report%s]--------------------------------------------------------------%s\n%s### Type Res Bearing Range   Arcs  Heading Speed  Arcs  Name       Class flags%s\n%s--- ---- --- ------- ------- ----- ------- ------ ----- ---------------- ------%s\n",
		  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL,
		  ANSI_CYAN, ANSI_WHITE, ANSI_BLUE, ANSI_WHITE);
		for (contact = 0 ; contact < sdb[n].sensor.contacts ; ++contact)
			if (sdb[sdb[n].slist.sdb[contact]].structure.type == ctype)
				strncat(buffer, contact_line(contact), sizeof(buffer) - 1);
		strncat(buffer, tprintf("%s%s%s\n%s%s\n%s",
		  format_l_line(), format_Course(n), format_Speed(n),
		  format_Location(n), format_Velocity(n), format_l_end()), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	} else if (contact != SENSOR_FAIL) {
		snprintf(buffer, sizeof(buffer), "%s### Type Res Bearing Range   Arcs  Heading Speed  Arcs  Name       Class flags%s\n%s",
		  ANSI_CYAN, ANSI_WHITE, contact_line(contact));
		notify(enactor, buffer);
		return 1;
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sSensor Report%s]--------------------------------------------------------------%s\n%s### Type Res Bearing Range   Arcs  Heading Speed  Arcs  Name       Class flags%s\n",
		  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL,
		  ANSI_CYAN, ANSI_WHITE);
		for (ctype = 1 ; ctype < MAX_TYPE_NAME ; ++ctype) {
			first = 1;
			for (contact = 0 ; contact < sdb[n].sensor.contacts ; ++contact) {
				x = sdb[n].slist.sdb[contact];
				if (sdb[x].structure.type == ctype) {
					if (first) {
						strncat(buffer, tprintf("%s--- ---- --- ------- ------- ----- ------- ------ ----- ---------------- ------%s\n",
						  ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);
						first = 0;
					}
					strncat(buffer, contact_line(contact), sizeof(buffer) - 1);
				}
			}
		}
		strncat(buffer, tprintf("%s%s%s\n%s%s\n%s",
		  format_l_line(), format_Course(n), format_Speed(n),
		  format_Location(n), format_Velocity(n), format_l_end()), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

void contact_line_bot (int contact, char *buff, char **bp)
{
	int x = sdb[n].slist.sdb[contact];
	static char arc1[10], arc2[10];
	double level = sdb[n].slist.lev[contact] * 100.0;

	if (level < 0.0) {
		level = 0.0;
	} else if (level > 100.0)
		level = 100.0;

	strncpy(arc1, unparse_arc(sdb2arc(n, x)), sizeof(arc1) - 1);
	strncpy(arc2, unparse_arc(sdb2arc(x, n)), sizeof(arc2) - 1);

	safe_format(buff, bp, "%d|%d|#%d|%d|%.0f|%.3f|%.3f|%.1f|%.3f|%.3f|%f|%.1f %.1f %.1f|%s|%s|%s|%s|%s|%d",
	  sdb[n].slist.num[contact],
	  x,
	  sdb[x].object,
	  sdb[x].structure.type,
	  level,
	  sdb2bearing(n, x),
	  sdb2elevation(n, x),
	  sdb2range(n, x),
	  sdb[x].course.yaw_out,
	  sdb[x].course.pitch_out,
	  sdb[x].move.out,
	  sdb[x].coords.x - sdb[n].coords.xo,
	  sdb[x].coords.y - sdb[n].coords.yo,
	  sdb[x].coords.z - sdb[n].coords.zo,
	  arc1,
	  arc2,
	  (level < 25 ? "" : contact_flags(x)),
	  ((level < 50) && (sdb[x].structure.type < 3) ? "" : (sdb[x].cloak.active ? "(cloaked)" : unparse_class(x))),
	  ((level < 50) && (sdb[x].structure.type < 3) ? "" : (sdb[x].cloak.active ? "(cloaked)" : Name(sdb[x].object))),
	  sdb[x].move.empire);
}

void do_sensor_contacts_bot (char *a, dbref enactor, char *buff, char **bp)
{
	register int contact = contact2slist(n, atoi(a));
	int ctype = 0, first = 0;

	switch (a[0]) {
		case 'a': case 'A':
			ctype = 4; break;
		case 'b': case 'B':
			ctype = 2; break;
		case 'p': case 'P':
			ctype = 3; break;
		case 's': case 'S':
			switch (a[1]) {
				case 'h': case 'H':
					ctype = 1; break;
				case 't': case 'T':
					ctype = 5; break;
				default:
					ctype = 0; break;
			} break;
		default:
			ctype = 0; break;
	}

	if (error_on_console(enactor)) {
		safe_str("#-1 ERROR ON CONSOLE", buff, bp);
	} else if (!sdb[n].sensor.contacts) {
		safe_str("#-1 NO CONTACTS", buff, bp);
	} else if (ctype != 0) {
		for (contact = 0 ; contact < sdb[n].sensor.contacts ; ++contact)
		{
			if (sdb[sdb[n].slist.sdb[contact]].structure.type == ctype) {
				if (first) {
					safe_str("~", buff, bp);
				}
				contact_line_bot(contact, buff, bp);
				++first;
			}
		}
	} else if (contact != SENSOR_FAIL) {
		contact_line_bot(contact, buff, bp);
	} else {
		for (ctype = 1 ; ctype < MAX_TYPE_NAME ; ++ctype) {
			for (contact = 0 ; contact < sdb[n].sensor.contacts ; ++contact) {
				if (sdb[sdb[n].slist.sdb[contact]].structure.type == ctype) {
					if (first) {
						safe_str("~", buff, bp);
					}
					contact_line_bot(contact, buff, bp);
					++first;
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------ */

int do_iff_check (int contact)
{
	int x = contact2sdb(n, contact);
	
	if ( sdb[x].iff.frequency == sdb[n].iff.frequency ) {
		return 1;
	} else {
		return 0;
	}
}

/* ------------------------------------------------------------------------ */

int do_sensor_report (int contact, dbref enactor)
{
	char buffer[BUFFER_LEN];
	int x = contact2sdb(n, contact);
	int i;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.lrs_exist && !sdb[n].sensor.srs_exist) {
		notify_format(enactor, "%s has no sensors.", Name(sdb[n].object));
	} else if (!sdb[n].sensor.contacts) {
		notify(enactor, ansi_red("There are no sensor contacts."));
	} else if (error_on_contact(n, x, enactor)) {
		return 0;
	} else {
		i = contact2slist(n, contact);
		snprintf(buffer, sizeof(buffer), "%s%s--[%sDetailed Sensor Report%s]-----------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Type(x), sizeof(buffer) - 1);
		strncat(buffer, format_Resolution(sdb[n].slist.lev[i]), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].slist.lev[i] > 0.5 && !sdb[x].cloak.active) {
			strncat(buffer, format_Name(x), sizeof(buffer) - 1);
 			strncat(buffer, format_Cargo_Cap(x), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
			if (sdb[x].structure.type < 3 && ( sdb[x].structure.has_docking_bay || sdb[x].structure.has_landing_pad )) {
				if (sdb[x].structure.has_docking_bay)
					strncat(buffer, format_Docking_Doors(x), sizeof(buffer) - 1);
				if (sdb[x].structure.has_landing_pad)
					strncat(buffer, format_Landing_Doors(x), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
			}
		}
		if (sdb[n].slist.lev[i] > 0.25 && !sdb[x].cloak.active) {
			strncat(buffer, format_Class(x), sizeof(buffer) - 1);
			strncat(buffer, format_Displacement(x), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		strncat(buffer, format_Contact_Arc(n, x), sizeof(buffer) - 1);
		strncat(buffer, format_Contact_Shield(n, x), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Course(x), sizeof(buffer) - 1);
		strncat(buffer, format_Speed(x), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_Bearing(n, x), sizeof(buffer) - 1);
		strncat(buffer, format_Range(n, x), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Firing_Arc(n, x), sizeof(buffer) - 1);
		strncat(buffer, format_Facing_Shield(n, x), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_eng_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sEngineering Status Report%s]--------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Speed(n), sizeof(buffer) - 1);
		strncat(buffer, format_Total_Power(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].engine.warp_exist || sdb[n].engine.impulse_exist) {
			if (sdb[n].engine.warp_exist)
				strncat(buffer, format_Warp_Cruise(n), sizeof(buffer) - 1);
			if (sdb[n].engine.impulse_exist)
				strncat(buffer, format_Impulse_Cruise(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
			if (sdb[n].engine.warp_exist)
				strncat(buffer, format_Warp_Maximum(n), sizeof(buffer) - 1);
			if (sdb[n].engine.impulse_exist)
				strncat(buffer, format_Impulse_Maximum(n), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		if (sdb[n].main.exist || sdb[n].aux.exist || sdb[n].batt.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			if (sdb[n].main.exist && sdb[n].main.gw)
				strncat(buffer, format_l_Antimatter(n), sizeof(buffer) - 1);
			if (sdb[n].aux.exist && sdb[n].aux.gw)
				strncat(buffer, format_l_Deuterium(n), sizeof(buffer) - 1);
			if (sdb[n].batt.exist && sdb[n].batt.gw)
				strncat(buffer, format_l_Reserves(n), sizeof(buffer) - 1);
		}
		if (sdb[n].main.exist || sdb[n].aux.exist || sdb[n].batt.exist || sdb[n].engine.warp_exist || sdb[n].engine.impulse_exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_s_header(), sizeof(buffer) - 1);
			if (sdb[n].main.exist)
				strncat(buffer, format_s_MA_Reactor(n), sizeof(buffer) - 1);
			if (sdb[n].aux.exist)
				strncat(buffer, format_s_Fusion_Reactor(n), sizeof(buffer) - 1);
			if (sdb[n].batt.exist)
				strncat(buffer, format_s_Batteries(n), sizeof(buffer) - 1);
			if (sdb[n].engine.warp_exist)
				strncat(buffer, format_s_Warp_Drive(n), sizeof(buffer) - 1);
			if (sdb[n].engine.impulse_exist)
				strncat(buffer, format_s_Impulse_Drive(n), sizeof(buffer) - 1);
			strncat(buffer, format_s_Structure(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_helm_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sHelm Status Report%s]---------------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_l_Galactic(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_Relative(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_Destination(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Territory(n), sizeof(buffer) - 1);
		strncat(buffer, format_Quadrant(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Visibility(n), sizeof(buffer) - 1);
		strncat(buffer, format_Cochranes(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Course(n), sizeof(buffer) - 1);
		strncat(buffer, format_Helm_Power(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Speed(n), sizeof(buffer) - 1);
		strncat(buffer, format_Velocity(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].engine.warp_exist || sdb[n].engine.impulse_exist) {
			if (sdb[n].engine.warp_exist)
				strncat(buffer, format_Warp_Cruise(n), sizeof(buffer) - 1);
			if (sdb[n].engine.impulse_exist)
				strncat(buffer, format_Impulse_Cruise(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
			if (sdb[n].engine.warp_exist)
				strncat(buffer, format_Warp_Maximum(n), sizeof(buffer) - 1);
			if (sdb[n].engine.impulse_exist)
				strncat(buffer, format_Impulse_Maximum(n), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		if (sdb[n].cloak.exist || sdb[n].shield.exist) {
			if (sdb[n].shield.exist)
				strncat(buffer, format_Shield_Freq(n), sizeof(buffer) - 1);
			if (sdb[n].cloak.exist)
				strncat(buffer, format_Cloak_Freq(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		if (sdb[n].cloak.exist || sdb[n].shield.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_s_header(), sizeof(buffer) - 1);
			if (sdb[n].cloak.exist)
				strncat(buffer, format_s_Cloak(n), sizeof(buffer) - 1);
			if (sdb[n].shield.exist)
				strncat(buffer, format_s_Shield(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_beam_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].beam.banks) {
		notify_format(enactor, "%s has no beam weapons.", Name(sdb[n].object));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sTactical/Beam Status Report%s]------------------------------------------------%s%s\n",
			ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_s_header(), sizeof(buffer) - 1);
		strncat(buffer, format_s_Beam(n, n), sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_missile_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].missile.tubes) {
		notify_format(enactor, "%s has no missile weapons.", Name(sdb[n].object));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sTactical/Missile Status Report%s]---------------------------------------------%s%s\n",
			ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_s_header(), sizeof(buffer) - 1);
		strncat(buffer, format_s_Missile(n, n), sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_tactical_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else if (!GoodSDB(n)) {
		notify(enactor, ansi_red("SDB number invalid."));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sTactical Status Report%s]-----------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_LRS_Res(n), sizeof(buffer) - 1);
		strncat(buffer, format_SRS_Res(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_LRS_Sig(n), sizeof(buffer) - 1);
		strncat(buffer, format_SRS_Sig(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Visibility(n), sizeof(buffer) - 1);
		strncat(buffer, format_Tactical_Power(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].beam.exist || sdb[n].missile.exist) {
			if (sdb[n].beam.exist)
				strncat(buffer, format_Beam_Freq(n), sizeof(buffer) - 1);
			if (sdb[n].missile.exist)
				strncat(buffer, format_Missile_Freq(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_l_Beam_Cap(n), sizeof(buffer) - 1);
			strncat(buffer, format_l_Missile_Cap(n), sizeof(buffer) - 1);
		}
		if (sdb[n].sensor.lrs_exist || sdb[n].sensor.srs_exist || sdb[n].sensor.ew_exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_s_header(), sizeof(buffer) - 1);
			if (sdb[n].sensor.lrs_exist)
				strncat(buffer, format_s_LRS(n), sizeof(buffer) - 1);
			if (sdb[n].sensor.srs_exist)
				strncat(buffer, format_s_SRS(n), sizeof(buffer) - 1);
			if (sdb[n].sensor.ew_exist)
				strncat(buffer, format_s_EW(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_operations_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sOperations Status Report%s]---------------------------------------------------%s%s\n",
			ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Territory(n), sizeof(buffer) - 1);
		strncat(buffer, format_Quadrant(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Displacement(n), sizeof(buffer) - 1);
		strncat(buffer, format_Operations_Power(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Cargo_Cap(n), sizeof(buffer) - 1);
		strncat(buffer, format_Cargo_Carried(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].trans.exist || sdb[n].tract.exist) {
			if (sdb[n].trans.exist)
				strncat(buffer, format_Trans_Freq(n), sizeof(buffer) - 1);
			if (sdb[n].tract.exist)
				strncat(buffer, format_Tract_Freq(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		strncat(buffer, format_Iff_Freq(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].structure.type < 3 && (sdb[n].structure.has_docking_bay || sdb[n].structure.has_landing_pad)) {
			if (sdb[n].structure.has_docking_bay)
				strncat(buffer, format_Docking_Doors(n), sizeof(buffer) - 1);
			if (sdb[n].structure.has_landing_pad)
				strncat(buffer, format_Landing_Doors(n), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
		}
		if (sdb[n].main.exist || sdb[n].aux.exist || sdb[n].batt.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			if (sdb[n].main.exist && sdb[n].main.gw)
				strncat(buffer, format_l_Antimatter(n), sizeof(buffer) - 1);
			if (sdb[n].aux.exist && sdb[n].aux.gw)
				strncat(buffer, format_l_Deuterium(n), sizeof(buffer) - 1);
			if (sdb[n].batt.exist && sdb[n].batt.gw)
				strncat(buffer, format_l_Reserves(n), sizeof(buffer) - 1);
		}
		if (sdb[n].structure.has_docking_bay) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_l_Docked(n), sizeof(buffer) - 1);
		}
		if (sdb[n].structure.has_landing_pad) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_l_Landed(n), sizeof(buffer) - 1);
		}
		if (sdb[n].trans.exist || sdb[n].tract.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_s_header(), sizeof(buffer) - 1);
			if (sdb[n].trans.exist)
				strncat(buffer, format_s_Transporters(n), sizeof(buffer) - 1);
			if (sdb[n].tract.exist)
				strncat(buffer, format_s_Tractors(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);
		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_allocation_status (dbref enactor)
{
	static char buffer[BUFFER_LEN * 2];
	register int i;

	if (error_on_console(enactor)) {
		return 0;
	} else {

	snprintf(buffer, sizeof(buffer), "%s%s--[%sTotal Allocation Report%s]----------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL);
	strncat(buffer, tprintf("%sAllocation             EPS Power\n",
	  ANSI_CYAN), sizeof(buffer) - 1);
	strncat(buffer, tprintf("%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);
	strncat(buffer, tprintf("%s%-22.22s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Total EPS", ANSI_WHITE,
	  unparse_power(sdb[n].power.total),
	  unparse_percent(1.0)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Total Helm", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.helm * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.helm)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Movement", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.movement * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.movement),
	  ansi_rainbow_scale(sdb[n].alloc.movement, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Shields", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.shields * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.shields)), sizeof(buffer) - 1);
	for (i = 0; i < MAX_SHIELD_NAME; ++i)
		strncat(buffer, tprintf("      %s%-16.16s%s %10.10s: %4.4s %s\n",
		  ANSI_CYAN, unparse_shield(i), ANSI_WHITE,
		  unparse_power(sdb[n].alloc.shield[i] * sdb[n].power.total),
		  unparse_percent(sdb[n].alloc.shield[i]),
		  ansi_rainbow_scale(sdb[n].alloc.shield[i], 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, cloak_name[sdb[n].cloak.exist], ANSI_WHITE,
	  unparse_power(sdb[n].alloc.cloak * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.cloak),
	  ansi_rainbow_scale(sdb[n].alloc.cloak, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Total Tactical", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.tactical * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.tactical)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Beam Weap", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.beams * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.beams),
	  ansi_rainbow_scale(sdb[n].alloc.beams, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Missile Weap", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.missiles * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.missiles),
	  ansi_rainbow_scale(sdb[n].alloc.missiles, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "EW Systems", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.sensors * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.sensors)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("      %s%-16.16s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "ECM", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.ecm * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.ecm),
	  ansi_rainbow_scale(sdb[n].alloc.ecm, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("      %s%-16.16s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "ECCM", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.eccm * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.eccm),
	  ansi_rainbow_scale(sdb[n].alloc.eccm, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Total Operations", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.operations * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.operations)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Transporters", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.transporters * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.transporters),
	  ansi_rainbow_scale(sdb[n].alloc.transporters, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Tractors", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.tractors * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.tractors),
	  ansi_rainbow_scale(sdb[n].alloc.tractors, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("    %s%-18.18s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Miscellaneous", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.miscellaneous * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.miscellaneous),
	  ansi_rainbow_scale(sdb[n].alloc.miscellaneous, 35)), sizeof(buffer) - 1);
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_damage_status (int type, char *name, dbref enactor)
{
	static char buffer[BUFFER_LEN * 2];
	register int i;
	int x;

	if (!GoodSDB(n)) {
		notify(enactor, ansi_red("SDB number invalid."));
		return 0;
	} else if (!SpaceObj(sdb[n].object) || !GoodObject(sdb[n].object)) {
		notify(enactor, ansi_red("Space object invalid."));
		return 0;
	} else if (sdb[n].structure.type == 0) {
		notify(enactor, ansi_red("Space object not loaded."));
		return 0;
	} else if (sdb[n].status.crippled == 2) {
		notify(enactor, ansi_red("Space object destroyed."));
		return 0;
	} else if (!sdb[n].status.active) {
		notify_format(enactor, "%s systems are inactive.", Name(sdb[n].object));
		return 0;
	}

	if (type) {
		x = name2sdb(name);
		if (x == SENSOR_FAIL) {
			notify(enactor, ansi_red("That is not a valid subject."));
			return 0;
		} else if (!GoodSDB(x)) {
			notify(enactor, ansi_red("That is not a valid subject."));
			return 0;
		} else if (sdb[x].location != n) {
			notify(enactor, ansi_red("That is not a valid subject."));
			return 0;
		} else if (sdb[x].structure.type == 0) {
			notify(enactor, ansi_red("That is not a valid subject."));
			write_spacelog(enactor, x, "BUG:Subject has bad TYPE");
			return 0;
		} else if (!sdb[x].status.connected) {
			notify_format(enactor, "%s is not connected.", Name(sdb[x].object));
			return 0;
		} else if (!SpaceObj(sdb[x].object) || !GoodObject(sdb[x].object)) {
			notify(enactor, ansi_red("That is not a valid subject."));
			write_spacelog(enactor, x, "BUG:Subject has bad flags");
			return 0;
		}
	} else
		x = n;

		snprintf(buffer, sizeof(buffer), "%s%s--[%sDamage Control Status Report%s]-----------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		if (type) {
			strncat(buffer, format_Name(x), sizeof(buffer) - 1);
			strncat(buffer, format_Class(x), sizeof(buffer) - 1);
			strncat(buffer, "\n", sizeof(buffer) - 1);
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		}
		strncat(buffer, tprintf("%sSystem Name         Integ  Condition%s\n%s------------------ ------- ----------------------------------------------------%s\n",
		  ANSI_CYAN, ANSI_WHITE, ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);
		if (sdb[x].structure.superstructure != sdb[x].structure.max_structure)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[0], ANSI_WHITE,
			  unparse_percent(sdb[x].structure.superstructure / sdb[x].structure.max_structure),
			  unparse_damage(sdb[x].structure.superstructure / sdb[x].structure.max_structure),
			  (sdb[x].structure.max_structure - sdb[x].structure.superstructure) * repair_mult[0]), sizeof(buffer) - 1);
		if (sdb[x].aux.exist && sdb[x].aux.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[1], ANSI_WHITE,
			  unparse_percent(sdb[x].aux.damage),
			  unparse_damage(sdb[x].aux.damage),
			  (1.0 - sdb[x].aux.damage) * repair_mult[1] * sdb[x].aux.gw * 100.0), sizeof(buffer) - 1);
		if (sdb[x].batt.exist && sdb[x].batt.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[2], ANSI_WHITE,
			  unparse_percent(sdb[x].batt.damage),
			  unparse_damage(sdb[x].batt.damage),
			  (1.0 - sdb[x].batt.damage) * repair_mult[2] * sdb[x].batt.gw * 100.0), sizeof(buffer) - 1);
		if (sdb[x].beam.exist)
			for (i = 0 ; i < sdb[x].beam.banks ; ++i)
				if (sdb[x].blist.damage[i] < 1.0)
					strncat(buffer, tprintf("%s%-15.15s %2d%s %7.7s %-15.15s %10.3f\n",
					  ANSI_CYAN, system_name[3], i + 1, ANSI_WHITE,
					  unparse_percent(sdb[x].blist.damage[i]),
					  unparse_damage(sdb[x].blist.damage[i]) ,
			  		  (1.0 - sdb[x].blist.damage[i]) * repair_mult[3] * sdb[x].blist.cost[i] * 100.0), sizeof(buffer) - 1);
		if (sdb[x].cloak.exist && sdb[x].cloak.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[4], ANSI_WHITE,
			  unparse_percent(sdb[x].cloak.damage),
			  unparse_damage(sdb[x].cloak.damage),
			  (1.0 - sdb[x].cloak.damage) * repair_mult[4] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].sensor.ew_exist && sdb[x].sensor.ew_damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[5], ANSI_WHITE,
			  unparse_percent(sdb[x].sensor.ew_damage),
			  unparse_damage(sdb[x].sensor.ew_damage),
			  (1.0 - sdb[x].sensor.ew_damage) * repair_mult[5] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].engine.impulse_exist && sdb[x].engine.impulse_damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[6], ANSI_WHITE,
			  unparse_percent(sdb[x].engine.impulse_damage),
			  unparse_damage(sdb[x].engine.impulse_damage),
			  (1.0 - sdb[x].engine.impulse_damage) * repair_mult[6] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].sensor.lrs_exist && sdb[x].sensor.lrs_damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[7], ANSI_WHITE,
			  unparse_percent(sdb[x].sensor.lrs_damage),
			  unparse_damage(sdb[x].sensor.lrs_damage),
			  (1.0 - sdb[x].sensor.lrs_damage) * repair_mult[7] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].main.exist && sdb[x].main.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[8], ANSI_WHITE,
			  unparse_percent(sdb[x].main.damage),
			  unparse_damage(sdb[x].main.damage),
			  (1.0 - sdb[x].main.damage) * repair_mult[8] * sdb[x].main.gw * 100.0), sizeof(buffer) - 1);
		if (sdb[x].missile.exist)
			for (i = 0 ; i < sdb[x].missile.tubes ; ++i)
				if (sdb[x].mlist.damage[i] < 1.0)
					strncat(buffer, tprintf("%s%-15.15s %2d%s %7.7s %-15.15s %10.3f\n",
					  ANSI_CYAN, system_name[9], i + 1, ANSI_WHITE,
					  unparse_percent(sdb[x].mlist.damage[i]),
					  unparse_damage(sdb[x].mlist.damage[i]),
					  (1.0 - sdb[x].mlist.damage[i]) * repair_mult[9] * sdb[x].mlist.warhead[i] * 100.0), sizeof(buffer) - 1);
		if (sdb[x].shield.exist)
			for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
				if (sdb[x].shield.damage[i] < 1.0)
					strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
					  ANSI_CYAN, unparse_shield(i), ANSI_WHITE,
					  unparse_percent(sdb[x].shield.damage[i]),
					  unparse_damage(sdb[x].shield.damage[i]),
			  		  (1.0 - sdb[x].shield.damage[i]) * repair_mult[10] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].sensor.srs_exist && sdb[x].sensor.srs_damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[11], ANSI_WHITE,
			  unparse_percent(sdb[x].sensor.srs_damage),
			  unparse_damage(sdb[x].sensor.srs_damage),
			  (1.0 - sdb[x].sensor.srs_damage) * repair_mult[11] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].tract.exist && sdb[x].tract.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[12], ANSI_WHITE,
			  unparse_percent(sdb[x].tract.damage),
			  unparse_damage(sdb[x].tract.damage),
			  (1.0 - sdb[x].tract.damage) * repair_mult[12] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].trans.exist && sdb[x].trans.damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[13], ANSI_WHITE,
			  unparse_percent(sdb[x].trans.damage),
			  unparse_damage(sdb[x].trans.damage),
			  (1.0 - sdb[x].trans.damage) * repair_mult[13] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		if (sdb[x].engine.warp_exist && sdb[x].engine.warp_damage < 1.0)
			strncat(buffer, tprintf("%s%-18.18s%s %7.7s %-15.15s %10.3f\n",
			  ANSI_CYAN, system_name[14], ANSI_WHITE,
			  unparse_percent(sdb[x].engine.warp_damage),
			  unparse_damage(sdb[x].engine.warp_damage),
			  (1.0 - sdb[x].engine.warp_damage) * repair_mult[14] * (1.0 + (sdb[x].structure.max_structure / 10.0)) * 100.0), sizeof(buffer) - 1);
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, tprintf("%s%s%s: %.3f (%d) units: %s\n",
		  ANSI_CYAN, "Repair Capacity", ANSI_WHITE,
		  sdb[n].structure.repair, sdb[n].structure.max_repair,
		  unparse_percent_3(sdb[n].structure.repair / sdb[n].structure.max_repair)), sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

	notify(enactor, buffer);
	sdb[n].status.time = sdb[n].move.time;
	return 1;
}

/* ------------------------------------------------------------------------ */

int do_science_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else if (!GoodSDB(n)) {
		notify(enactor, ansi_red("SDB number invalid."));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sScience Status Report%s]------------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_l_Galactic(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_Relative(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_Destination(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Territory(n), sizeof(buffer) - 1);
		strncat(buffer, format_Quadrant(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Location(n), sizeof(buffer) - 1);
		strncat(buffer, format_Visibility(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_LRS_Res(n), sizeof(buffer) - 1);
		strncat(buffer, format_SRS_Res(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_LRS_Sig(n), sizeof(buffer) - 1);
		strncat(buffer, format_SRS_Sig(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].sensor.lrs_exist || sdb[n].sensor.srs_exist || sdb[n].sensor.ew_exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			strncat(buffer, format_s_header(), sizeof(buffer) - 1);
			if (sdb[n].sensor.lrs_exist)
				strncat(buffer, format_s_LRS(n), sizeof(buffer) - 1);
			if (sdb[n].sensor.srs_exist)
				strncat(buffer, format_s_SRS(n), sizeof(buffer) - 1);
			if (sdb[n].sensor.ew_exist)
				strncat(buffer, format_s_EW(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_fighter_status (dbref enactor)
{
	static char buffer[BUFFER_LEN];

	if (error_on_console(enactor)) {
		return 0;
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sCondensed Status Report%s]----------------------------------------------------%s%s\n",
			ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Name(n), sizeof(buffer) - 1);
		strncat(buffer, format_Class(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Visibility(n), sizeof(buffer) - 1);
		strncat(buffer, format_Cochranes(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Course(n), sizeof(buffer) - 1);
		strncat(buffer, format_Total_Power(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Speed(n), sizeof(buffer) - 1);
		strncat(buffer, format_Velocity(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		if (sdb[n].main.exist || sdb[n].aux.exist || sdb[n].batt.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
			if (sdb[n].main.exist && sdb[n].main.gw)
				strncat(buffer, format_l_Antimatter(n), sizeof(buffer) - 1);
			if (sdb[n].aux.exist && sdb[n].aux.gw)
				strncat(buffer, format_l_Deuterium(n), sizeof(buffer) - 1);
			if (sdb[n].batt.exist && sdb[n].batt.gw)
				strncat(buffer, format_l_Reserves(n), sizeof(buffer) - 1);
		}
		if (sdb[n].beam.exist || sdb[n].missile.exist) {
			strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		if (sdb[n].beam.exist)
			strncat(buffer, format_l_Beam_Cap(n), sizeof(buffer) - 1);
		if (sdb[n].missile.exist)
			strncat(buffer, format_l_Missile_Cap(n), sizeof(buffer) - 1);
		}
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_s_header(), sizeof(buffer) - 1);
		if (sdb[n].shield.exist)
			strncat(buffer, format_s_Shield(n), sizeof(buffer) - 1);
		strncat(buffer, format_s_Structure(n), sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_scanner_report (int a, char *s, dbref enactor)
{
	static char buffer[BUFFER_LEN], desc[BUFFER_LEN];
	char *dp = desc;
	const char *sp;
        char *sbuf;
	int x, f, y;
	ATTR *d;

	switch (s[0]) {
		case 'b': case 'B': f = 1; break; /* beam */
		case 'd': case 'D': f = 2; break; /* defense */
		case 'e': case 'E': f = 3; break; /* engines */
		case 'f': case 'F': f = 4; break; /* fuel */
		case 'h': case 'H': f = 5; break; /* hull */
		case 'm': case 'M': f = 6; break; /* missile */
		case 'o': case 'O': f = 7; break; /* other */
		case 'p': case 'P': f = 8; break; /* power */
		case 'v': case 'V': f = 9; break; /* visual */
		default: f = 0; break;
	}

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.srs_exist) {
		notify_format(enactor, "%s has no short-range sensors.", Name(sdb[n].object));
	} else if (!sdb[n].sensor.srs_active) {
		notify(enactor, ansi_red("Short-range sensors are inactive."));
	} else if (!sdb[n].sensor.contacts) {
		notify(enactor, ansi_red("There are no sensor contacts."));
	} else if (!f) {
		notify(enactor, ansi_red("That is not a valid scanning mode."));
	} else if (error_on_contact(n, (x = contact2sdb(n, a)), enactor)) {
		return 0;
	} else if (sdb[x].cloak.active) {
		notify(enactor, ansi_red("Contact is cloaked."));
	} else if (sdb[n].slist.lev[contact2slist(n, a)] < 0.5) {
		notify(enactor, ansi_red("Insufficient resolution of contact."));
	} else {
		snprintf(buffer, sizeof(buffer), "%s%s--[%sScanner Report%s]-------------------------------------------------------------%s%s\n",
		  ANSI_BLUE, ANSI_HILITE, ANSI_YELLOW, ANSI_BLUE, ANSI_WHITE, ANSI_NORMAL);
		strncat(buffer, format_Name(x), sizeof(buffer) - 1);
		strncat(buffer, format_Class(x), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		switch (f) {
			case 1:
				if (sdb[x].beam.banks) {
					strncat(buffer, format_l_Beam_Cap(x), sizeof(buffer) - 1);
					strncat(buffer, format_l_line(), sizeof(buffer) - 1);
					strncat(buffer, format_s_header(), sizeof(buffer) - 1);
					strncat(buffer, format_s_Beam(x, n), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No beam weapons detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 2:
				if (sdb[x].cloak.exist || sdb[x].shield.exist) {
					strncat(buffer, format_s_header(), sizeof(buffer) - 1);
					if (sdb[x].cloak.exist)
						strncat(buffer, format_s_Cloak(x), sizeof(buffer) - 1);
					if (sdb[x].shield.exist)
						strncat(buffer, format_s_Shield(x), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No defensive systems detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 3:
				if (sdb[x].main.exist || sdb[x].aux.exist || sdb[x].batt.exist || sdb[x].engine.warp_exist || sdb[x].engine.impulse_exist) {
					strncat(buffer, format_s_header(), sizeof(buffer) - 1);
					if (sdb[x].main.exist)
						strncat(buffer, format_s_MA_Reactor(x), sizeof(buffer) - 1);
					if (sdb[x].aux.exist)
						strncat(buffer, format_s_Fusion_Reactor(x), sizeof(buffer) - 1);
					if (sdb[x].batt.exist)
						strncat(buffer, format_s_Batteries(x), sizeof(buffer) - 1);
					if (sdb[x].engine.warp_exist)
						strncat(buffer, format_s_Warp_Drive(x), sizeof(buffer) - 1);
					if (sdb[x].engine.impulse_exist)
						strncat(buffer, format_s_Impulse_Drive(x), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No engine systems detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 4:
				if (sdb[x].main.exist || sdb[x].aux.exist || sdb[x].batt.exist) {
					strncat(buffer, format_l_line(), sizeof(buffer) - 1);
					if (sdb[x].main.exist && sdb[x].main.gw)
						strncat(buffer, format_l_Antimatter(x), sizeof(buffer) - 1);
					if (sdb[x].aux.exist && sdb[x].aux.gw)
						strncat(buffer, format_l_Deuterium(x), sizeof(buffer) - 1);
					if (sdb[x].batt.exist && sdb[x].batt.gw)
						strncat(buffer, format_l_Reserves(x), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No fuel systems detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 5:
				strncat(buffer, format_Cargo_Carried(x), sizeof(buffer) - 1);
				strncat(buffer, format_Cargo_Cap(x), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
				if (sdb[x].structure.has_docking_bay) {
					strncat(buffer, format_l_line(), sizeof(buffer) - 1);
					strncat(buffer, format_l_Docked(x), sizeof(buffer) - 1);
				}
				if (sdb[x].structure.has_landing_pad) {
					strncat(buffer, format_l_line(), sizeof(buffer) - 1);
					strncat(buffer, format_l_Landed(x), sizeof(buffer) - 1);
				}
				strncat(buffer, format_l_line(), sizeof(buffer) - 1);
				strncat(buffer, format_s_header(), sizeof(buffer) - 1);
				strncat(buffer, format_s_Structure(x), sizeof(buffer) - 1);
				break;
			case 6:
				if (sdb[x].missile.tubes) {
					strncat(buffer, format_l_Missile_Cap(x), sizeof(buffer) - 1);
					strncat(buffer, format_l_line(), sizeof(buffer) - 1);
					strncat(buffer, format_s_header(), sizeof(buffer) - 1);
					strncat(buffer, format_s_Missile(x, n), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No missile weapons detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 7:
				if (sdb[x].trans.exist || sdb[x].tract.exist || sdb[x].sensor.lrs_exist
				  || sdb[x].sensor.srs_exist || sdb[x].sensor.ew_exist) {
					strncat(buffer, format_s_header(), sizeof(buffer) - 1);
					if (sdb[x].sensor.lrs_exist)
						strncat(buffer, format_s_LRS(x), sizeof(buffer) - 1);
					if (sdb[x].sensor.srs_exist)
						strncat(buffer, format_s_SRS(x), sizeof(buffer) - 1);
					if (sdb[x].sensor.ew_exist)
						strncat(buffer, format_s_EW(x), sizeof(buffer) - 1);
					if (sdb[x].trans.exist)
						strncat(buffer, format_s_Transporters(x), sizeof(buffer) - 1);
					if (sdb[x].tract.exist)
						strncat(buffer, format_s_Tractors(x), sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No other systems detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
			case 8:
				strncat(buffer, format_Total_Power(x), sizeof(buffer) - 1);
				strncat(buffer, format_Helm_Power(x), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
				strncat(buffer, format_Tactical_Power(x), sizeof(buffer) - 1);
				strncat(buffer, format_Operations_Power(x), sizeof(buffer) - 1);
				strncat(buffer, "\n", sizeof(buffer) - 1);
				break;
			case 9:
				d = atr_get(sdb[x].object, "DESCRIBE");
				if (d) {
					sp = sbuf = safe_atr_value(d, "space.describe");
					process_expression(desc, &dp, &sp, sdb[x].object, enactor, enactor,
		    			 PE_DEFAULT, PT_DEFAULT, NULL);
		    		mush_free(sbuf, "space.describe");
		    		*dp = '\0';
					strncat(buffer, desc, sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} else {
					strncat(buffer, ansi_red("No visible features detected."), sizeof(buffer) - 1);
					strncat(buffer, "\n", sizeof(buffer) - 1);
				} break;
		}
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		if (f != 5 && f != 9) {
			y = sdb2contact(x, n);
			if (y != SENSOR_FAIL)
				console_message(x, "science",
					ansi_warn(tprintf("Scan from %s detected", unparse_identity(x, n))));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

/* do_nebula_report is from here */

/* ------------------------------------------------------------------------ */

/* do_border_report is from here */

/* ------------------------------------------------------------------------ */

char *planet_eta (int x, double r)
{
	static char buffer[20];
	double v = fabs(sdb[n].move.in), s;

	if (v < 1.0) {
		v *= LIGHTSPEED;
	} else {
		v = LIGHTSPEED * pow(v, 3.333333) * 0.5 *
		  (xyz2cochranes(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z) +
		  xyz2cochranes(sdb[x].coords.x, sdb[x].coords.y, sdb[x].coords.z));
	}

	if (v == 0) {
		strncpy(buffer, "n/a", sizeof(buffer) - 1);
	} else {
		s = r / v;
		if (s < 60.0) {
			snprintf(buffer, sizeof(buffer), "%6.0fs", s);
		} else if (s < 3600.0) {
			snprintf(buffer, sizeof(buffer), "%6.0fm", s / 60.0);
		} else if (s < 86400.0) {
			snprintf(buffer, sizeof(buffer), "%6.0fh", s / 3600.0);
		} else if (s < 31536000.0) {
			snprintf(buffer, sizeof(buffer), "%6.0fd", s / 86400.0);
		} else
			strncpy(buffer, "n/a", sizeof(buffer) - 1);
	}

	return (buffer);
}

int do_planet_report (const char *s, int t, dbref enactor)
{
	static char buffer[BUFFER_LEN];

	int x, c = 0, max = 20, pln[20], i, j;
	double r = 0.0, rng[20];

	for (i = 0; i < max; ++i) {
		pln[i] = 0;
		rng[i] = 0.0;
	}

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.lrs_exist) {
		notify_format(enactor, "%s has no long-range sensors.", Name(sdb[n].object));
	} else if (!sdb[n].sensor.lrs_active) {
		notify(enactor, ansi_red("Long-range sensors are inactive."));
	} else {
		if (t) {
			for (x = MIN_SPACE_OBJECTS ; x <= max_space_objects ; ++x) {
				if (c < max) {
					if (sdb[n].space == sdb[x].space &&
					  sdb[x].structure.type == 5 &&
					  n != x &&
					  SpaceObj(sdb[x].object) &&
					  GoodObject(sdb[x].object)) {
						if (local_wild_match(s, Name(sdb[x].object), NULL)) {
							r = sdb2range(n, x);
							pln[c] = x;
							rng[c] = r;
							++c;
						}
					}
				}
			}
		} else {
			for (x = MIN_SPACE_OBJECTS ; x <= max_space_objects ; ++x) {
				if (sdb[n].space == sdb[x].space &&
				  sdb[x].structure.type == 5 &&
				  n != x &&
			  	  SpaceObj(sdb[x].object) &&
				  GoodObject(sdb[x].object)) {
				  	r = sdb2range(n, x);
					if (r / PARSEC < 1000.0) {
						for (i = 0; i < max - 1; ++i) {
							if (!pln[i]) {
								pln[i] = x;
								rng[i] = r;
								++c;
								break;
							} else if (r < rng[i]) {
								for (j = max - 1; j > i; --j) {
									if (pln[j - 1]) {
										pln[j] = pln[j - 1];
										rng[j] = rng[j - 1];
									}
								}
								pln[i] = x;
								rng[i] = r;
								break;
							}
						}
					}
				}
			}
		}

		if (c) {
			snprintf(buffer, sizeof(buffer), "%s%s--[%sStar-System Report%s]---------------------------------------------------------%s\n%sStar               T   ETA     Bearing Range   Coordinates%s\n%s-------------------- - ------- ------- ------- --------------------------------%s\n",
			  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL, ANSI_CYAN, ANSI_WHITE, ANSI_BLUE, ANSI_WHITE);
			for (i = 0; i < max; ++i) {
				if (pln[i])
					strncat(buffer, tprintf("%s%-20.20s%s %2.1s %7.7s %3.0f %-3.0f %7.7s %10.3f %10.3f %10.3f\n",
					  ANSI_HILITE, Name(sdb[pln[i]].object), ANSI_NORMAL,
					  unparse_class(pln[i]),
					  planet_eta(pln[i], rng[i]),
					  sdb2bearing(n, pln[i]),
					  sdb2elevation(n, pln[i]),
					  unparse_range(rng[i]),
					  (sdb[pln[i]].coords.x - sdb[n].coords.xo) / PARSEC,
					  (sdb[pln[i]].coords.y - sdb[n].coords.yo) / PARSEC,
					  (sdb[pln[i]].coords.z - sdb[n].coords.zo) / PARSEC), sizeof(buffer) - 1);
			}
			strncat(buffer, format_l_end(), sizeof(buffer) - 1);

			notify(enactor, buffer);
		} else
			notify(enactor, ansi_red("No star-systems found."));

		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

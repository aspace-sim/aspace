/* space_report.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void report_eng_power (void)
{
	static char buffer[BUFFER_LEN];

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sEngineering Allocation Report%s : :                                           : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL, ANSI_CYAN, ANSI_BLUE, ANSI_WHITE);
	strncat(buffer, tprintf("%s%-22.22s%s %10.10s: %4.4s\n",
	  ANSI_CYAN, "Total EPS", ANSI_WHITE,
	  unparse_power(sdb[n].power.total),
	  unparse_percent(1.0)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Total Helm", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.helm * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.helm),
	  ansi_rainbow_scale(sdb[n].alloc.helm, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Total Tactical", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.tactical * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.tactical),
	  ansi_rainbow_scale(sdb[n].alloc.tactical, 35)), sizeof(buffer) - 1);
	strncat(buffer, tprintf("  %s%-20.20s%s %10.10s: %4.4s %s\n",
	  ANSI_CYAN, "Total Operations", ANSI_WHITE,
	  unparse_power(sdb[n].alloc.operations * sdb[n].power.total),
	  unparse_percent(sdb[n].alloc.operations),
	  ansi_rainbow_scale(sdb[n].alloc.operations, 35)), sizeof(buffer) - 1);
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);
	do_console_notify(n, console_engineering, 0, 0, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

void report_helm_power (void)
{
	static char buffer[BUFFER_LEN];
	register int i;

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sHelm/Navigation Allocation Report%s : :                                       : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL, ANSI_CYAN, ANSI_BLUE, ANSI_WHITE);
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
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);
	do_console_notify(n, console_helm, console_engineering, 0, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

void report_tact_power (void)
{
	static char buffer[BUFFER_LEN];

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sTactical/Weapon Allocation Report%s : :                                       : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL, ANSI_CYAN, ANSI_BLUE, ANSI_WHITE);
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
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);
	do_console_notify(n, console_tactical, console_science, console_engineering, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

void report_ops_power (void)
{
	static char buffer[BUFFER_LEN];

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sOperations Allocation Report%s : :                                            : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL, ANSI_CYAN, ANSI_BLUE, ANSI_WHITE);
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
	do_console_notify(n, console_operation, console_damage, console_engineering, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

void report_shield_power (void)
{
	static char buffer[BUFFER_LEN];
	register int i;

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sShield Allocation Report%s : :                                                : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL, ANSI_CYAN, ANSI_BLUE, ANSI_WHITE);
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
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);
	do_console_notify(n, console_helm, console_engineering, 0, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

void report_sensor_power (void)
{
	static char buffer[BUFFER_LEN];

	snprintf(buffer, sizeof(buffer),
	  "%s%s: %sEW Systems Allocation Report%s : :                                            : %s\n%sAllocation             EPS Power\n%s---------------------- --------------------------------------------------------%s\n",
	  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE,
	  ANSI_NORMAL ,ANSI_CYAN ,ANSI_BLUE, ANSI_WHITE);
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
	strncat(buffer, format_l_end(), sizeof(buffer) - 1);
	do_console_notify(n, console_tactical, console_science, console_engineering, buffer);

	return;
}

/* ------------------------------------------------------------------------ */

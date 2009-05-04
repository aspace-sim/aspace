/* space_misc.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

extern time_t mudtime;

/* ------------------------------------------------------------------------ */

void damage_structure (int x, double damage)
{
	register int i;
	double s = sdb[x].structure.superstructure;

	if (sdb[x].structure.superstructure == -sdb[x].structure.max_structure)
		return;
	sdb[x].structure.superstructure -= damage;
	if (sdb[x].structure.superstructure < -sdb[x].structure.max_structure)
		sdb[x].structure.superstructure = -sdb[x].structure.max_structure;
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[0], ANSI_WHITE,
	  unparse_percent(sdb[x].structure.superstructure / sdb[x].structure.max_structure),
	  unparse_damage(sdb[x].structure.superstructure / sdb[x].structure.max_structure))));
	if ((sdb[x].structure.superstructure <= -sdb[x].structure.max_structure) &&
	  (s > -sdb[x].structure.max_structure)) {
		/* Mordak Aspace v1.0.0p1 - Removed Silly KayBye! */
		do_ship_notify(x, tprintf("%s%s%s%s explodes into white hot vapor.%s%s",
		  ANSI_HILITE, ANSI_INVERSE, ANSI_RED, Name(sdb[x].object), ANSI_WHITE, ANSI_NORMAL));
		/* End Aspace v1.0.0p1*/
		do_space_notify_one(x, console_helm, console_tactical, console_science,
		  "has been destroyed");
		write_spacelog(sdb[x].object, sdb[x].object, tprintf("LOG: Destroyed, Shields %.6f GHz",
		  sdb[x].shield.freq));
		sdb[x].space = -1;
		sdb[x].status.active = 0;
		sdb[x].status.crippled = 2;
		for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
			if (sdb[i].location == x)
				if (sdb[i].structure.type) {
					do_ship_notify(i, tprintf("%s%s%s%s explodes into white hot vapor. Goodbye!%s%s",
					  ANSI_HILITE, ANSI_INVERSE, ANSI_RED, Name(sdb[i].object), ANSI_WHITE, ANSI_NORMAL));
					write_spacelog(sdb[x].object, sdb[x].object, tprintf("LOG: Destroyed, Shields %.6f GHz",
					  sdb[x].shield.freq));
					sdb[i].space = -1;
					sdb[i].status.active = 0;
					sdb[i].status.crippled = 2;
				}
	} else if ((sdb[x].structure.superstructure <= 0.0) && (s > 0.0)) {
		do_all_console_notify(x, ansi_warn("Excessive damage. All systems shutting down"));
		do_ship_notify(x, tprintf("%s experiences total systems failure.",Name(sdb[x].object)));
		sdb[x].status.crippled = 1;
		do_space_notify_one(x, console_helm, console_tactical, console_science,
			"has been disabled");
		write_spacelog(sdb[x].object, sdb[x].object, tprintf("LOG: Disabled, Shields %.6f GHz",
		  sdb[x].shield.freq));
	}
	if (((sdb[x].structure.superstructure <= 0.0) && (s > 0.0)) ||
	  ((sdb[x].structure.superstructure <= -sdb[x].structure.max_structure) &&
	  (s > -sdb[x].structure.max_structure))) {
	  	if (sdb[x].main.damage > 0.0)
			sdb[x].main.in = 0.0;
	  	if (sdb[x].aux.damage > 0.0)
			sdb[x].aux.in = 0.0;
		for (i = 0; i < MAX_SHIELD_NAME; ++i)
			sdb[x].shield.active[i] = 0;
		sdb[x].beam.in = 0.0;
		sdb[x].beam.out = 0.0;
		for (i = 0; i < sdb[x].beam.banks; ++i) {
			sdb[x].blist.lock[i] = 0;
			sdb[x].blist.active[i] = 0;
		}
		sdb[x].missile.in = 0.0;
		sdb[x].missile.out = 0.0;
		for (i = 0; i < sdb[x].missile.tubes; ++i) {
			sdb[x].mlist.lock[i] = 0;
			sdb[x].mlist.active[i] = 0;
		}
		sdb[x].batt.in = 0.0;
		sdb[x].batt.out = 0.0;
		sdb[x].move.in = 0.0;
		sdb[x].move.out = 0.0;
		sdb[x].move.v = 0.0;
		sdb[x].engine.warp_max = 0.0;
		sdb[x].engine.impulse_max = 0.0;
		sdb[x].power.batt = 0.0;
		sdb[x].sensor.lrs_active = 0;
		sdb[x].sensor.srs_active = 0;
		sdb[x].sensor.ew_active = 0;
		sdb[x].cloak.active = 0;
		sdb[x].trans.active = 0;
		sdb[x].trans.d_lock = 0;
		sdb[x].trans.s_lock = 0;
		sdb[x].tract.active = 0;
		sdb[x].tract.lock = 0;
		if (GoodSDB(sdb[x].status.tractoring)) {
			sdb[sdb[x].status.tractoring].status.tractored = 0;
			sdb[x].status.tractoring = 0;
		}
		up_cochranes();
		up_empire();
		up_quadrant();
		up_vectors();
		up_resolution();
		up_signature(x);
		up_visibility();
		debug_space(x);
	}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_aux (int x, double damage)
{
	if (!sdb[x].aux.exist || sdb[x].aux.damage == -1.0)
		return;
	if (sdb[x].power.aux != 0.0
	  && sdb[x].aux.damage > 0.0
	  && (sdb[x].aux.damage - damage / sdb[x].aux.gw <= 0.0)) {
		alert_aux_overload(x);
	}
	sdb[x].aux.damage -= damage / sdb[x].aux.gw;
	if (sdb[x].aux.damage < -1.0) {
		sdb[x].aux.damage = -1.0;
		if (sdb[x].power.aux != 0.0) {
			do_all_console_notify(x, tprintf(ansi_warn("%s core breach."), system_name[1]));
			damage_structure(x, sdb[x].power.aux * (get_random_long(0,10) + 1.0));
		}
	}
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[1], ANSI_WHITE,
	  unparse_percent(sdb[x].aux.damage),
	  unparse_damage(sdb[x].aux.damage))));
	return;
}

/* ------------------------------------------------------------------------ */

void damage_batt (int x, double damage)
{
	if (!sdb[x].batt.exist || sdb[x].batt.damage == -1.0)
		return;
	sdb[x].batt.damage -= damage / sdb[x].batt.gw;
	if (sdb[x].batt.damage < -1.0)
		sdb[x].batt.damage = -1.0;
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[2], ANSI_WHITE,
	  unparse_percent(sdb[x].batt.damage),
	  unparse_damage(sdb[x].batt.damage))));
	if (sdb[x].batt.damage <= 0.0) {
		sdb[x].batt.in = 0.0;
		sdb[x].batt.out = 0.0;
		sdb[x].power.batt = 0.0;
		sdb[x].power.version = 1;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_beam (int x, int beam, double damage)
{
	if (!sdb[x].beam.exist || sdb[x].blist.damage[beam] == -1.0)
		return;
	sdb[x].blist.damage[beam] -= damage
	  / ((sdb[x].blist.cost[beam] + sdb[x].blist.bonus[beam]) / 10.0);
	if (sdb[x].blist.damage[beam] < -1.0)
		sdb[x].blist.damage[beam] = -1.0;
	do_console_notify(x, console_tactical, console_damage, 0,
	  ansi_alert(tprintf("%s%s %d%s: %s %s",
	  ANSI_CYAN, system_name[3], beam + 1, ANSI_WHITE,
	  unparse_percent(sdb[x].blist.damage[beam]),
	  unparse_damage(sdb[x].blist.damage[beam]))));
	if (sdb[x].blist.damage[beam] <= 0.0)
		if (sdb[x].blist.active[beam]) {
			sdb[x].beam.in -= 10.0 * sdb[x].blist.cost[beam];
			sdb[x].blist.active[beam] = 0;
			sdb[x].blist.lock[beam] = 0;
		}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_cloak (int x, double damage)
{
	if (!sdb[x].cloak.exist || sdb[x].cloak.damage == -1.0)
		return;
	sdb[x].cloak.damage -= damage / (1.0 + (sdb[x].structure.max_structure / 100.0));
	if (sdb[x].cloak.damage < -1.0)
		sdb[x].cloak.damage = -1.0;
	do_console_notify(x, console_helm, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[4], ANSI_WHITE,
	  unparse_percent(sdb[x].cloak.damage),
	  unparse_damage(sdb[x].cloak.damage))));
	if (sdb[x].cloak.damage <= 0.0)
		if (sdb[x].cloak.active) {
			sdb[x].cloak.active = 0;
			sdb[x].engine.version = 1;
			alert_ship_cloak_offline(x);
		}
	sdb[x].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_ew (int x, double damage)
{
	if (!sdb[x].sensor.ew_exist || sdb[x].sensor.ew_damage == -1.0)
		return;
	sdb[x].sensor.ew_damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].sensor.ew_damage < -1.0)
		sdb[x].sensor.ew_damage = -1.0;
	do_console_notify(x, console_tactical, console_science, console_damage,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[5], ANSI_WHITE,
	  unparse_percent(sdb[x].sensor.ew_damage),
	  unparse_damage(sdb[x].sensor.ew_damage))));
	if (sdb[x].sensor.ew_damage <= 0.0)
		if (sdb[x].sensor.ew_active)
			sdb[x].sensor.ew_active = 0;
	sdb[x].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_impulse (int x, double damage)
{
	if (!sdb[x].engine.impulse_exist || sdb[x].engine.impulse_damage == -1.0)
		return;
	sdb[x].engine.impulse_damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].engine.impulse_damage < -1.0)
		sdb[x].engine.impulse_damage = -1.0;
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[6], ANSI_WHITE,
	  unparse_percent(sdb[x].engine.impulse_damage),
	  unparse_damage(sdb[x].engine.impulse_damage))));
	sdb[x].engine.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_lrs (int x, double damage)
{
	if (!sdb[x].sensor.lrs_exist || sdb[x].sensor.lrs_damage == -1.0)
		return;
	sdb[x].sensor.lrs_damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].sensor.lrs_damage < -1.0)
		sdb[x].sensor.lrs_damage = -1.0;
	do_console_notify(x, console_tactical, console_science, console_damage,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[7], ANSI_WHITE,
	  unparse_percent(sdb[x].sensor.lrs_damage),
	  unparse_damage(sdb[x].sensor.lrs_damage))));
	if (sdb[x].sensor.lrs_damage <= 0.0)
		if (sdb[x].sensor.lrs_active)
			sdb[x].sensor.lrs_active = 0;
	sdb[x].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_main (int x, double damage)
{
	if (!sdb[x].main.exist || sdb[x].main.damage == -1.0)
		return;
	if (sdb[x].power.main != 0.0
	  && sdb[x].main.damage > 0.0
  	  && (sdb[x].main.damage - damage / sdb[x].main.gw <= 0.0)) {
		alert_main_overload(x);
	}
	sdb[x].main.damage -= damage / sdb[x].main.gw;
	if (sdb[x].main.damage < -1.0) {
		sdb[x].main.damage = -1.0;
		if (sdb[x].power.main != 0.0) {
			do_all_console_notify(x, tprintf(ansi_warn("%s core breach."), system_name[8]));
			damage_structure(x, sdb[x].power.main * (get_random_long(0,100) + 1.0));
		}
	}
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[8], ANSI_WHITE,
	  unparse_percent(sdb[x].main.damage),
	  unparse_damage(sdb[x].main.damage))));
	return;
}

/* ------------------------------------------------------------------------ */

void damage_missile (int x, int missile, double damage)
{
	if (!sdb[x].missile.exist || sdb[x].mlist.damage[missile] == -1.0)
		return;
	sdb[x].mlist.damage[missile] -= damage
	  / (sdb[x].mlist.warhead[missile] / 10.0);
	if (sdb[x].mlist.damage[missile] < -1.0)
		sdb[x].mlist.damage[missile] = -1.0;
	do_console_notify(x, console_tactical, console_damage, 0,
	  ansi_alert(tprintf("%s%s %d%s: %s %s",
	  ANSI_CYAN, system_name[9], missile + 1, ANSI_WHITE,
	  unparse_percent(sdb[x].mlist.damage[missile]),
	  unparse_damage(sdb[x].mlist.damage[missile]))));
	if (sdb[x].mlist.damage[missile] <= 0.0)
		if (sdb[x].mlist.active[missile]) {
			sdb[x].missile.in -= sdb[x].mlist.cost[missile];
			sdb[x].mlist.active[missile] = 0;
			sdb[x].mlist.lock[missile] = 0;
		}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_shield (int x, int shield, double damage)
{
	if (!sdb[x].shield.exist || sdb[x].shield.damage[shield] == -1.0)
		return;
	sdb[x].shield.damage[shield] -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].shield.damage[shield] < -1.0)
		sdb[x].shield.damage[shield] = -1.0;
	do_console_notify(x, console_helm, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, unparse_shield(shield), ANSI_WHITE,
	  unparse_percent(sdb[x].shield.damage[shield]),
	  unparse_damage(sdb[x].shield.damage[shield]))));
	if (sdb[x].shield.damage[shield] <= 0.0) {
		sdb[x].shield.active[shield] = 0;
		sdb[x].engine.version = 1;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_srs (int x, double damage)
{
	if (!sdb[x].sensor.srs_exist || sdb[x].sensor.srs_damage == -1.0)
		return;
	sdb[x].sensor.srs_damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].sensor.srs_damage < -1.0)
		sdb[x].sensor.srs_damage = -1.0;
	do_console_notify(x, console_tactical, console_science, console_damage,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[11], ANSI_WHITE,
	  unparse_percent(sdb[x].sensor.srs_damage),
	  unparse_damage(sdb[x].sensor.srs_damage))));
	if (sdb[x].sensor.srs_damage <= 0.0)
		if (sdb[x].sensor.srs_active)
			sdb[x].sensor.srs_active = 0;
	sdb[x].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_tract (int x, double damage)
{
	if (!sdb[x].tract.exist || sdb[x].tract.damage == -1.0)
		return;
	sdb[x].tract.damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].tract.damage < -1.0)
		sdb[x].tract.damage = -1.0;
	do_console_notify(x, console_operation, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[12], ANSI_WHITE,
	  unparse_percent(sdb[x].tract.damage),
	  unparse_damage(sdb[x].tract.damage))));
	if (sdb[x].tract.damage <= 0.0)
		if (sdb[x].tract.active) {
			if (sdb[x].status.tractoring) {
				alert_tract_lost(x, sdb[x].status.tractoring);
				sdb[x].tract.lock = 0;
				sdb[sdb[x].status.tractoring].status.tractored = 0;
				sdb[sdb[x].status.tractoring].power.version = 1;
				sdb[x].status.tractoring = 0;
			}
			sdb[x].tract.active = 0;
		}
	sdb[x].power.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void damage_trans (int x, double damage)
{
	if (!sdb[x].trans.exist || sdb[x].trans.damage == -1.0)
		return;
	sdb[x].trans.damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].trans.damage < -1.0)
		sdb[x].trans.damage = -1.0;
	do_console_notify(x, console_operation, console_damage, console_transporter,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[13], ANSI_WHITE,
	  unparse_percent(sdb[x].trans.damage),
	  unparse_damage(sdb[x].trans.damage))));
	if (sdb[x].trans.damage <= 0.0)
		if (sdb[x].trans.active) {
			sdb[x].trans.s_lock = 0;
			sdb[x].trans.d_lock = 0;
			sdb[x].trans.active = 0;
		}
	return;
}

/* ------------------------------------------------------------------------ */

void damage_warp (int x, double damage)
{
	if (!sdb[x].engine.warp_exist || sdb[x].engine.warp_damage == -1.0)
		return;
	sdb[x].engine.warp_damage -= damage / (1.0 + (sdb[x].structure.max_structure / 10.0));
	if (sdb[x].engine.warp_damage < -1.0)
		sdb[x].engine.warp_damage = -1.0;
	do_console_notify(x, console_engineering, console_damage, 0,
	  ansi_alert(tprintf("%s%s%s: %s %s",
	  ANSI_CYAN, system_name[14], ANSI_WHITE,
	  unparse_percent(sdb[x].engine.warp_damage),
	  unparse_damage(sdb[x].engine.warp_damage))));
	sdb[x].engine.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */



/* ------------------------------------------------------------------------ */

int repair_everything (void)
{
	register int i;

	if (sdb[n].aux.exist)
		sdb[n].aux.damage = 1.0;

	if (sdb[n].batt.exist)
		sdb[n].batt.damage = 1.0;

	if (sdb[n].beam.exist)
		for (i = 0; i < sdb[n].beam.banks; ++i)
			sdb[n].blist.damage[i] = 1.0;

	if (sdb[n].missile.exist)
		for (i = 0; i < sdb[n].missile.tubes; ++i)
			sdb[n].mlist.damage[i] = 1.0;

	if (sdb[n].cloak.exist)
		sdb[n].cloak.damage = 1.0;

	if (sdb[n].engine.warp_exist)
		sdb[n].engine.warp_damage = 1.0;
	if (sdb[n].engine.impulse_exist)
		sdb[n].engine.impulse_damage = 1.0;

	if (sdb[n].main.exist)
		sdb[n].main.damage = 1.0;

	if (sdb[n].sensor.ew_exist)
		sdb[n].sensor.ew_damage = 1.0;
	if (sdb[n].sensor.lrs_exist)
		sdb[n].sensor.lrs_damage = 1.0;
	if (sdb[n].sensor.srs_exist)
		sdb[n].sensor.srs_damage = 1.0;

	if (sdb[n].shield.exist)
		for (i = 0; i < MAX_SHIELD_NAME; ++i)
			sdb[n].shield.damage[i] = 1.0;

	sdb[n].status.crippled = 0;

	sdb[n].structure.superstructure = sdb[n].structure.max_structure;
	sdb[n].structure.repair = sdb[n].structure.max_repair;

	if (sdb[n].trans.exist)
		sdb[n].trans.damage = 1.0;

	if (sdb[n].tract.exist)
		sdb[n].tract.damage = 1.0;

	sdb[n].sensor.version = 1;
	sdb[n].engine.version = 1;
	sdb[n].power.version = 1;
	sdb[n].cloak.version = 1;
	up_cochranes();
	up_empire();
	up_quadrant();
	up_vectors();
	up_resolution();
	up_signature(n);
	up_visibility();
	debug_space(n);

	return 1;
}

/* ------------------------------------------------------------------------ */

void dump_space (dbref executor) /* dumps the sdb[] struct into the space objects */
{
	int result;

	for (n = MIN_SPACE_OBJECTS ; n <= max_space_objects ; ++n)
		if (sdb[n].structure.type) {
			if (sdb[n].structure.type == 1 && sdb[n].status.active && sdb[n].power.total == 0.0) /* Only ships - Anyasha - STDW */
				result = do_set_inactive(sdb[n].object);
			do_space_db_write(sdb[n].object, executor);
		}

	return;
}

/* ------------------------------------------------------------------------ */

int debug_space (int x)
{
	register int i, bug = 1;

/* --- OBJECT ------------------------------------------------------------- */

	if (sdb[x].structure.type <= 0) {
		sdb[x].structure.type = 0;
		bug = 0;
	}
	if (!SpaceObj(sdb[x].object) || !GoodObject(sdb[x].object)) {
		sdb[x].object = 0;
		sdb[x].structure.type = 0;
		bug = 0;
	}

/* --- LOCATION ----------------------------------------------------------- */

	sdb[x].location = 0;
	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
		if (SpaceObj(sdb[i].object)) {
			if (Location(sdb[i].object) == sdb[x].object)
				sdb[i].location = x;
			if (Location(sdb[x].object) == sdb[i].object)
				sdb[x].location = i;
		}

/* --- MAIN --------------------------------------------------------------- */

	if (!sdb[x].main.exist || sdb[x].main.gw <= 0) {
		sdb[x].main.damage = 0.0;
		sdb[x].main.exist = 0;
		sdb[x].main.gw = 0.0;
		sdb[x].main.in = 0.0;
		sdb[x].main.out = 0.0;
		sdb[x].power.main = 0.0;
	}

/* --- AUX ---------------------------------------------------------------- */

	if (!sdb[x].aux.exist || sdb[x].aux.gw <= 0) {
		sdb[x].aux.damage = 0.0;
		sdb[x].aux.exist = 0;
		sdb[x].aux.gw = 0.0;
		sdb[x].aux.in = 0.0;
		sdb[x].aux.out = 0.0;
		sdb[x].power.aux = 0.0;
	}

/* --- BATT --------------------------------------------------------------- */

	if (!sdb[x].batt.exist || sdb[x].batt.gw <= 0) {
		sdb[x].batt.damage = 0.0;
		sdb[x].batt.exist = 0;
		sdb[x].batt.gw = 0.0;
		sdb[x].batt.in = 0.0;
		sdb[x].batt.out = 0.0;
		sdb[x].fuel.reserves = 0.0;
		sdb[x].power.batt = 0.0;
	}

/* --- ALLOCATE ----------------------------------------------------------- */

	if (!sdb[x].main.exist && !sdb[x].aux.exist && !sdb[x].batt.exist) {
		sdb[x].alloc.helm = 0.0;
		sdb[x].alloc.tactical = 0.0;
		sdb[x].alloc.operations = 1.0;
		sdb[x].alloc.movement = 0.0;
		sdb[x].alloc.shields = 0.0;
		for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
			sdb[x].alloc.shield[i] = 0.0;
		sdb[x].alloc.cloak = 0.0;
		sdb[x].alloc.beams = 0.0;
		sdb[x].alloc.missiles = 0.0;
		sdb[x].alloc.sensors = 0.0;
		sdb[x].alloc.ecm = 0.0;
		sdb[x].alloc.eccm = 0.0;
		sdb[x].alloc.transporters = 0.0;
		sdb[x].alloc.tractors = 0.0;
		sdb[x].alloc.miscellaneous = 1.0;
		sdb[x].power.total = 0.0;
		sdb[x].beam.in = 0.0;
		sdb[x].beam.out = 0.0;
		sdb[x].missile.in = 0.0;
		sdb[x].missile.out = 0.0;
	}

/* --- BEAM --------------------------------------------------------------- */

	if (!sdb[x].beam.exist || sdb[x].beam.banks <= 0) {
		sdb[x].beam.banks = 0;
		sdb[x].beam.exist = 0;
		sdb[x].beam.freq = 0.0;
		sdb[x].beam.in = 0.0;
		sdb[x].beam.out = 0.0;
		for (i = 0; i < MAX_BEAM_BANKS; ++i) {
			sdb[x].blist.name[i] = 0;
			sdb[x].blist.damage[i] = 0.0;
			sdb[x].blist.bonus[i] = 0;
			sdb[x].blist.cost[i] = 0;
			sdb[x].blist.range[i] = 0;
			sdb[x].blist.arcs[i] = 0;
			sdb[x].blist.active[i] = 0;
			sdb[x].blist.lock[i] = 0;
			sdb[x].blist.load[i] = 0;
			sdb[x].blist.recycle[i] = 0;
		}
	} else {
		if (sdb[x].beam.in < 0.0) {
			sdb[x].beam.in = 0.0;
			bug = 0;
		}
		if (sdb[x].beam.out < 0.0) {
			sdb[x].beam.out = 0.0;
			bug = 0;
		}
		if (sdb[x].beam.banks > MAX_BEAM_BANKS) {
			sdb[x].beam.banks = MAX_BEAM_BANKS;
			bug = 0;
		}
		if (sdb[x].beam.freq <= 1.0 || sdb[x].beam.freq >= 1000.0) {
			sdb[x].beam.freq = get_random_long(0,10000) / 100.0;
			bug = 0;
		}
		for (i = 0; i < MAX_BEAM_BANKS; ++i) {
			if (!(sdb[x].blist.arcs[i] & 1) && !(sdb[x].blist.arcs[i] & 4))
				sdb[x].blist.arcs[i] += 5;
			if (!(sdb[x].blist.arcs[i] & 2) && !(sdb[x].blist.arcs[i] & 8))
				sdb[x].blist.arcs[i] += 10;
			if (!(sdb[x].blist.arcs[i] & 16) && !(sdb[x].blist.arcs[i] & 32))
				sdb[x].blist.arcs[i] += 48;
		}
 		for (i = 0; i < MAX_BEAM_BANKS; ++i) {
			if (sdb[x].blist.recycle[i] < 1) {
 				sdb[x].blist.recycle[i] = 1;
// Jarek fixed this to not throw bugs for every non-existant weapon
				if(i < sdb[x].beam.banks)
	 				bug = 0;
 			}
 		}
	}

/* --- MISSILE ------------------------------------------------------------ */

	if (!sdb[x].missile.exist || sdb[x].missile.tubes <= 0) {
		sdb[x].missile.exist = 0;
		sdb[x].missile.freq = 0.0;
		sdb[x].missile.tubes = 0;
		sdb[x].missile.in = 0.0;
		sdb[x].missile.out = 0.0;
		for (i = 0; i < MAX_MISSILE_TUBES; ++i) {
			sdb[x].mlist.name[i] = 0;
			sdb[x].mlist.damage[i] = 0.0;
			sdb[x].mlist.warhead[i] = 0;
			sdb[x].mlist.cost[i] = 0;
			sdb[x].mlist.range[i] = 0;
			sdb[x].mlist.arcs[i] = 0;
			sdb[x].mlist.active[i] = 0;
			sdb[x].mlist.lock[i] = 0;
			sdb[x].mlist.load[i] = 0;
			sdb[x].mlist.recycle[i] = 0;
		}
	} else {
		if (sdb[x].missile.out < 0.0) {
			sdb[x].missile.out = 0.0;
			bug = 0;
		}
		if (sdb[x].missile.in < 0.0) {
			sdb[x].missile.in = 0.0;
			bug = 0;
		}
		if (sdb[x].missile.tubes > MAX_MISSILE_TUBES) {
			sdb[x].missile.tubes = MAX_MISSILE_TUBES;
			bug = 0;
		}
		if (sdb[x].missile.freq <= 1.0 || sdb[x].missile.freq >= 1000.0) {
			sdb[x].missile.freq = get_random_long(0,10000) / 100.0;
			bug = 0;
		}
		for (i = 0; i < MAX_MISSILE_TUBES; ++i) {
			if (!(sdb[x].mlist.arcs[i] & 1) && !(sdb[x].mlist.arcs[i] & 4))
				sdb[x].mlist.arcs[i] += 5;
			if (!(sdb[x].mlist.arcs[i] & 2) && !(sdb[x].mlist.arcs[i] & 8))
				sdb[x].mlist.arcs[i] += 10;
			if (!(sdb[x].mlist.arcs[i] & 16) && !(sdb[x].mlist.arcs[i] & 32))
				sdb[x].mlist.arcs[i] += 48;
		}
 		for (i = 0; i < MAX_MISSILE_TUBES; ++i) {
 			if (sdb[x].mlist.recycle[i] < 1) {
 				sdb[x].mlist.recycle[i] = 1;
// Jarek fixed this to not throw bugs for every non-existant weapon
				if(i < sdb[x].missile.tubes)
 					bug = 0;
 			}
 		}
	}

/* --- ENGINE ------------------------------------------------------------- */

	if (!sdb[x].engine.impulse_exist) {
		sdb[x].engine.impulse_exist = 0;
		sdb[x].engine.impulse_damage = 0.0;
		sdb[x].engine.impulse_max = 0.0;
		sdb[x].engine.impulse_cruise = 0.0;
		sdb[x].engine.impulse_hard_cruise = 0.0;  /* Anyasha - STDW - Hard Cruise Speed */
	}
	if (!sdb[x].engine.warp_exist) {
		sdb[x].engine.warp_exist = 0;
		sdb[x].engine.warp_damage = 0.0;
		sdb[x].engine.warp_max = 0.0;
		sdb[x].engine.warp_cruise = 0.0;
		sdb[x].engine.warp_hard_cruise = 0.0;  /* Anyasha - STDW - Hard Cruise Speed */
	}
	if (!sdb[x].engine.warp_exist && !sdb[x].engine.impulse_exist) {
		sdb[x].move.in = 0.0;
		sdb[x].move.out = 0.0;
	}

/* --- STRUCTURE ---------------------------------------------------------- */

	if (sdb[x].structure.displacement <= 0) {
		sdb[x].structure.displacement = 1;
		bug = 0;
	}
	if (sdb[x].structure.cargo_hold > sdb[x].structure.displacement) {
		sdb[x].structure.cargo_hold = sdb[x].structure.displacement;
		bug = 0;
	} else if (sdb[x].structure.cargo_hold < 0) {
		sdb[x].structure.cargo_hold = 0;
		bug = 0;
	}
	if (sdb[x].structure.max_structure <= 0) {
		sdb[x].structure.max_structure = 1;
		bug = 0;
	}
	if (sdb[x].structure.superstructure > sdb[x].structure.max_structure) {
		sdb[x].structure.superstructure = sdb[x].structure.max_structure;
		bug = 0;
	}
	if (sdb[x].structure.max_repair < 0) {
		sdb[x].structure.max_repair = 0;
		bug = 0;
	}
	if (sdb[x].structure.repair > sdb[x].structure.max_repair) {
		sdb[x].structure.repair = sdb[x].structure.max_repair;
		bug = 0;
	} else if (sdb[x].structure.repair < 0.0) {
		sdb[x].structure.repair = 0.0;
		bug = 0;
	}

/* --- SENSOR ------------------------------------------------------------- */

	if (!sdb[x].sensor.lrs_exist) {
		sdb[x].sensor.lrs_active = 0;
		sdb[x].sensor.lrs_damage = 0.0;
		sdb[x].sensor.lrs_resolution = 0.0;
	}
	if (!sdb[x].sensor.srs_exist) {
		sdb[x].sensor.srs_active = 0;
		sdb[x].sensor.srs_damage = 0.0;
		sdb[x].sensor.srs_resolution = 0.0;
	}
	if (!sdb[x].sensor.ew_exist) {
		sdb[x].sensor.ew_active = 0;
		sdb[x].sensor.ew_damage = 0.0;
	}
	if (!sdb[x].sensor.srs_exist && !sdb[x].sensor.lrs_exist) {
		sdb[x].sensor.contacts = 0;
		sdb[x].sensor.counter = 0;
		for (i = 0 ; i < MAX_SENSOR_CONTACTS ; ++i) {
			sdb[x].slist.sdb[i] = 0;
			sdb[x].slist.num[i] = 0;
			sdb[x].slist.lev[i] = 0.0;
		}
	}

/* --- SHIELD ------------------------------------------------------------- */
	if (!sdb[x].shield.exist || sdb[x].shield.ratio <= 0.0 || sdb[x].shield.maximum <= 0) {
		sdb[x].shield.ratio = 0.0;
		sdb[x].shield.maximum = 0;
		sdb[x].shield.freq = 0.0;
		sdb[x].shield.exist = 0;
		for (i = 0 ; i < MAX_SHIELD_NAME; ++i) {
			sdb[x].shield.damage[i] = 0.0;
			sdb[x].shield.active[i] = 0;
			sdb[x].shield.capacitor[i] = 0.0; /* line added for shield capacitors - Anya  STDW */
		}
	}

/* --- TECH --------------------------------------------------------------- */

	if (sdb[x].tech.firing <= 0.0) {
		sdb[x].tech.firing = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.fuel <= 0.0) {
		sdb[x].tech.fuel = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.stealth <= 0.0) {
		sdb[x].tech.stealth = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.cloak <= 0.0) {
		sdb[x].tech.cloak = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.sensors <= 0.0) {
		sdb[x].tech.sensors = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.main_max <= 0.0) {
		sdb[x].tech.main_max = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.aux_max <= 0.0) {
		sdb[x].tech.aux_max = 1.0;
		bug = 0;
	}
	if (sdb[x].tech.armor <= 0.0) {
		sdb[x].tech.armor = 1.0;
		bug = 0;
	}

/* --- MOVE --------------------------------------------------------------- */

	if (sdb[x].move.ratio <= 0.0) {
		sdb[x].move.ratio = 1.0;
		bug = 0;
	}

/* --- CLOAK -------------------------------------------------------------- */

	if (!sdb[x].cloak.exist || sdb[x].cloak.cost <= 0) {
		sdb[x].cloak.cost = 0;
		sdb[x].cloak.damage = 0.0;
		sdb[x].cloak.exist = 0;
		sdb[x].cloak.freq = 0.0;
		sdb[x].cloak.active = 0;
	} else {
		if (sdb[x].cloak.freq <= 1.0 || sdb[x].cloak.freq >= 1000.0) {
			sdb[x].cloak.freq = get_random_long(0,10000) / 100.0;
			bug = 0;
		}
	}

/* --- TRANS -------------------------------------------------------------- */

	if (!sdb[x].trans.exist) {
		sdb[x].trans.cost = 0;
		sdb[x].trans.damage = 0;
		sdb[x].trans.freq = 0;
		sdb[x].trans.active = 0;
		sdb[x].trans.d_lock = 0;
		sdb[x].trans.s_lock = 0;
	} else if (sdb[x].trans.freq <= 1.0 || sdb[x].trans.freq >= 1000.0) {
		sdb[x].trans.freq = get_random_long(0,10000) / 100.0;
		bug = 0;
	}
	if (sdb[x].trans.d_lock)
		if (sdb[x].trans.d_lock != x)
			if (sdb2contact(n, sdb[x].trans.d_lock) == SENSOR_FAIL) {
				sdb[x].trans.d_lock = 0;
			}
	if (sdb[x].trans.s_lock)
		if (sdb[x].trans.s_lock != x)
			if (sdb2contact(n, sdb[x].trans.s_lock) == SENSOR_FAIL) {
				sdb[x].trans.s_lock = 0;
			}

/* --- TRACT -------------------------------------------------------------- */

	if (!sdb[x].tract.exist) {
		sdb[x].tract.cost = 0;
		sdb[x].tract.damage = 0;
		sdb[x].tract.freq = 0;
		sdb[x].tract.active = 0;
		sdb[x].tract.lock = 0;
		sdb[x].status.tractoring = 0;
	} else if (sdb[x].tract.freq <= 1.0 || sdb[x].tract.freq >= 1000.0) {
		sdb[x].tract.freq = get_random_long(0,10000) / 100.0;
		bug = 0;
	}
	/* if (sdb[x].status.tractoring)
		if (sdb2contact(n, sdb[x].status.tractoring) == SENSOR_FAIL) {
			sdb[sdb[x].status.tractoring].status.tractored = 0;
			sdb[x].tract.lock = 0;
			sdb[x].status.tractoring = 0;
		} */
	/* if (sdb[x].status.tractored)
		if (sdb2contact(sdb[x].status.tractored, x) == SENSOR_FAIL) {
			sdb[sdb[x].status.tractored].status.tractoring = 0;
			sdb[sdb[x].status.tractored].tract.lock = 0;
			sdb[x].status.tractored = 0;
		} */

/* --- FUEL --------------------------------------------------------------- */

	if (sdb[x].fuel.antimatter < 0.0)
		sdb[x].fuel.antimatter = 0.0;
	if (sdb[x].fuel.deuterium < 0.0)
		sdb[x].fuel.deuterium = 0.0;
	if (sdb[x].fuel.reserves < 0.0)
		sdb[x].fuel.reserves = 0.0;
	if (sdb[x].fuel.antimatter > sdb2max_antimatter(x))
		sdb[x].fuel.antimatter = sdb2max_antimatter(x);
	if (sdb[x].fuel.deuterium > sdb2max_deuterium(x))
		sdb[x].fuel.deuterium = sdb2max_deuterium(x);
	if (sdb[x].fuel.reserves > sdb2max_reserve(x))
		sdb[x].fuel.reserves = sdb2max_reserve(x);

/* --- STATUS ------------------------------------------------------------- */

	if (sdb[x].structure.superstructure <= -sdb[x].structure.max_structure) {
		sdb[x].status.crippled = 2;
	} else if (sdb[x].structure.superstructure <= 0.0) {
		sdb[x].status.crippled = 1;
	} else {
		sdb[x].status.crippled = 0;
	}

/* --- other updates ------------------------------------------------------ */

	up_cochranes();
	up_turn_rate();
	up_vectors();
	up_empire();
	up_quadrant();
	up_resolution();
	up_signature(x);
	up_visibility();

	return bug;
}

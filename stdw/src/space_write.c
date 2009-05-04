/* space_write.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

int do_space_db_write (dbref ship, dbref executor)
{
	register int i;
	int x, result;
	static char buffer[BUFFER_LEN];
	ATTR *a;

/* SDB */

	x = 0;
	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
		if (sdb[i].object == ship) {
			x = i;
			break;
		}
	if (!GoodSDB(x)) {
		a = atr_get(ship, SDB_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "WRITE: unable to read SDB attribute.");
			return 0;
		}
		result = parse_integer(atr_value(a));
		if (!GoodSDB(result)) {
			write_spacelog(executor, ship, "WRITE: unable to validate SDB.");
			return 0;
		} else if (sdb[result].object != ship) {
			write_spacelog(executor, ship, "WRITE: unable to verify SDB.");
			return 0;
		} else if (sdb[result].structure.type == 0) {
			write_spacelog(executor, ship, "WRITE: unable to verify STRUCTURE.");
			return 0;
		} else
			x = result;
	}
	snprintf(buffer, sizeof(buffer), "%d", x);
	result = atr_add(ship, SDB_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write SDB attribute.");
		return 0;
	} else if (max_space_objects < x)
		max_space_objects = x;

/* OBJECT */

	if (!SpaceObj(ship) || !GoodObject(ship)) {
		write_spacelog(executor, ship, "WRITE: unable to validate SPACE_OBJECT.");
		return 0;
	}

/* DEBUGGING */

	n = x;
	result = debug_space(x);
	if (result == 0) {
		write_spacelog(executor, ship, "WRITE: Bugs found and corrected.");
	}

/* LOCATION */

	strncpy(buffer, unparse_integer(sdb[x].location), sizeof(buffer) - 1);
	result = atr_add(ship, LOCATION_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write LOCATION attribute.");
		return 0;
	}

/* SPACE */

	strncpy(buffer, unparse_integer(sdb[x].space), sizeof(buffer) - 1);
	result = atr_add(ship, SPACE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write SPACE attribute.");
		return 0;
	}

/* ALLOCATE */

	snprintf(buffer, sizeof(buffer), "%d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
		sdb[x].alloc.version,
		sdb[x].alloc.helm,
		sdb[x].alloc.tactical,
		sdb[x].alloc.operations,
		sdb[x].alloc.movement,
		sdb[x].alloc.shields,
		sdb[x].alloc.shield[0],
		sdb[x].alloc.shield[1],
		sdb[x].alloc.shield[2],
		sdb[x].alloc.shield[3],
		sdb[x].alloc.shield[4],
		sdb[x].alloc.shield[5],
		sdb[x].alloc.cloak,
		sdb[x].alloc.beams,
		sdb[x].alloc.missiles,
		sdb[x].alloc.sensors,
		sdb[x].alloc.ecm,
		sdb[x].alloc.eccm,
		sdb[x].alloc.transporters,
		sdb[x].alloc.tractors,
		sdb[x].alloc.miscellaneous);
	result = atr_add(ship, ALLOCATE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write ALLOCATE attribute.");
		return 0;
	}

/* BEAM */

	snprintf(buffer, sizeof(buffer), "%f %f %f %d %d",
		sdb[x].beam.in,
		sdb[x].beam.out,
		sdb[x].beam.freq,
		sdb[x].beam.exist,
		sdb[x].beam.banks);
	result = atr_add(ship, BEAM_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM attribute.");
		return 0;
	}

/* BEAM_ACTIVE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.active[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_ACTIVE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_ACTIVE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_ACTIVE attribute.");
		return 0;
	}

/* BEAM_NAME */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.name[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_NAME_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_NAME_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_NAME attribute.");
		return 0;
	}

/* BEAM_DAMAGE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%f ", sdb[x].blist.damage[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_DAMAGE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_DAMAGE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_DAMAGE attribute.");
		return 0;
	}

/* BEAM_BONUS */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.bonus[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_BONUS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_BONUS_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_BONUS attribute.");
		return 0;
	}

/* BEAM_COST */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.cost[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_COST_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_COST_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_COST attribute.");
		return 0;
	}

/* BEAM_RANGE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.range[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_RANGE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_RANGE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_RANGE attribute.");
		return 0;
	}

/* BEAM_ARCS */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.arcs[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_ARCS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_ARCS_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_ARCS attribute.");
		return 0;
	}

/* BEAM_LOCK */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.lock[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_LOCK_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_LOCK_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_LOCK attribute.");
		return 0;
	}

/* BEAM_LOAD */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.load[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_LOAD_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_LOAD_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_LOAD attribute.");
		return 0;
	}

/* BEAM_RECYCLE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].beam.exist) {
		for (i = 0; i < sdb[x].beam.banks; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].blist.recycle[i]), sizeof(buffer) - 1);
			result = atr_add(ship, BEAM_RECYCLE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) BEAM_RECYCLE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BEAM_RECYCLE attribute.");
		return 0;
	}

/* MISSILE */

	snprintf(buffer, sizeof(buffer), "%f %f %f %d %d",
		sdb[x].missile.in,
		sdb[x].missile.out,
		sdb[x].missile.freq,
		sdb[x].missile.exist,
		sdb[x].missile.tubes);
	result = atr_add(ship, MISSILE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE attribute.");
		return 0;
	}

/* MISSILE_ACTIVE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.active[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_ACTIVE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_ACTIVE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_ACTIVE attribute.");
		return 0;
	}

/* MISSILE_NAME */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.name[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_NAME_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_NAME_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_NAME attribute.");
		return 0;
	}

/* MISSILE_DAMAGE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%f ", sdb[x].mlist.damage[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_DAMAGE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_DAMAGE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_DAMAGE attribute.");
		return 0;
	}

/* MISSILE_WARHEAD */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.warhead[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_WARHEAD_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_WARHEAD_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_WARHEAD attribute.");
		return 0;
	}

/* MISSILE_COST */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.cost[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_COST_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_COST_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_COST attribute.");
		return 0;
	}

/* MISSILE_RANGE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.range[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_RANGE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_RANGE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_RANGE attribute.");
		return 0;
	}

/* MISSILE_ARCS */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.arcs[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_ARCS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_ARCS_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_ARCS attribute.");
		return 0;
	}

/* MISSILE_LOCK */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.lock[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_LOCK_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_LOCK_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_LOCK attribute.");
		return 0;
	}

/* MISSILE_LOAD */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.load[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_LOAD_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_LOAD_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_LOAD attribute.");
		return 0;
	}

/* MISSILE_RECYCLE */

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (sdb[x].missile.exist) {
		for (i = 0; i < sdb[x].missile.tubes; ++i)
			strncat(buffer, tprintf("%d ", sdb[x].mlist.recycle[i]), sizeof(buffer) - 1);
			result = atr_add(ship, MISSILE_RECYCLE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	} else {
		atr_clr(ship, (char *) MISSILE_RECYCLE_ATTR_NAME, GOD);
		result = 0;
	}
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MISSILE_RECYCLE attribute.");
		return 0;
	}

/* ENGINE */

	snprintf(buffer, sizeof(buffer), "%d %f %f %d %f %f %d %f %f %f %f",
		sdb[x].engine.version,
		sdb[x].engine.warp_damage,
		sdb[x].engine.warp_max,
		sdb[x].engine.warp_exist,
		sdb[x].engine.impulse_damage,
		sdb[x].engine.impulse_max,
		sdb[x].engine.impulse_exist,
		sdb[x].engine.warp_cruise,
		sdb[x].engine.impulse_cruise,
		sdb[x].engine.warp_hard_cruise,
		sdb[x].engine.impulse_hard_cruise
		);  /* Added by Anya for hard cruise limits - Anya STDW */
	result = atr_add(ship, ENGINE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write ENGINE attribute.");
		return 0;
	}

/* STRUCTURE */

	snprintf(buffer, sizeof(buffer), "%d %f %f %f %f %d %d %d %d %d %f %d",
		sdb[x].structure.type,
		sdb[x].structure.displacement,
		sdb[x].structure.cargo_hold,
		sdb[x].structure.cargo_mass,
		sdb[x].structure.superstructure,
		sdb[x].structure.max_structure,
		sdb[x].structure.has_landing_pad,
		sdb[x].structure.has_docking_bay,
		sdb[x].structure.can_land,
		sdb[x].structure.can_dock,
		sdb[x].structure.repair,
		sdb[x].structure.max_repair);
	result = atr_add(ship, STRUCTURE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write STRUCTURE attribute.");
		return 0;
	}

/* POWER */

	snprintf(buffer, sizeof(buffer), "%d %f %f %f %f",
		sdb[x].power.version,
		sdb[x].power.main,
		sdb[x].power.aux,
		sdb[x].power.batt,
		sdb[x].power.total);
	result = atr_add(ship, POWER_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write POWER attribute.");
		return 0;
	}

/* SENSOR */

	snprintf(buffer, sizeof(buffer), "%d %f %d %d %f %f %f %d %d %f %f %f %d %d %f %d %d",
		sdb[x].sensor.version,
		sdb[x].sensor.lrs_damage,
		sdb[x].sensor.lrs_active,
		sdb[x].sensor.lrs_exist,
		sdb[x].sensor.lrs_resolution,
		sdb[x].sensor.lrs_signature,
		sdb[x].sensor.srs_damage,
		sdb[x].sensor.srs_active,
		sdb[x].sensor.srs_exist,
		sdb[x].sensor.srs_resolution,
		sdb[x].sensor.srs_signature,
		sdb[x].sensor.ew_damage,
		sdb[x].sensor.ew_active,
		sdb[x].sensor.ew_exist,
		sdb[x].sensor.visibility,
		sdb[x].sensor.contacts,
		sdb[x].sensor.counter);
	result = atr_add(ship, SENSOR_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write SENSOR attribute.");
		return 0;
	}

/* SHIELD */

	snprintf(buffer, sizeof(buffer), "%f %d %f %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f",
		sdb[x].shield.ratio,
		sdb[x].shield.maximum,
		sdb[x].shield.freq,
		sdb[x].shield.exist,
		sdb[x].shield.active[0],
		sdb[x].shield.active[1],
		sdb[x].shield.active[2],
		sdb[x].shield.active[3],
		sdb[x].shield.active[4],
		sdb[x].shield.active[5],
		sdb[x].shield.damage[0],
		sdb[x].shield.damage[1],
		sdb[x].shield.damage[2],
		sdb[x].shield.damage[3],
		sdb[x].shield.damage[4],
		sdb[x].shield.damage[5],
		sdb[x].shield.capacitor[0],
		sdb[x].shield.capacitor[1],
		sdb[x].shield.capacitor[2],
		sdb[x].shield.capacitor[3],
		sdb[x].shield.capacitor[4],
		sdb[x].shield.capacitor[5]);

	result = atr_add(ship, SHIELD_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write SHIELD attribute.");
		return 0;
	}

/* TECHNOLOGY */

	snprintf(buffer, sizeof(buffer), "%f %f %f %f %f %f %f %f %f",
		sdb[x].tech.firing,
		sdb[x].tech.fuel,
		sdb[x].tech.stealth,
		sdb[x].tech.cloak,
		sdb[x].tech.sensors,
		sdb[x].tech.aux_max,
		sdb[x].tech.main_max,
		sdb[x].tech.armor,
		sdb[x].tech.ly_range);
	result = atr_add(ship, TECHNOLOGY_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write TECHNOLOGY attribute.");
		return 0;
	}

/* MOVEMENT */

	snprintf(buffer, sizeof(buffer), "%d %d %f %f %f %f %f %d %d",
		sdb[x].move.time,
		sdb[x].move.dt,
		sdb[x].move.in,
		sdb[x].move.out,
		sdb[x].move.ratio,
		sdb[x].move.cochranes,
		sdb[x].move.v,
		sdb[x].move.empire,
		sdb[x].move.quadrant);
	result = atr_add(ship, MOVEMENT_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MOVEMENT attribute.");
		return 0;
	}

/* CLOAK */

	snprintf(buffer, sizeof(buffer), "%d %d %f %d %d %f",
		sdb[x].cloak.version,
		sdb[x].cloak.cost,
		sdb[x].cloak.freq,
		sdb[x].cloak.exist,
		sdb[x].cloak.active,
		sdb[x].cloak.damage);
	result = atr_add(ship, CLOAK_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write CLOAK attribute.");
		return 0;
	}

/* TRANS */

	snprintf(buffer, sizeof(buffer), "%d %f %d %d %f %d %d",
		sdb[x].trans.cost,
		sdb[x].trans.freq,
		sdb[x].trans.exist,
		sdb[x].trans.active,
		sdb[x].trans.damage,
		sdb[x].trans.d_lock,
		sdb[x].trans.s_lock);
	result = atr_add(ship, TRANS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write TRANS attribute.");
		return 0;
	}

/* TRACT */

	snprintf(buffer, sizeof(buffer), "%d %f %d %d %f %d",
		sdb[x].tract.cost,
		sdb[x].tract.freq,
		sdb[x].tract.exist,
		sdb[x].tract.active,
		sdb[x].tract.damage ,
		sdb[x].tract.lock);
	result = atr_add(ship, TRACT_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write TRACT attribute.");
		return 0;
	}

/* COORDS */

	snprintf(buffer, sizeof(buffer), "%f %f %f %f %f %f %f %f %f",
		sdb[x].coords.x,
		sdb[x].coords.y,
		sdb[x].coords.z,
		sdb[x].coords.xo,
		sdb[x].coords.yo,
		sdb[x].coords.zo,
		sdb[x].coords.xd,
		sdb[x].coords.yd,
		sdb[x].coords.zd);
	result = atr_add(ship, COORDS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write COORDS attribute.");
		return 0;
	}

/* COURSE */

	snprintf(buffer, sizeof(buffer), "%d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
		sdb[x].course.version,
		sdb[x].course.yaw_in,
		sdb[x].course.yaw_out,
		sdb[x].course.pitch_in,
		sdb[x].course.pitch_out,
		sdb[x].course.roll_in,
		sdb[x].course.roll_out,
		sdb[x].course.d[0][0],
		sdb[x].course.d[0][1],
		sdb[x].course.d[0][2],
		sdb[x].course.d[1][0],
		sdb[x].course.d[1][1],
		sdb[x].course.d[1][2],
		sdb[x].course.d[2][0],
		sdb[x].course.d[2][1],
		sdb[x].course.d[2][2],
		sdb[x].course.rate);
	result = atr_add(ship, COURSE_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write COURSE attribute.");
		return 0;
	}

/* MAIN */

	snprintf(buffer, sizeof(buffer), "%f %f %f %f %d",
		sdb[x].main.in,
		sdb[x].main.out,
		sdb[x].main.damage,
		sdb[x].main.gw,
		sdb[x].main.exist);
	result = atr_add(ship, MAIN_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write MAIN attribute.");
		return 0;
	}

/* AUX */

	snprintf(buffer, sizeof(buffer), "%f %f %f %f %d",
		sdb[x].aux.in,
		sdb[x].aux.out,
		sdb[x].aux.damage,
		sdb[x].aux.gw,
		sdb[x].aux.exist);
	result = atr_add(ship, AUX_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write AUX attribute.");
		return 0;
	}

/* BATT */

	snprintf(buffer, sizeof(buffer), "%f %f %f %f %d",
		sdb[x].batt.in,
		sdb[x].batt.out,
		sdb[x].batt.damage,
		sdb[x].batt.gw,
		sdb[x].batt.exist);
	result = atr_add(ship, BATT_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write BATT attribute.");
		return 0;
	}

/* FUEL */

	snprintf(buffer, sizeof(buffer), "%f %f %f",
			sdb[x].fuel.antimatter,
			sdb[x].fuel.deuterium,
			sdb[x].fuel.reserves);
	result = atr_add(ship, FUEL_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write FUEL attribute.");
		return 0;
	}

/* STATUS */

	snprintf(buffer, sizeof(buffer), "%d %d %d %d %d %d %d %d %d %d",
		sdb[x].status.active,
		sdb[x].status.docked,
		sdb[x].status.landed,
		sdb[x].status.connected,
		sdb[x].status.crippled,
 		sdb[x].status.tractoring,
		sdb[x].status.tractored,
		sdb[x].status.open_landing,
 		sdb[x].status.open_docking,
		sdb[x].status.link);
	result = atr_add(ship, STATUS_ATTR_NAME, buffer, GOD, (AF_MDARK + AF_WIZARD + AF_NOPROG));
	if (result != 0) {
		write_spacelog(executor, ship, "WRITE: unable to write STATUS attribute.");
		return 0;
	}

	return 1;
}

/* ------------------------------------------------------------------------ */

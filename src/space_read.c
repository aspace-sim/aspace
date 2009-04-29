/* space_read.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

int do_space_db_read (dbref ship, dbref executor)
{
	ATTR *a;
	char **array;
	register int i, x;
	int result;
	static char buffer[10];
	//dbref object;

/* SDB */

	x = 0;
	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
		if (sdb[i].object == ship) {
			x = i;
			break;
		}
	if (!GoodSDB(x)) {
		result = get_empty_sdb();
		if (result == VACANCY_FAIL) {
			write_spacelog(executor, ship, "READ: unable find empty SDB slot.");
			return 0;
		} else
			x = result;
	}
	snprintf(buffer, sizeof(buffer), "%d", x);
	result = atr_add(ship, SDB_ATTR_NAME, buffer, GOD, AF_MDARK|AF_WIZARD|AF_NOPROG);
	if (result != 0) {
		write_spacelog(executor, ship, "READ: unable to write SDB attribute.");
		return 0;
	} else if (max_space_objects < x)
		max_space_objects = x;

/* OBJECT */

	if (!SpaceObj(ship) || !GoodObject(ship)) {
		write_spacelog(executor, ship, "READ: unable to validate SPACE_OBJECT.");
		return 0;
	} else
		sdb[x].object = ship;

/* SPACE */

	a = atr_get(ship, SPACE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read SPACE attribute.");
		return 0;
	} else {
		sdb[x].space = parse_integer(atr_value(a));
	}

/* ALLOCATE */

	a = atr_get(ship, ALLOCATE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read ALLOCATE attribute.");
		return 0;
	}
	
	array = mush_calloc(ALLOCATE_DATA_NUMBER + 1, sizeof(char *), "arrayarray");
	result = list2arr(array, ALLOCATE_DATA_NUMBER + 1, atr_value(a), ' ');
	
	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Crack ALLOCATE Attribute.");
		return 0;
	}

        if (result != ALLOCATE_DATA_NUMBER) {
                write_spacelog(executor, ship, "READ: Unable to Crack ALLOCATE Attribute Format.");
                return 0;
        }

	result += convert_long(array[0], 0, &sdb[x].alloc.version);
	result += convert_double(array[1], 0.0, &sdb[x].alloc.helm);
	result += convert_double(array[2], 0.0, &sdb[x].alloc.tactical);
	result += convert_double(array[3], 0.0, &sdb[x].alloc.operations);
	result += convert_double(array[4], 0.0, &sdb[x].alloc.movement);
	result += convert_double(array[5], 0.0, &sdb[x].alloc.shields);
	result += convert_double(array[6], 0.0, &sdb[x].alloc.shield[0]);
	result += convert_double(array[7], 0.0, &sdb[x].alloc.shield[1]);
	result += convert_double(array[8], 0.0, &sdb[x].alloc.shield[2]);
	result += convert_double(array[9], 0.0, &sdb[x].alloc.shield[3]);
	result += convert_double(array[10], 0.0, &sdb[x].alloc.shield[4]);
	result += convert_double(array[11], 0.0, &sdb[x].alloc.shield[5]);
	result += convert_double(array[12], 0.0, &sdb[x].alloc.cloak);
	result += convert_double(array[13], 0.0, &sdb[x].alloc.beams);
	result += convert_double(array[14], 0.0, &sdb[x].alloc.missiles);
	result += convert_double(array[15], 0.0, &sdb[x].alloc.sensors);
	result += convert_double(array[16], 0.0, &sdb[x].alloc.ecm);
	result += convert_double(array[17], 0.0, &sdb[x].alloc.eccm);
	result += convert_double(array[18], 0.0, &sdb[x].alloc.transporters);
	result += convert_double(array[19], 0.0, &sdb[x].alloc.tractors);
	result += convert_double(array[20], 0.0, &sdb[x].alloc.miscellaneous);
	
	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Convert ALLOCATE Attribute.");
		return 0;
	}

        if (result != ALLOCATE_DATA_NUMBER) {
                write_spacelog(executor, ship, "READ: Unable to Convert ALLOCATE Attribute Format.");
                return 0;
        }

	/* BEAM */

	a = atr_get(ship, BEAM_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read BEAM attribute.");
		return 0;
	}

	array = mush_calloc(BEAM_DATA_NUMBER + 1, sizeof(char *), "arrayarray");
	result = list2arr(array, BEAM_DATA_NUMBER + 1, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Crack BEAM Attribute.");
		return 0;
	}

        if (result != BEAM_DATA_NUMBER) {
                write_spacelog(executor, ship, "READ: Unable to Crack BEAM Attribute Format.");
                return 0;
        }

	result += convert_double(array[0], 0.0, &sdb[x].beam.in);
	result += convert_double(array[1], 0.0, &sdb[x].beam.out);
	result += convert_double(array[2], 0.0, &sdb[x].beam.freq);
	result += convert_long(array[3], 0, &sdb[x].beam.exist);
	if (sdb[x].beam.exist) {
		result += convert_long(array[4], 0, &sdb[x].beam.banks);
	} else {
		sdb[x].beam.banks = 0;
                write_spacelog(executor, ship, "READ: BEAMS Do Not Exist, Beam Banks Value Defaulting to 0.");  
	}
	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Convert BEAM Attribute.");
		return 0;
	} else if (result != BEAM_DATA_NUMBER) {
		write_spacelog(executor, ship, "READ: Unable to Convert BEAM Attribute Format.");
		return 0;
	} else {
		if (sdb[x].beam.banks > MAX_BEAM_BANKS)
			sdb[x].beam.banks = MAX_BEAM_BANKS;
		if (sdb[x].beam.banks <= 0) {
			sdb[x].beam.banks = 0;
			sdb[x].beam.exist = 0;
		}
	}

	/* BEAM_ACTIVE */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_ACTIVE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_ACTIVE Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_ACTIVE Attribute.");
			return 0;
		}
		if  (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_ACTIVE Attribute Format.");
			return 0;
		}
		
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.active[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_ACTIVE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_Active Attribute Format.");
			return 0;
		}
	}
	
	/* BEAM_NAME */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_NAME_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_NAME Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_NAME Attribute.");
			return 0;
		}

		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_NAME Attribute Format.");
			return 0;
		}

		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.name[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_NAME Attribute.");
			return 0;
		}

		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_NAME Attribute Format.");
			return 0;
		}
	}
	
	/* BEAM_DAMAGE */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_DAMAGE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_DAMAGE Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_DAMAGE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_DAMAGE Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_double(array[i], 0.0, &sdb[x].blist.damage[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_DAMAGE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_DAMAGE Attribute Format.");
			return 0;
		}
	}

/* BEAM_BONUS */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_BONUS_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_BONUS Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_BONUS Attribute.");
			return 0;
		}

		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ:  Unable to Crack BEAM_BONUS Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.bonus[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_BONUS Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_BONUS Attribute Format.");
			return 0;
		}
	}

/* BEAM_COST */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_COST_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_COST Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_COST Attribute.");
			return 0;
		}

		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_COST Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.cost[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_COST Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_COST Attribute Format.");
			return 0;
		}
	}

/* BEAM_RANGE */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_RANGE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_RANGE Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_RANGE Attribute.");
			return 0;
		}
		if (result !=sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_RANGE Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.range[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_RANGE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_RANGE Attribute Format.");
			return 0;
		}
	}

/* BEAM_ARCS */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_ARCS_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_ARCS Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_ARCS Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_ARCS Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.arcs[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_ARCS Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_ARCS Attribute Format.");
			return 0;
		}
	}

/* BEAM_LOCK */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_LOCK_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_LOCK Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_LOCK Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_LOCK Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.lock[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_LOCK Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_LOCK Attribute Format.");
			return 0;
		}
	}

/* BEAM_LOAD */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_LOAD_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_LOAD Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_LOAD Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_LOAD Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.load[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_LOAD Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_LOAD Attribute Format.");
			return 0;
		}
	}

/* BEAM_RECYCLE */

	if (sdb[x].beam.exist) {
		a = atr_get(ship, BEAM_RECYCLE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: Unable to Read BEAM_RECYCLE Attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].beam.banks + 1, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].beam.banks + 1, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_RECYCLE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Crack BEAM_RECYCLE Attribute Format.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].beam.banks ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].blist.recycle[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_RECYCLE Attribute.");
			return 0;
		}
		if (result != sdb[x].beam.banks) {
			write_spacelog(executor, ship, "READ: Unable to Convert BEAM_RECYCLE Attribute Format.");
			return 0;
		}
	}

/* ISSILE */

	a = atr_get(ship, MISSILE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: Unable to Read MISSILE Attribute.");
		return 0;
	}	
	
	array = mush_calloc(MISSILE_DATA_NUMBER + 1, sizeof(char *), "arrayarray");
	result = list2arr(array, MISSILE_DATA_NUMBER + 1, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Crack MISSILE Attribute.");
		return 0;
	}
	if (result != MISSILE_DATA_NUMBER) {
		write_spacelog(executor, ship, "READ: Unable to Crack MISSILE Attribute Format.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].missile.in);
	result += convert_double(array[1], 0.0, &sdb[x].missile.out);
	result += convert_double(array[2], 0.0, &sdb[x].missile.freq);
	result += convert_long(array[3], 0, &sdb[x].missile.exist);
	if (sdb[x].missile.exist) {
		result += convert_long(array[4], 0, &sdb[x].missile.tubes);
	} else {
		sdb[x].missile.tubes = 0;
		write_spacelog(executor, ship, "READ: MISSILES Do Not Exist, Missile Tubes Value Defaulting to 0.");
	}
	if (result == 0) {
		write_spacelog(executor, ship, "READ: Unable to Convert MISSILE Attribute.");
		return 0;
	} else if (result != MISSILE_DATA_NUMBER) {
		write_spacelog(executor, ship, "READ: Unable to Convert MISSILE Attribute Format.");
		return 0;
	} else {
		if (sdb[x].missile.tubes > MAX_MISSILE_TUBES)
			sdb[x].missile.tubes = MAX_MISSILE_TUBES;
		if (sdb[x].missile.tubes <= 0) {
			sdb[x].missile.tubes = 0;
			sdb[x].missile.exist = 0;
		}
	}
	
	/* MISSILE_ACTIVE */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_ACTIVE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_ACTIVE attribute.");
			return 0;
		}	
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_ACTIVE attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.active[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_ACTIVE attribute.");
			return 0;
		}
	}
	
	/* MISSILE_NAME */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_NAME_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_NAME attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_NAME attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.name[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_NAME attribute.");
			return 0;
		}
	}

/* MISSILE_DAMAGE */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_DAMAGE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_DAMAGE attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_DAMAGE attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_double(array[i], 0.0, &sdb[x].mlist.damage[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_DAMAGE attribute.");
			return 0;
		}
	}

/* MISSILE_WARHEAD */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_WARHEAD_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_WARHEAD attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_WARHEAD attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.warhead[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_WARHEAD attribute.");
			return 0;
		}
	}

/* MISSILE_COST */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_COST_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_COST attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_COST attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.cost[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_COST attribute.");
			return 0;
		}
	}

/* MISSILE_RANGE */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_RANGE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_RANGE attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_RANGE attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.range[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_RANGE attribute.");
			return 0;
		}
	}

/* MISSILE_ARCS */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_ARCS_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_ARCS attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_ARCS attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.arcs[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_ARCS attribute.");
			return 0;
		}
	}

/* MISSILE_LOCK */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_LOCK_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_LOCK attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_LOCK attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.lock[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_LOCK attribute.");
			return 0;
		}
	}

/* MISSILE_LOAD */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_LOAD_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_LOAD attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_LOAD attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.load[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_LOAD attribute.");
			return 0;
		}
	}

/* MISSILE_RECYCLE */

	if (sdb[x].missile.exist) {
		a = atr_get(ship, MISSILE_RECYCLE_ATTR_NAME);
		if (a == NULL) {
			write_spacelog(executor, ship, "READ: unable to read MISSILE_RECYCLE attribute.");
			return 0;
		}
		
		array = mush_calloc(sdb[x].missile.tubes, sizeof(char *), "arrayarray");
		result = list2arr(array, sdb[x].missile.tubes, atr_value(a), ' ');

		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to crack MISSILE_RECYCLE attribute.");
			return 0;
		}
		for (i = 0 ; i < sdb[x].missile.tubes ; ++i) {
			result += convert_long(array[i], 0, &sdb[x].mlist.recycle[i]);
		}
		if (result == 0) {
			write_spacelog(executor, ship, "READ: unable to convert MISSILE_RECYCLE attribute.");
			return 0;
		}
	}
	
	/* ENGINE */

 	a = atr_get(ship, ENGINE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read ENGINE attribute.");
		return 0;
	}
	
	array = mush_calloc(ENGINE_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, ENGINE_DATA_NUMBER, atr_value(a), ' ');
	
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack ENGINE attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].engine.version);
	result += convert_double(array[1], 0.0, &sdb[x].engine.warp_damage);
	result += convert_double(array[2], 0.0, &sdb[x].engine.warp_max);
	result += convert_long(array[3], 0, &sdb[x].engine.warp_exist);
	result += convert_double(array[4], 0.0, &sdb[x].engine.impulse_damage);
	result += convert_double(array[5], 0.0, &sdb[x].engine.impulse_max);
	result += convert_long(array[6], 0, &sdb[x].engine.impulse_exist);
	result += convert_double(array[7], 0.0, &sdb[x].engine.warp_cruise);
	result += convert_double(array[8], 0.0, &sdb[x].engine.impulse_cruise);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert ENGINE attribute.");
		return 0;
	}
	sdb[x].engine.version = 1;
	
	/* STRUCTURE */

 	a = atr_get(ship, STRUCTURE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read STRUCTURE attribute.");
		return 0;
	}
	array = mush_calloc(STRUCTURE_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, STRUCTURE_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack STRUCTURE attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].structure.type);
	result += convert_double(array[1], 0.0, &sdb[x].structure.displacement);
	result += convert_double(array[2], 0.0, &sdb[x].structure.cargo_hold);
	result += convert_double(array[3], 0.0, &sdb[x].structure.cargo_mass);
	result += convert_double(array[4], 0.0, &sdb[x].structure.superstructure);
	result += convert_long(array[5], 0, &sdb[x].structure.max_structure);
	result += convert_long(array[6], 0, &sdb[x].structure.has_landing_pad);
	result += convert_long(array[7], 0, &sdb[x].structure.has_docking_bay);
	result += convert_long(array[8], 0, &sdb[x].structure.can_land);
	result += convert_long(array[9], 0, &sdb[x].structure.can_dock);
	result += convert_double(array[10], 0.0, &sdb[x].structure.repair);
	result += convert_long(array[11], 0, &sdb[x].structure.max_repair);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert STRUCTURE attribute.");
		return 0;
	}
	
	/* POWER */

 	a = atr_get(ship, POWER_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read POWER attribute.");
		return 0;
	}
	array = mush_calloc(POWER_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, POWER_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack POWER attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].power.version);
	result += convert_double(array[1], 0.0, &sdb[x].power.main);
	result += convert_double(array[2], 0.0, &sdb[x].power.aux);
	result += convert_double(array[3], 0.0, &sdb[x].power.batt);
	result += convert_double(array[4], 0.0, &sdb[x].power.total);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert POWER attribute.");
		return 0;
	}

/* SENSOR */

 	a = atr_get(ship, SENSOR_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read SENSOR attribute.");
		return 0;
	}
	array = mush_calloc(SENSOR_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, SENSOR_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack SENSOR attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].sensor.version);
	result += convert_double(array[1], 0.0, &sdb[x].sensor.lrs_damage);
	result += convert_long(array[2], 0, &sdb[x].sensor.lrs_active);
	result += convert_long(array[3], 0, &sdb[x].sensor.lrs_exist);
	result += convert_double(array[4], 0.0, &sdb[x].sensor.lrs_resolution);
	result += convert_double(array[5], 0.0, &sdb[x].sensor.lrs_signature);
	result += convert_double(array[6], 0.0, &sdb[x].sensor.srs_damage);
	result += convert_long(array[7], 0, &sdb[x].sensor.srs_active);
	result += convert_long(array[8], 0, &sdb[x].sensor.srs_exist);
	result += convert_double(array[9], 0.0, &sdb[x].sensor.srs_resolution);
	result += convert_double(array[10], 0.0, &sdb[x].sensor.srs_signature);
	result += convert_double(array[11], 0.0, &sdb[x].sensor.ew_damage);
	result += convert_long(array[12], 0, &sdb[x].sensor.ew_active);
	result += convert_long(array[13], 0, &sdb[x].sensor.ew_exist);
	result += convert_double(array[14], 0.0, &sdb[x].sensor.visibility);
	result += convert_long(array[15], 0, &sdb[x].sensor.contacts);
	result += convert_long(array[16], 0, &sdb[x].sensor.counter);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert SENSOR attribute.");
		return 0;
	}
	sdb[x].sensor.contacts = 0;
	sdb[x].sensor.counter = 0;

/* SHIELD */

        a = atr_get(ship, SHIELD_ATTR_NAME);
        if (a == NULL) {
                write_spacelog(executor, ship, "READ: unable to read SHIELD attribute.");
                return 0;
        }
        array = mush_calloc(SHIELD_DATA_NUMBER + 1, sizeof(char *), "arrayarray");
        result = list2arr(array, SHIELD_DATA_NUMBER + 1, atr_value(a), ' ');

        if (result == 0) {
                write_spacelog(executor, ship, "READ: Unable to Crack SHIELD Attribute.");
                return 0;
        }

        if (result != SHIELD_DATA_NUMBER) {
	        write_spacelog(executor, ship, "READ: Unable to Crack SHIELD Attribute Format.");
		return 0;
	}

        result += convert_double(array[0], 0.0, &sdb[x].shield.ratio);
        result += convert_long(array[1], 0, &sdb[x].shield.maximum);
        result += convert_double(array[2], 0.0, &sdb[x].shield.freq);
        result += convert_long(array[3], 0, &sdb[x].shield.exist);
        result += convert_long(array[4], 0, &sdb[x].shield.active[0]);
        result += convert_long(array[5], 0, &sdb[x].shield.active[1]);
        result += convert_long(array[6], 0, &sdb[x].shield.active[2]);
        result += convert_long(array[7], 0, &sdb[x].shield.active[3]);
        result += convert_long(array[8], 0, &sdb[x].shield.active[4]);
        result += convert_long(array[9], 0, &sdb[x].shield.active[5]);
        result += convert_double(array[10], 0.0, &sdb[x].shield.damage[0]);
        result += convert_double(array[11], 0.0, &sdb[x].shield.damage[1]);
        result += convert_double(array[12], 0.0, &sdb[x].shield.damage[2]);
        result += convert_double(array[13], 0.0, &sdb[x].shield.damage[3]);
        result += convert_double(array[14], 0.0, &sdb[x].shield.damage[4]);
        result += convert_double(array[15], 0.0, &sdb[x].shield.damage[5]);
        if (result == 0) {
                write_spacelog(executor, ship, "READ: Unable to Convert SHIELD Attribute.");
                return 0;
        }
        
	if (result != SHIELD_DATA_NUMBER) {
		write_spacelog(executor, ship, "READ: Unable to Convert SHIELD Attribute Format.");
		return 0;
	}
	
/* TECHNOLOGY */

 	a = atr_get(ship, TECHNOLOGY_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read TECHNOLOGY attribute.");
		return 0;
	}
	array = mush_calloc(TECHNOLOGY_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, TECHNOLOGY_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack TECHNOLOGY attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].tech.firing);
	result += convert_double(array[1], 0.0, &sdb[x].tech.fuel);
	result += convert_double(array[2], 0.0, &sdb[x].tech.stealth);
	result += convert_double(array[3], 0.0, &sdb[x].tech.cloak);
	result += convert_double(array[4], 0.0, &sdb[x].tech.sensors);
	result += convert_double(array[5], 0.0, &sdb[x].tech.aux_max);
	result += convert_double(array[6], 0.0, &sdb[x].tech.main_max);
	result += convert_double(array[7], 0.0, &sdb[x].tech.armor);
	result += convert_double(array[8], 0.0, &sdb[x].tech.ly_range);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert TECHNOLOGY attribute.");
		return 0;
	}

/* MOVEMENT */

 	a = atr_get(ship, MOVEMENT_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read MOVEMENT attribute.");
		return 0;
	}
	array = mush_calloc(MOVEMENT_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, MOVEMENT_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack MOVEMENT attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].move.time);
	result += convert_long(array[1], 0, &sdb[x].move.dt);
	result += convert_double(array[2], 0.0, &sdb[x].move.in);
	result += convert_double(array[3], 0.0, &sdb[x].move.out);
	result += convert_double(array[4], 0.0, &sdb[x].move.ratio);
	result += convert_double(array[5], 0.0, &sdb[x].move.cochranes);
	result += convert_double(array[6], 0.0, &sdb[x].move.v);
	result += convert_long(array[7], 0, &sdb[x].move.empire);
	result += convert_long(array[8], 0, &sdb[x].move.quadrant);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert MOVEMENT attribute.");
		return 0;
	}

/* CLOAK */

 	a = atr_get(ship, CLOAK_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read CLOAK attribute.");
		return 0;
	}
	array = mush_calloc(CLOAK_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, CLOAK_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack CLOAK attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].cloak.version);
	result += convert_long(array[1], 0, &sdb[x].cloak.cost);
	result += convert_double(array[2], 0.0, &sdb[x].cloak.freq);
	result += convert_long(array[3], 0, &sdb[x].cloak.exist);
	result += convert_long(array[4], 0, &sdb[x].cloak.active);
	result += convert_double(array[5], 0.0, &sdb[x].cloak.damage);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert CLOAK attribute.");
		return 0;
	}

/* TRANS */

 	a = atr_get(ship, TRANS_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read TRANS attribute.");
		return 0;
	}
	array = mush_calloc(TRANS_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, TRANS_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack TRANS attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].trans.cost);
	result += convert_double(array[1], 0.0, &sdb[x].trans.freq);
	result += convert_long(array[2], 0, &sdb[x].trans.exist);
	result += convert_long(array[3], 0, &sdb[x].trans.active);
	result += convert_double(array[4], 0.0, &sdb[x].trans.damage);
	result += convert_long(array[5], 0, &sdb[x].trans.d_lock);
	result += convert_long(array[5], 0, &sdb[x].trans.s_lock);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert TRANS attribute.");
		return 0;
	}

/* TRACT */

 	a = atr_get(ship, TRACT_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read TRACT attribute.");
		return 0;
	}
	array = mush_calloc(TRACT_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, TRACT_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack TRACT attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].tract.cost);
	result += convert_double(array[1], 0.0, &sdb[x].tract.freq);
	result += convert_long(array[2], 0, &sdb[x].tract.exist);
	result += convert_long(array[3], 0, &sdb[x].tract.active);
	result += convert_double(array[4], 0.0, &sdb[x].tract.damage);
	result += convert_long(array[5], 0, &sdb[x].tract.lock);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert TRACT attribute.");
		return 0;
	}

/* COORDS */

 	a = atr_get(ship, COORDS_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read COORDS attribute.");
		return 0;
	}
	array = mush_calloc(COORDS_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, COORDS_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack COORDS attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].coords.x);
	result += convert_double(array[1], 0.0, &sdb[x].coords.y);
	result += convert_double(array[2], 0.0, &sdb[x].coords.z);
	result += convert_double(array[3], 0.0, &sdb[x].coords.xo);
	result += convert_double(array[4], 0.0, &sdb[x].coords.yo);
	result += convert_double(array[5], 0.0, &sdb[x].coords.zo);
	result += convert_double(array[6], 0.0, &sdb[x].coords.xd);
	result += convert_double(array[7], 0.0, &sdb[x].coords.yd);
	result += convert_double(array[8], 0.0, &sdb[x].coords.zd);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert COORDS attribute.");
		return 0;
	}

/* COURSE */

 	a = atr_get(ship, COURSE_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read COURSE attribute.");
		return 0;
	}
	array = mush_calloc(COURSE_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, COURSE_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack COURSE attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].course.version);
	result += convert_double(array[1], 0.0, &sdb[x].course.yaw_in);
	result += convert_double(array[2], 0.0, &sdb[x].course.yaw_out);
	result += convert_double(array[3], 0.0, &sdb[x].course.pitch_in);
	result += convert_double(array[4], 0.0, &sdb[x].course.pitch_out);
	result += convert_double(array[5], 0.0, &sdb[x].course.roll_in);
	result += convert_double(array[6], 0.0, &sdb[x].course.roll_out);
	result += convert_double(array[7], 0.0, &sdb[x].course.d[0][0]);
	result += convert_double(array[8], 0.0, &sdb[x].course.d[0][1]);
	result += convert_double(array[9], 0.0, &sdb[x].course.d[0][2]);
	result += convert_double(array[10], 0.0, &sdb[x].course.d[1][0]);
	result += convert_double(array[11], 0.0, &sdb[x].course.d[1][1]);
	result += convert_double(array[12], 0.0, &sdb[x].course.d[1][2]);
	result += convert_double(array[13], 0.0, &sdb[x].course.d[2][0]);
	result += convert_double(array[14], 0.0, &sdb[x].course.d[2][1]);
	result += convert_double(array[15], 0.0, &sdb[x].course.d[2][2]);
	result += convert_double(array[16], 0.0, &sdb[x].course.rate);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert COURSE attribute.");
		return 0;
	}

/* MAIN */

 	a = atr_get(ship, MAIN_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read MAIN attribute.");
		return 0;
	}
	array = mush_calloc(MAIN_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, MAIN_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack MAIN attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].main.in);
	result += convert_double(array[1], 0.0, &sdb[x].main.out);
	result += convert_double(array[2], 0.0, &sdb[x].main.damage);
	result += convert_double(array[3], 0.0, &sdb[x].main.gw);
	result += convert_long(array[4], 0, &sdb[x].main.exist);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert MAIN attribute.");
		return 0;
	}

/* AUX */

 	a = atr_get(ship, AUX_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read AUX attribute.");
		return 0;
	}
	array = mush_calloc(AUX_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, AUX_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack AUX attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].aux.in);
	result += convert_double(array[1], 0.0, &sdb[x].aux.out);
	result += convert_double(array[2], 0.0, &sdb[x].aux.damage);
	result += convert_double(array[3], 0.0, &sdb[x].aux.gw);
	result += convert_long(array[4], 0, &sdb[x].aux.exist);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert AUX attribute.");
		return 0;
	}

/* BATT */

 	a = atr_get(ship, BATT_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read BATT attribute.");
		return 0;
	}
	array = mush_calloc(BATT_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, BATT_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack BATT attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].batt.in);
	result += convert_double(array[1], 0.0, &sdb[x].batt.out);
	result += convert_double(array[2], 0.0, &sdb[x].batt.damage);
	result += convert_double(array[3], 0.0, &sdb[x].batt.gw);
	result += convert_long(array[4], 0, &sdb[x].batt.exist);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert BATT attribute.");
		return 0;
	}

/* FUEL */

 	a = atr_get(ship, FUEL_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read FUEL attribute.");
		return 0;
	}
	array = mush_calloc(FUEL_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, FUEL_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack FUEL attribute.");
		return 0;
	}
	result += convert_double(array[0], 0.0, &sdb[x].fuel.antimatter);
	result += convert_double(array[1], 0.0, &sdb[x].fuel.deuterium);
	result += convert_double(array[2], 0.0, &sdb[x].fuel.reserves);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert FUEL attribute.");
		return 0;
	}

/* STATUS */

 	a = atr_get(ship, STATUS_ATTR_NAME);
	if (a == NULL) {
		write_spacelog(executor, ship, "READ: unable to read STATUS attribute.");
		return 0;
	}
	array = mush_calloc(STATUS_DATA_NUMBER, sizeof(char *), "arrayarray");
	result = list2arr(array, STATUS_DATA_NUMBER, atr_value(a), ' ');

	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to crack STATUS attribute.");
		return 0;
	}
	result += convert_long(array[0], 0, &sdb[x].status.active);
	result += convert_long(array[1], 0, &sdb[x].status.docked);
	result += convert_long(array[2], 0, &sdb[x].status.landed);
	result += convert_long(array[3], 0, &sdb[x].status.connected);
	result += convert_long(array[4], 0, &sdb[x].status.crippled);
	result += convert_long(array[5], 0, &sdb[x].status.tractoring);
	result += convert_long(array[6], 0, &sdb[x].status.tractored);
	result += convert_long(array[7], 0, &sdb[x].status.open_landing);
	result += convert_long(array[8], 0, &sdb[x].status.open_docking);
	result += convert_long(array[9], 0, &sdb[x].status.link);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: unable to convert STATUS attribute.");
		return 0;
	}
	sdb[x].status.time = sdb[x].move.time;
	
/* DEBUGGING */

	result = debug_space(x);
	if (result == 0) {
		write_spacelog(executor, ship, "READ: Bugs found and corrected.");
	}

	return 1;
}

/* -------------------------------------------------------------------- */

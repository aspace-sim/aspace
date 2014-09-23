/* space_do_set.c */

#include "config.h"
#include "space.h"

struct db_stat_info current_state;

/*----------------------------------------------------------------------------*/

int do_set_fire (int first, int last, int weapon, int mode, dbref enactor)
{
	static char buff_n[1000], buff_x[MAX_SENSOR_CONTACTS][1000];
	int a, b, i, j, k, x = 0, flag = 0;
	int is_b_active = 0, is_b_lock = 0, is_b_load = 0, is_b_arm = 0, is_b_arc = 0, is_b_range = 0;
	int is_m_active = 0, is_m_lock = 0, is_m_load = 0, is_m_arm = 0, is_m_arc = 0, is_m_range = 0;
	double range, prob, dmg_b[MAX_BEAM_BANKS], dmg_m[MAX_MISSILE_TUBES];
	double ss, pdmg, d_system[15], d_shield[MAX_SHIELD_NAME];
	//double dis;
	double d_beam[MAX_BEAM_BANKS], d_missile[MAX_MISSILE_TUBES];

	if (error_on_console(enactor)) {
		return 0;
	} else if (weapon < 0 || weapon > 2 || mode < 0 || mode > 6) {
		return 0;
	} else if (weapon == 1 && !sdb[n].beam.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[3])));
		return 0;
	} else if (weapon == 2 && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[9])));
		return 0;
	} else if (!weapon && !sdb[n].beam.exist && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no weapons.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot fire weapons while cloaked.", Name(sdb[n].object))));
		return 0;
	}

	strncpy(buff_n, "", sizeof(buff_n) - 1);
	for (i = 0; i < sdb[n].sensor.contacts; ++i)
		strncpy(buff_x[i], "", sizeof(buff_x[i]) - 1);

	/* check beam weapon list */

	if (weapon == 1 || (!weapon && sdb[n].beam.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].beam.banks) {
			a = 0;
			b = sdb[n].beam.banks - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].beam.banks)
				b = sdb[n].beam.banks - 1;
			if (b < a)
				b = a;
		}
		for (i = 0; i < sdb[n].beam.banks; ++i)
			dmg_b[i] = 0.0;
		for (i = a; i <= b ; ++i) {
			if (sdb[n].blist.damage[i] <= 0.0)
				continue;
			if (!sdb[n].blist.active[i])
				continue;
			++is_b_active;
			x = sdb[n].blist.lock[i];
			if (!GoodSDB(x))
				continue;
			if (sdb[x].tmp.i0 != n) {
			    sdb[x].tmp.i0 = n;               /* initial marker */
			    sdb[x].tmp.i1 = sdb2slist(n, x); /* slist number */
			    sdb[x].tmp.i2 = 0;               /* firing arc to target */
			    sdb[x].tmp.i3 = 0;               /* facing shield of target */
			    sdb[x].tmp.i4 = 0;               /* multiple-hit flag for target */
			    sdb[x].tmp.i5 = 0;               /* hit flag of target */
			    sdb[x].tmp.d0 = 0.0;             /* range to target & shield damage */
			    sdb[x].tmp.d1 = 0.0;             /* target shield GW */
			    sdb[x].tmp.d2 = 0.0;             /* internal damage */
			}
			if (sdb[x].tmp.i1 == SENSOR_FAIL)
				continue;
			++is_b_lock;
			if (sdb[n].beam.out < sdb[n].blist.cost[i])
				continue;
			++is_b_arm;
			if (!sdb[x].tmp.i2)
				sdb[x].tmp.i2 = sdb2arc(n, x);
			if (arc_check(sdb[x].tmp.i2, sdb[n].blist.arcs[i]) == ARC_FAIL)
				continue;
			if (sdb[n].status.tractored)
				if (sdb[n].status.tractored != x)
					continue;
			++is_b_arc;
			if (sdb[x].tmp.d0 == 0.0)
				sdb[x].tmp.d0 = sdb2range(n, x);
			if (fabs(sdb[n].move.out) < 1.0) {
				range = sdb[n].blist.range[i];
			} else
				range = sdb[n].blist.range[i] * PARSEC / 10000.0;
			if (sdb[x].tmp.d0 > range * 10.0)
				continue;
			if (fabs(sdb[n].move.out) >= 1.0 && fabs(sdb[x].move.out) < 1.0)
				if (sdb[n].status.tractoring != x && sdb[n].status.tractored != x)
					continue;
			if (fabs(sdb[n].move.out) < 1.0 && fabs(sdb[x].move.out) >= 1.0)
  				if (sdb[n].status.tractoring != x && sdb[n].status.tractored != x)
  					continue;
  			++is_b_range;
 			if (sdb[n].blist.load[i] + sdb[n].blist.recycle[i] > time(NULL))
  				continue;
  			++is_b_load;
  			sdb[n].blist.load[i] = time(NULL);
  			sdb[n].beam.out -= sdb[n].blist.cost[i];
			prob = sdb[n].slist.lev[sdb[x].tmp.i1];
			prob *= sdb[n].blist.damage[i] * sdb[n].tech.firing;
			if (sdb[x].tmp.d0 > range)
				prob *= 0.01 + 0.99 * range / sdb[x].tmp.d0;
			prob /= 1.0 + (sdb2angular(n, x) * 10.0 * (1.0 + sdb[x].move.ratio) / sdb[x].move.ratio);
			prob = (prob > 1.0) ? 1.0 : (prob < 0.01) ? 0.01 : prob;
			if (sdb[x].tmp.i4)
				strncat(buff_x[sdb[x].tmp.i1], " ", sizeof(buff_x[sdb[x].tmp.i1]) - 1);
			++sdb[x].tmp.i4;
			if (get_random_long(0,101) < prob * 100.0) {
				dmg_b[i] = (sdb[n].blist.cost[i] + sdb[n].blist.bonus[i]);
				if (sdb[x].tmp.d0 > range && sdb[n].blist.name[i] != 19) // Mass Driver Kludge - Jarek
					dmg_b[i] *= (1.0 - (sdb[x].tmp.d0 - range) / (18.0 * range));
				strncat(buff_x[sdb[x].tmp.i1], tprintf("B%d:%s%s%d%s%s",
				  i + 1, ANSI_HILITE, ANSI_RED, (int) (dmg_b[i] + 0.5),
				  ANSI_NORMAL, ANSI_CYAN), sizeof(buff_x[sdb[x].tmp.i1]) - 1);
				if (!sdb[x].tmp.i5)
					++sdb[x].tmp.i5;
			} else {
				strncat(buff_x[sdb[x].tmp.i1], tprintf("B%d:--",
				  i + 1), sizeof(buff_x[sdb[x].tmp.i1]) - 1);
			}
		}
	}

	/* check missile weapon list */

	if (weapon == 2 || (!weapon && sdb[n].missile.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].missile.tubes) {
			a = 0;
			b = sdb[n].missile.tubes - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].missile.tubes)
				b = sdb[n].missile.tubes - 1;
			if (b < a)
				b = a;
		}
		for (i = 0; i < sdb[n].missile.tubes; ++i)
			dmg_m[i] = 0.0;
		for (i = a; i <= b ; ++i) {
			if (sdb[n].mlist.damage[i] <= 0.0)
				continue;
			if (!sdb[n].mlist.active[i])
				continue;
			++is_m_active;
			x = sdb[n].mlist.lock[i];
			if (!GoodSDB(x))
				continue;
			if (sdb[x].tmp.i0 != n) {
			    sdb[x].tmp.i0 = n;               /* initial marker */
			    sdb[x].tmp.i1 = sdb2slist(n, x); /* slist number */
			    sdb[x].tmp.i2 = 0;               /* firing arc to target */
			    sdb[x].tmp.i3 = 0;               /* facing shield of target */
			    sdb[x].tmp.i4 = 0;               /* multiple-hit flag for target */
			    sdb[x].tmp.i5 = 0;               /* hit flag of target */
			    sdb[x].tmp.d0 = 0.0;             /* range to target & shield damage */
			    sdb[x].tmp.d1 = 0.0;             /* target shield GW */
			    sdb[x].tmp.d2 = 0.0;             /* internal damage */
			}
			if (sdb[x].tmp.i1 == SENSOR_FAIL)
				continue;
			++is_m_lock;
			if (sdb[n].missile.out < sdb[n].mlist.cost[i])
				continue;
			++is_m_arm;
			if (!sdb[x].tmp.i2)
				sdb[x].tmp.i2 = sdb2arc(n, x);
			if (arc_check(sdb[x].tmp.i2, sdb[n].mlist.arcs[i]) == ARC_FAIL)
				continue;
			if (sdb[n].status.tractored)
				if (sdb[n].status.tractored != x)
					continue;
			++is_m_arc;
			if (sdb[x].tmp.d0 == 0.0)
				sdb[x].tmp.d0 = sdb2range(n, x);
			if ((fabs(sdb[n].move.out) >= 1.0) || ((sdb[n].structure.type > 1) && (fabs(sdb[x].move.out) >= 1.0))) {
				range = sdb[n].mlist.range[i] * PARSEC / 10000.0;
			} else
				range = sdb[n].mlist.range[i];
			if (sdb[x].tmp.d0 > range * 10.0)
				continue;
			if (fabs(sdb[n].move.out) >= 1.0 && fabs(sdb[x].move.out) < 1.0)
				if (sdb[n].status.tractoring != x && sdb[n].status.tractored != x)
					continue;
			if (fabs(sdb[n].move.out) < 1.0 && fabs(sdb[x].move.out) >= 1.0 && sdb[n].structure.type == 1)
				if (sdb[n].status.tractoring != x && sdb[n].status.tractored != x)
					continue;
			++is_m_range;
			if (sdb[n].mlist.load[i] + sdb[n].mlist.recycle[i] > time(NULL)) 
				continue;
			++is_m_load;
			sdb[n].mlist.load[i] = time(NULL);
			sdb[n].missile.out -= sdb[n].mlist.cost[i];
			prob = sdb[n].slist.lev[sdb[x].tmp.i1];
			prob *= sdb[n].mlist.damage[i] * sdb[n].tech.firing;
			if (sdb[x].tmp.d0 > range)
				prob *= 0.01 + 0.99 * range / sdb[x].tmp.d0;
			prob /= 1.0 + (sdb2angular(n, x) * 10.0 * (1.0 + sdb[x].move.ratio) / sdb[x].move.ratio);
			prob = (prob > 1.0) ? 1.0 : (prob < 0.01) ? 0.01 : prob;
			if (sdb[x].tmp.i4)
				strncat(buff_x[sdb[x].tmp.i1], " ", sizeof(buff_x[sdb[x].tmp.i1]) - 1);
			++sdb[x].tmp.i4;
			if (get_random_long(0,101) < prob * 100.0) {
				dmg_m[i] = sdb[n].mlist.warhead[i];
				if (sdb[n].mlist.name[i] == 3) /* plasma kludge */
					if (sdb[x].tmp.d0 > range)
						dmg_m[i] *= (1.0 - (sdb[x].tmp.d0 - range) / (18.0 * range));
				strncat(buff_x[sdb[x].tmp.i1], tprintf("M%d:%s%s%d%s%s",
				  i + 1, ANSI_HILITE, ANSI_RED, (int) (dmg_m[i] + 0.5),
				  ANSI_NORMAL, ANSI_CYAN), sizeof(buff_x[sdb[x].tmp.i1]) - 1);
				if (!sdb[x].tmp.i5)
					++sdb[x].tmp.i5;
			} else {
				strncat(buff_x[sdb[x].tmp.i1], tprintf("M%d:--", i + 1), sizeof(buff_x[sdb[x].tmp.i1]) - 1);
			}
		}
	}

	/* report weapon status */

	if (!weapon && !is_b_load && !is_m_load) {
		if (!is_b_active) {
			notify(enactor, ansi_red(tprintf("No %ss are online.", system_name[3])));
		} else if (!is_b_lock) {
			notify(enactor, ansi_red(tprintf("No %ss are locked.", system_name[3])));
		} else if (!is_b_arm) {
			notify(enactor, ansi_red(tprintf("No %ss are powered.", system_name[3])));
		} else if (!is_b_arc) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in firing arc.", system_name[3])));
		} else if (!is_b_range) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in range.", system_name[3])));
		} else if (!is_b_load) {
			notify(enactor, ansi_red(tprintf("No %ss are recycled.", system_name[3])));
		}
		if (!is_m_active) {
			notify(enactor, ansi_red(tprintf("No %ss are online.", system_name[9])));
		} else if (!is_m_lock) {
			notify(enactor, ansi_red(tprintf("No %ss are locked.", system_name[9])));
		} else if (!is_m_arm) {
			notify(enactor, ansi_red(tprintf("No %ss are powered.", system_name[9])));
		} else if (!is_m_arc) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in firing arc.", system_name[9])));
		} else if (!is_m_range) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in range.", system_name[9])));
		} else if (!is_m_load) {
			notify(enactor, ansi_red(tprintf("No %ss are recycled.", system_name[9])));
		}
		for (i = 0; i < sdb[n].sensor.contacts; ++i) {
			x = sdb[n].slist.sdb[i];
			if (sdb[x].tmp.i0 == n) {
				sdb[x].tmp.i0 = 0;
				sdb[x].tmp.i1 = 0;
				sdb[x].tmp.i2 = 0;
				sdb[x].tmp.i3 = 0;
				sdb[x].tmp.i4 = 0;
				sdb[x].tmp.i5 = 0;
				sdb[x].tmp.d0 = 0.0;
				sdb[x].tmp.d1 = 0.0;
				sdb[x].tmp.d2 = 0.0;
			}
		}
		return 0;
	} else if (weapon == 1 && !is_b_load) {
		if (!is_b_active) {
			notify(enactor, ansi_red(tprintf("No %ss are online.", system_name[3])));
		} else if (!is_b_lock) {
			notify(enactor, ansi_red(tprintf("No %ss are locked.", system_name[3])));
		} else if (!is_b_arm) {
			notify(enactor, ansi_red(tprintf("No %ss are powered.", system_name[3])));
		} else if (!is_b_arc) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in firing arc.", system_name[3])));
		} else if (!is_b_range) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in range.", system_name[3])));
		} else if (!is_b_load) {
			notify(enactor, ansi_red(tprintf("No %ss are recycled.", system_name[3])));
		}
		for (i = 0; i < sdb[n].sensor.contacts; ++i) {
			x = sdb[n].slist.sdb[i];
			if (sdb[x].tmp.i0 == n) {
				sdb[x].tmp.i0 = 0;
				sdb[x].tmp.i1 = 0;
				sdb[x].tmp.i2 = 0;
				sdb[x].tmp.i3 = 0;
				sdb[x].tmp.i4 = 0;
				sdb[x].tmp.i5 = 0;
				sdb[x].tmp.d0 = 0.0;
				sdb[x].tmp.d1 = 0.0;
				sdb[x].tmp.d2 = 0.0;
			}
		}
		return 0;
	} else if (weapon == 2 && !is_m_load) {
		if (!is_m_active) {
			notify(enactor, ansi_red(tprintf("No %ss are online.", system_name[9])));
		} else if (!is_m_lock) {
			notify(enactor, ansi_red(tprintf("No %ss are locked.", system_name[9])));
		} else if (!is_m_arm) {
			notify(enactor, ansi_red(tprintf("No %ss are powered.", system_name[9])));
		} else if (!is_m_arc) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in firing arc.", system_name[9])));
		} else if (!is_m_range) {
			notify(enactor, ansi_red(tprintf("No %ss have targets in range.", system_name[9])));
		} else if (!is_m_load) {
			notify(enactor, ansi_red(tprintf("No %ss are recycled.", system_name[9])));
		}
		for (i = 0; i < sdb[n].sensor.contacts; ++i) {
			x = sdb[n].slist.sdb[i];
			if (sdb[x].tmp.i0 == n) {
				sdb[x].tmp.i0 = 0;
				sdb[x].tmp.i1 = 0;
				sdb[x].tmp.i2 = 0;
				sdb[x].tmp.i3 = 0;
				sdb[x].tmp.i4 = 0;
				sdb[x].tmp.i5 = 0;
				sdb[x].tmp.d0 = 0.0;
				sdb[x].tmp.d1 = 0.0;
				sdb[x].tmp.d2 = 0.0;
			}
		}
		return 0;
	}

	/* report firing messages */

	flag = 0;
	for (i = 0; i < sdb[n].sensor.contacts; ++i) {
		x = sdb[n].slist.sdb[i];
		if (sdb[x].tmp.i0 == n) {
			if (sdb[x].tmp.i4) {
				if (flag)
					strncat(buff_n, "\n", sizeof(buff_n) - 1);
				++flag;
				strncat(buff_n, ansi_cmd(enactor,
				  tprintf("Firing at %s: %s", unparse_identity(n, x), buff_x[i])), sizeof(buff_n) - 1);
				if (sdb[x].tmp.i5) {
					do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science,
					  "fires and hits");
					write_spacelog(enactor, sdb[n].object, tprintf("LOG: Fired and hit, Beam %.6f GHz, Missile %.6f GHz",
					  sdb[n].beam.freq, sdb[n].missile.freq));
				} else {
					do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science,
					  "fires and misses");
				}
				if (sdb[x].status.active && !sdb[x].status.crippled) {
					console_message(x, "helm science tactical",
					  ansi_warn(tprintf("%s firing: %s", unparse_identity(x, n), buff_x[i])));
					do_fed_shield_bug_check(x);
					sdb[x].tmp.i3 = sdb2shield(x, n);
					sdb[x].tmp.d1 = sdb2dissipation(x, sdb[x].tmp.i3);
				}
			}
			sdb[x].tmp.d0 = 0.0; /* shield damage */
			sdb[x].tmp.d2 = 0.0; /* ship damage */
		}
	}
	console_message(n, "helm science tactical", buff_n);

	/* compute damage */

	if (!weapon || weapon == 1.0)
		for (i = 0; i < sdb[n].beam.banks; ++i) {
			x = sdb[n].blist.lock[i];
			if (sdb[x].tmp.i0 == n)
				if (dmg_b[i] > 0.0) {
					if (sdb[x].shield.exist) {
						if (sdb[x].tmp.d1 > 0.0) {
							if (sdb[x].shield.freq == sdb[n].beam.freq) {
								pdmg = dmg_b[i];
							} else
								pdmg = 0.0;
							dmg_b[i] -= pdmg;
							if (dmg_b[i] > 0.0)
								sdb[x].tmp.d0 += dmg_b[i] / sdb[x].tmp.d1 / 10.0;
							dmg_b[i] /= sdb[x].tmp.d1;
							dmg_b[i] += pdmg;
						}
					}
					/* dmg_b[i] -= sdb[x].tech.armor; */
					/* dmg_b[i] /= sdb[x].tech.armor; */
					if (dmg_b[i] > 0.0)
						sdb[x].tmp.d2 += dmg_b[i];
				}
		}
	if (!weapon || weapon == 2.0)
		for (i = 0; i < sdb[n].missile.tubes; ++i) {
			x = sdb[n].mlist.lock[i];
			if (sdb[x].tmp.i0 == n)
				if (dmg_m[i] > 0.0) {
					if (sdb[x].shield.exist) {
						if (sdb[x].tmp.d1 > 0.0) {
							if (sdb[x].shield.freq == sdb[n].missile.freq) {
								pdmg = dmg_m[i];
							} else
								pdmg = 0.0;
							dmg_m[i] -= pdmg;
							if (dmg_m[i] > 0.0)
								sdb[x].tmp.d0 += dmg_m[i] / sdb[x].tmp.d1 / 10.0;
							dmg_m[i] /= sdb[x].tmp.d1;
							dmg_m[i] += pdmg;
						}
					}
					/* dmg_m[i] -= sdb[x].tech.armor; */
					/* dmg_m[i] /= sdb[x].tech.armor; */
					if (dmg_m[i] > 0.0)
						sdb[x].tmp.d2 += dmg_m[i];
				}
		}

        if (sdb[x].tmp.d0 > 0.0 && sdb[x].tmp.d2 > 0.0) /* This if statement added by Kaly */
        {
	
	  sdb[x].tmp.d0 += ( sdb[x].tmp.d2 / 5.0 );
	  sdb[x].tmp.d2 *= 0.75;
        }
   
	/* assess damage */

	for (i = 0; i < sdb[n].sensor.contacts; ++i) {
		x = sdb[n].slist.sdb[i];
		if (sdb[x].tmp.i0 == n) {
			for (j = 0; j < MAX_SHIELD_NAME; ++j)
				d_shield[j] = 0.0;
			for (j = 0; j < 15; ++j)
				d_system[j] = 0.0;
			for (j = 0; j < sdb[x].beam.banks; ++j)
				d_beam[j] = 0.0;
			for (j = 0; j < sdb[x].missile.tubes; ++j)
				d_missile[j] = 0.0;
			sdb[x].tmp.d2 -= sdb[x].tech.armor;	/* additive weapons */
			sdb[x].tmp.d2 /= sdb[x].tech.armor; /* additive weapons */
			if (sdb[x].tmp.d0 > 0.0) {
				d_shield[sdb[x].tmp.i3] += sdb[x].tmp.d0;
				if (sdb[x].tmp.d2 > 0.0) {
					alert_ship_hurt(x);
				} else
					alert_ship_hit(x);
			}
			if (sdb[x].tmp.d2 > 0.0) {
					if (mode == 0 || mode == 6) {
						d_system[0] += sdb[x].tmp.d2;
					} else
						d_system[0] += sdb[x].tmp.d2 / 100.0;
				k = (int) (sdb[x].tmp.d2 / 5.0 + 1.0);
				for (j = 0; j < k; ++j) {
					pdmg = get_random_long(0,500) / 100.0;
					if (mode == 0) /* normal damage */ {
						switch (get_random_long(0,10) + get_random_long(0,10) + 2) {
							case 2: d_shield[get_random_long(0,MAX_SHIELD_NAME)] += pdmg; break;
							case 3: d_system[11] += pdmg; break;
							case 4: d_system[7] += pdmg; break;
							case 5: d_system[5] += pdmg; break;
							case 6: d_system[1] += pdmg; break;
							case 7: d_system[8] += pdmg; break;
							case 8: d_missile[get_random_long(0,sdb[x].missile.tubes)] += pdmg; break;
							case 14: d_beam[get_random_long(0,sdb[x].beam.banks)] += pdmg; break;
							case 15: d_system[14] += pdmg; break;
							case 16: d_system[6] += pdmg; break;
							case 17: d_system[13] += pdmg; break;
							case 18: d_system[12] += pdmg; break;
							case 19: d_system[4] += pdmg; break;
							case 20: d_system[2] += pdmg; break;
							default: break;
						}
					} else if (mode == 2) /* engine damage */ {
						switch (get_random_long(0,10) + get_random_long(0,10) + 2) {
							case 15: d_system[14] += pdmg; break;
							case 16: d_system[6] += pdmg; break;
							default: break;
						}
					} else if (mode == 3) /* weapon damage */ {
						switch (get_random_long(0,10) + get_random_long(0,10) + 2) {
							case 2: d_shield[get_random_long(0,MAX_SHIELD_NAME)] += pdmg; break;
							case 5: d_system[5] += pdmg; break;
							case 8: d_missile[get_random_long(0,sdb[x].missile.tubes)] += pdmg; break;
							case 14: d_beam[get_random_long(0,sdb[x].beam.banks)] += pdmg; break;
							case 17: d_system[13] += pdmg; break;
							case 18: d_system[12] += pdmg; break;
							case 19: d_system[4] += pdmg; break;
							default: break;
						}
					} else if (mode == 4) /* sensor damage */ {
						switch (get_random_long(0,10) + get_random_long(0,10) + 2) {
							case 3: d_system[11] += pdmg; break;
							case 4: d_system[7] += pdmg; break;
							case 5: d_system[5] += pdmg; break;
							case 19: d_system[4] += pdmg; break;
							default: break;
						}
					} else if (mode == 5) /* power damage */ {
						switch (get_random_long(0,10) + get_random_long(0,10) + 2) {
							case 6: d_system[1] += pdmg; break;
							case 7: d_system[8] += pdmg; break;
							case 20: d_system[2] += pdmg; break;
							default: break;
						}
					}
				}
			}
			if (sdb[x].tmp.d0 > 0.0 || sdb[x].tmp.d2 > 0.0)
				if (sdb[x].shield.exist)
					for (j = 0; j < MAX_SHIELD_NAME; ++j)
						if (d_shield[j] > 0.0)
							damage_shield(x, j, d_shield[j]);
			if (sdb[x].tmp.d2 > 0.0) {
				ss = sdb[x].structure.max_structure;
				if (d_system[0] > 0.0) {
					pdmg = sdb[x].structure.superstructure;
					damage_structure(x, d_system[0]);
					if (sdb[x].structure.superstructure <= -ss) {
						atr_add(sdb[x].object, "COMMENT",
						  tprintf("%s(#%d) killed by %s(#%d) on %s(#%d) at %s",
						  Name(sdb[x].object), sdb[x].object,
						  Name(enactor), enactor,
						  Name(sdb[n].object), sdb[n].object,
						  (char *) ctime(&mudtime)), GOD,
						  (AF_MDARK + AF_WIZARD + AF_NOPROG));
						continue;
					}
					if (pdmg > 0.0 && sdb[x].structure.superstructure <= 0.0) {
						for (j = 0; j < sdb[n].beam.banks; ++j)
							if (sdb[n].blist.lock[j] == x)
								sdb[n].blist.lock[j] = 0;
						for (j = 0; j < sdb[n].missile.tubes; ++j)
							if (sdb[n].mlist.lock[j] == x)
								sdb[n].mlist.lock[j] = 0;
					}
				}
				if (d_system[1] > 0.0 && sdb[x].aux.exist) {
					damage_aux(x, d_system[1]);
					if (sdb[x].structure.superstructure <= -ss)
						continue;
				}
				if (d_system[8] > 0.0 && sdb[x].main.exist) {
					damage_main(x, d_system[8]);
					if (sdb[x].structure.superstructure <= -ss)
						continue;
				}
				if (d_system[2] > 0.0 && sdb[x].batt.exist)
					damage_batt(x, d_system[2]);
				if (sdb[x].beam.exist)
					for (j = 0; j < sdb[x].beam.banks; ++j)
						if (d_beam[j] > 0.0)
							damage_beam(x, j, d_beam[j]);
				if (d_system[4] > 0.0 && sdb[x].cloak.exist)
					damage_cloak(x, d_system[4]);
				if (d_system[5] > 0.0 && sdb[x].sensor.ew_exist)
					damage_ew(x, d_system[5]);
				if (d_system[6] > 0.0 && sdb[x].engine.impulse_exist)
					damage_impulse(x, d_system[6]);
				if (d_system[7] > 0.0 && sdb[x].sensor.lrs_exist)
					damage_lrs(x, d_system[7]);
				if (sdb[x].missile.exist)
					for (j = 0; j < sdb[x].missile.tubes; ++j)
						if (d_missile[j] > 0.0)
							damage_missile(x, j, d_missile[j]);
				if (d_system[11] > 0.0 && sdb[x].sensor.srs_exist)
					damage_srs(x, d_system[11]);
				if (d_system[12] > 0.0 && sdb[x].tract.exist)
					damage_tract(x, d_system[12]);
				if (d_system[13] > 0.0 && sdb[x].trans.exist)
					damage_trans(x, d_system[13]);
				if (d_system[14] > 0.0 && sdb[x].engine.warp_exist)
					damage_warp(x, d_system[14]);
			}
			sdb[x].tmp.i0 = 0;
			sdb[x].tmp.i1 = 0;
			sdb[x].tmp.i2 = 0;
			sdb[x].tmp.i3 = 0;
			sdb[x].tmp.i4 = 0;
			sdb[x].tmp.i5 = 0;
			sdb[x].tmp.d0 = 0.0;
			sdb[x].tmp.d1 = 0.0;
			sdb[x].tmp.d2 = 0.0;
		}
	}
	return 1;
}


int do_set_main_reactor (double level, dbref enactor)
{
	char buff[BUFFER_LEN];
	char *bp = buff;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].main.exist) {
		notify(enactor, ansi_red(tprintf("%s has no M/A reactor.", Name(sdb[n].object))));
	} else if (sdb[n].main.damage <= -1.0) {
		notify(enactor, ansi_red("M/A reactor controls are inoperative."));
	} else if (sdb[n].fuel.antimatter <= 0.0) {
		notify(enactor, ansi_red("There is no antimatter fuel."));
	} else if (sdb[n].fuel.deuterium <= 0.0) {
		notify(enactor, ansi_red("There is no deuterium fuel."));
	} else {
		if (level < 0.0) {
			sdb[n].main.in = 0.0;
		} else if (level > sdb[n].tech.main_max) {
			sdb[n].main.in = sdb[n].tech.main_max;
		} else {
			sdb[n].main.in = level;
		}
		if (sdb[n].main.in > sdb[n].main.damage) {
			safe_format(buff, &bp, "M/A reactor set at %.3f%% %s%sOVERLOAD%s", sdb[n].main.in * 100.0, ANSI_BLINK, ANSI_RED, ANSI_NORMAL);
			console_message(n, "engineering", ansi_cmd(enactor, buff));
		} else {
			safe_format(buff, &bp, "M/A reactor set at %.3f%%", sdb[n].main.in * 100.0);
			*bp = '\0';

			console_message(n, "engineering", ansi_cmd(enactor, buff));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_aux_reactor (double level, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].aux.exist) {
		notify(enactor, ansi_red(tprintf("%s has no fusion reactor.", Name(sdb[n].object))));
	} else if (sdb[n].aux.damage <= -1.0) {
		notify(enactor, ansi_red("Fusion reactor controls are inoperative."));
	} else if (sdb[n].fuel.deuterium <= 0.0) {
		notify(enactor, ansi_red("There is no deuterium fuel."));
	} else {
		if (level < 0.0) {
			sdb[n].aux.in = 0.0;
		} else if (level > sdb[n].tech.aux_max) {
			sdb[n].aux.in = sdb[n].tech.aux_max;
		} else
			sdb[n].aux.in = level;
		if (sdb[n].aux.in > sdb[n].aux.damage) {
			console_message(n, "engineering",
				ansi_cmd(enactor, tprintf("Fusion reactor set at %.3f%% %s%sOVERLOAD%s",
				sdb[n].aux.in * 100.0, ANSI_BLINK, ANSI_RED, ANSI_NORMAL)));
		} else {
			console_message(n, "engineering",
				ansi_cmd(enactor, tprintf("Fusion reactor set at %.3f%%", sdb[n].aux.in * 100.0)));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_battery (double level, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].batt.exist) {
		notify(enactor, ansi_red(tprintf("%s has no batteries.", Name(sdb[n].object))));
	} else if (sdb[n].batt.damage <= 0.0) {
		notify(enactor, ansi_red("Batteries are inoperative."));
	} else if (sdb[n].fuel.reserves <= 0.0) {
		notify(enactor, ansi_red("There is no battery power."));
	} else {
		if (level < 0.0) {
			sdb[n].batt.in = 0.0;
		} else if (level > 1.0) {
			sdb[n].batt.in = 1.0;
		} else
			sdb[n].batt.in = level;
		console_message(n, "engineering",
			ansi_cmd(enactor, tprintf("Batteries set at %.3f%%", sdb[n].batt.in * 100)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_lrs (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.lrs_exist) {
		notify(enactor, ansi_red(tprintf("%s has no long-range sensors.", Name(sdb[n].object))));
	} else if (sdb[n].sensor.lrs_damage <= 0.0) {
		notify(enactor, ansi_red("Long-range sensors are inoperative."));
	} else {
		if (active) {
			if (sdb[n].sensor.lrs_active) {
				notify(enactor, ansi_red("Long-range sensors are already online."));
			} else {
				sdb[n].sensor.lrs_active = 1;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Long-range sensors online"));
				return 1;
			}
		} else {
			if (!sdb[n].sensor.lrs_active) {
				notify(enactor, ansi_red("Long-range sensors are already offline."));
			} else {
				sdb[n].sensor.lrs_active = 0;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Long-range sensors offline"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_srs (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.srs_exist) {
		notify(enactor, ansi_red(tprintf("%s has no short-range sensors.", Name(sdb[n].object))));
	} else if (sdb[n].sensor.srs_damage <= 0.0) {
		notify(enactor, ansi_red("Short-range sensors are inoperative."));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use short-range sensors while cloaked.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].sensor.srs_active) {
				notify(enactor, ansi_red("Short-range sensors are already online."));
			} else {
				sdb[n].sensor.srs_active = 1;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Short-range sensors online"));
				return 1;
			}
		} else {
			if (!sdb[n].sensor.srs_active) {
				notify(enactor, ansi_red("Short-range sensors are already offline."));
			} else {
				sdb[n].sensor.srs_active = 0;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Short-range sensors offline"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_ew (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.ew_exist) {
		notify(enactor, ansi_red(tprintf("%s has no electronic warfare systems.", Name(sdb[n].object))));
	} else if (sdb[n].sensor.ew_damage <= 0.0) {
		notify(enactor, ansi_red("Electronic warfare systems are inoperative."));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use electronic warfare sensors while cloaked.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].sensor.ew_active) {
				notify(enactor, ansi_red("Electronic warfare systems are already online."));
			} else {
				sdb[n].sensor.ew_active = 1;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Electronic warfare systems online"));
				return 1;
			}
		} else {
			if (!sdb[n].sensor.ew_active) {
				notify(enactor, ansi_red("Electronic warfare systems are already offline."));
			} else {
				sdb[n].sensor.ew_active = 0;
				sdb[n].sensor.version = 1;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Electronic warfare systems offline"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_cloak (int active, dbref enactor)
{
	static char buffer[800];
	register int i;
	int x, flag;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].cloak.exist) {
		notify(enactor, ansi_red(tprintf("%s has no cloaking device.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].cloak.damage <= 0.0) {
		notify(enactor, ansi_red("Cloaking device is inoperative."));
		return 0;
	}

	if (active) {
		if (sdb[n].cloak.active) {
			notify(enactor, ansi_red("Cloaking device is already online."));
			return 0;
		} else if (sdb[n].alloc.cloak * sdb[n].power.total < sdb[n].cloak.cost) {
			notify(enactor, ansi_red("Insufficient power for cloaking device."));
			return 0;
		} else if (sdb[n].tech.cloak < 2.0) {
			if (sdb[n].sensor.srs_active) {
				sdb[n].sensor.srs_active = 0;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Short-range sensors offline"));
			}
			if (sdb[n].sensor.ew_active) {
				sdb[n].sensor.ew_active = 0;
				console_message(n, "helm science tactical",
					ansi_cmd(enactor, "Electronic warfare systems offline"));
			}
			if (sdb[n].trans.active) {
				sdb[n].trans.active = 0;
				sdb[n].trans.d_lock = sdb[n].trans.s_lock = 0;
				console_message(n, "helm operation transporter",
					ansi_cmd(enactor, "Transporters offline"));
			}
			if (sdb[n].tract.active) {
				sdb[n].tract.active = 0;
				sdb[n].tract.lock = 0;
				if (sdb[n].status.tractoring) {
					x = sdb[n].status.tractoring;
					alert_tract_unlock(n, x, enactor);
					sdb[n].status.tractoring = 0;
					sdb[x].status.tractored = 0;
					sdb[n].power.version = 1;
					sdb[x].power.version = 1;
				}
				console_message(n, "helm operation",
					ansi_cmd(enactor, "Tractor beams offline"));
			}
			if (sdb[n].beam.exist)
				for (i = 0; i < sdb[n].beam.banks; ++i)
					sdb[n].blist.lock[i] = 0;
			if (sdb[n].missile.exist)
				for (i = 0; i < sdb[n].missile.tubes; ++i)
					sdb[n].mlist.lock[i] = 0;
			if (sdb[n].shield.exist) {
				strncpy(buffer, "", sizeof(buffer) - 1);
				flag = 0;
				for (i = 0; i < MAX_SHIELD_NAME; ++i) {
					if (sdb[n].shield.active[i]) {
						if (flag)
							strncat(buffer, "\n", sizeof(buffer) - 1);
						sdb[n].shield.active[i] = 0;
						strncat(buffer, ansi_cmd(enactor, tprintf("%s offline", unparse_shield(i))), sizeof(buffer) - 1);
						++flag;
					}
				}
				if (flag) {
					console_message(n, "helm", buffer);
				}
			}
		}
		sdb[n].cloak.active = 1;
		sdb[n].sensor.version = 1;
		sdb[n].engine.version = 1;
		console_message(n, "helm tactical",
		  ansi_cmd(enactor, "Cloaking device online"));
		alert_ship_cloak_online(n);
		return 1;
	} else {
		if (!sdb[n].cloak.active) {
			notify(enactor, ansi_red("Cloaking device is already offline."));
			return 0;
		} else {
			if (sdb[n].tech.cloak < 2.0) {
				if ( ! sdb[n].sensor.srs_active) {
					sdb[n].sensor.srs_active = 1;
					console_message(n, "helm science tactical",
						ansi_cmd(enactor, "Short-range sensors online"));
				}
				if ( ! sdb[n].sensor.ew_active) {
					sdb[n].sensor.ew_active = 1;
					console_message(n, "helm science tactical",
						ansi_cmd(enactor, "Electronic warfare systems online"));
				}
			}
			sdb[n].cloak.active = 0;
			sdb[n].sensor.version = 1;
			sdb[n].engine.version = 1;
			console_message(n, "helm tactical",
			  ansi_cmd(enactor, "Cloaking device offline"));
			alert_ship_cloak_offline(n);
			return 1;
		}
	}
}

/* ------------------------------------------------------------------------ */

int do_set_course (double yaw, double pitch, double roll, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else if (fabs(yaw) > 360.0 || fabs(pitch) > 360.0 || fabs(roll) > 360.0) {
		notify(enactor, ansi_red(tprintf("That is not a valid course.")));
	} else {
		if (sdb[n].status.autopilot) {
			sdb[n].status.autopilot = 0;
			console_message(n, "helm",
			  ansi_cmd(enactor, "Autopilot disengaged"));
		}
		sdb[n].course.yaw_in = fmod(yaw,360);
		sdb[n].course.pitch_in = fmod(pitch,360);
		sdb[n].course.roll_in = fmod(roll,360);
		if (sdb[n].course.yaw_in < 0.0)
			sdb[n].course.yaw_in += 360.0;
		if (sdb[n].course.pitch_in < 0.0)
			sdb[n].course.pitch_in += 360.0;
		if (sdb[n].course.roll_in < 0.0)
			sdb[n].course.roll_in += 360.0;
		if (sdb[n].course.roll_in != 0.0) {
			console_message(n, "helm",
				ansi_cmd(enactor, tprintf("Course set to %.3f %.3f %.3f",
				sdb[n].course.yaw_in, sdb[n].course.pitch_in, sdb[n].course.roll_in)));
		} else {
			console_message(n, "helm",
				ansi_cmd(enactor, tprintf("Course set to %.3f %.3f",
				sdb[n].course.yaw_in, sdb[n].course.pitch_in)));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_yaw (double yaw, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else if (fabs(yaw) > 360.0) {
		notify(enactor, ansi_red(tprintf("That is not a valid yaw value.")));
	} else {
		if (sdb[n].status.autopilot) {
			sdb[n].status.autopilot = 0;
			console_message(n, "helm",
			  ansi_cmd(enactor, "Autopilot disengaged"));
		}
		sdb[n].course.yaw_in = fmod(sdb[n].course.yaw_in + yaw,360);
		if (sdb[n].course.yaw_in < 0.0)
			sdb[n].course.yaw_in += 360.0;
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Yaw adjusted %+.3f to %.3f",
			yaw, sdb[n].course.yaw_in)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_pitch (double pitch, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else if (fabs(pitch) > 360.0) {
		notify(enactor, ansi_red(tprintf("That is not a valid pitch value.")));
	} else {
		if (sdb[n].status.autopilot) {
			sdb[n].status.autopilot = 0;
			console_message(n, "helm",
			  ansi_cmd(enactor, "Autopilot disengaged"));
		}
		sdb[n].course.pitch_in = fmod(sdb[n].course.pitch_in + pitch,360);
		if (sdb[n].course.pitch_in < 0.0)
			sdb[n].course.pitch_in += 360.0;
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Pitch adjusted %+.3f to %.3f",
			pitch, sdb[n].course.pitch_in)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_roll (double roll, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else if (fabs(roll) > 360.0) {
		notify(enactor, ansi_red(tprintf("That is not a valid roll value.")));
	} else {
		if (sdb[n].status.autopilot) {
			sdb[n].status.autopilot = 0;
			console_message(n, "helm",
			  ansi_cmd(enactor, "Autopilot disengaged"));
		}
		sdb[n].course.roll_in = fmod(sdb[n].course.roll_in + roll,360);
		if (sdb[n].course.roll_in < 0.0)
			sdb[n].course.roll_in += 360.0;
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Roll adjusted %+.3f to %.3f",
			roll, sdb[n].course.roll_in)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_intercept (int contact, dbref enactor)
{
	int x;

	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else {
			sdb[n].coords.xd = sdb[x].coords.x;
			sdb[n].coords.yd = sdb[x].coords.y;
			sdb[n].coords.zd = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb2bearing(n, x);
			sdb[n].course.pitch_in = sdb2elevation(n, x);
			if (sdb[n].course.roll_in != 0.0) {
				console_message(n, "helm",
				  ansi_cmd(enactor, tprintf("Intercept course to %s set %.3f %.3f %.3f",
				  unparse_identity(n, x), sdb[n].course.yaw_in, sdb[n].course.pitch_in,
				  sdb[n].course.roll_in)));
			} else {
				console_message(n, "helm",
				  ansi_cmd(enactor, tprintf("Intercept course to %s set %.3f %.3f",
				  unparse_identity(n, x), sdb[n].course.yaw_in, sdb[n].course.pitch_in)));
			}
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_evade (int contact, dbref enactor)
{
	int x;

	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else {
			if (sdb[n].status.autopilot) {
				sdb[n].status.autopilot = 0;
				console_message(n, "helm",
				  ansi_cmd(enactor, "Autopilot disengaged"));
			}
			sdb[n].coords.xd = sdb[x].coords.x;
			sdb[n].coords.yd = sdb[x].coords.y;
			sdb[n].coords.zd = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb2bearing(x,n);
			sdb[n].course.pitch_in = sdb2elevation(x,n);
			if (sdb[n].course.yaw_in < 0.0)
				sdb[n].course.yaw_in += 360.0;
			if (sdb[n].course.pitch_in < 0.0)
				sdb[n].course.pitch_in += 360.0;
			if (sdb[n].course.roll_in != 0.0) {
				console_message(n, "helm",
				  ansi_cmd(enactor, tprintf("Evasive course from %s set %.3f %.3f %.3f",
				  unparse_identity(n, x), sdb[n].course.yaw_in, sdb[n].course.pitch_in,
				  sdb[n].course.roll_in)));
			} else {
				console_message(n, "helm",
				  ansi_cmd(enactor, tprintf("Evasive course from %s set %.3f %.3f",
				  unparse_identity(n, x), sdb[n].course.yaw_in, sdb[n].course.pitch_in)));
			}
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_parallel (int contact, dbref enactor)
{
	int x;

	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else {
			if (sdb[n].status.autopilot) {
				sdb[n].status.autopilot = 0;
				console_message(n, "helm",
				  ansi_cmd(enactor, "Autopilot disengaged"));
			}
			sdb[n].course.yaw_in = sdb[x].course.yaw_out;
			sdb[n].course.pitch_in = sdb[x].course.pitch_out;
			console_message(n, "helm",
			  ansi_cmd(enactor, tprintf("Parallel course for %s set %.3f %.3f",
			  unparse_identity(n, x), sdb[n].course.yaw_in, sdb[n].course.pitch_in)));
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_coords_manual (double x, double y, double z, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].coords.xo = sdb[n].coords.x - pc2su(x);
		sdb[n].coords.yo = sdb[n].coords.y - pc2su(y);
		sdb[n].coords.zo = sdb[n].coords.z - pc2su(z);
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Relative coordinates set to %.3f %.3f %.3f",
				x, y, z)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_coords_reset (dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].coords.xo = 0;
		sdb[n].coords.yo = 0;
		sdb[n].coords.zo = 0;
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Relative coordinates reset to %.3f %.3f %.3f",
				su2pc(sdb[n].coords.x), su2pc(sdb[n].coords.y), su2pc(sdb[n].coords.z))));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_coords_layin (double x, double y, double z, dbref enactor)
{
//	double dx, dy, dz;

	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else {
		sdb[n].coords.xd = pc2su(x) + sdb[n].coords.xo;
		sdb[n].coords.yd = pc2su(y) + sdb[n].coords.yo;
		sdb[n].coords.zd = pc2su(z) + sdb[n].coords.zo;
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Coordinates %.3f %.3f %.3f laid in", x, y, z)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_coords_engage (dbref enactor)
{
//	double dx, dy, dz;

	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else {
		sdb[n].course.yaw_in = xy2bearing(sdb[n].coords.xd - sdb[n].coords.x, sdb[n].coords.yd - sdb[n].coords.y);
		sdb[n].course.pitch_in = xyz2elevation(sdb[n].coords.xd - sdb[n].coords.x, sdb[n].coords.yd - sdb[n].coords.y, sdb[n].coords.zd - sdb[n].coords.z);
		console_message(n, "helm",
			ansi_cmd(enactor, tprintf("Course %.3f %.3f engaged", sdb[n].course.yaw_in, sdb[n].course.pitch_in)));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_eta (double speed, dbref enactor)
{
	double dx, dy, dz, years, days, hours, minutes, seconds;
	static char buffer[100];

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s has no engines.", Name(sdb[n].object))));
		return 0;
	}

	speed = fabs(speed);
	if (speed == 0.0) {
		speed = fabs(sdb[n].move.in);
		if (speed == 0.0) {
			notify(enactor, ansi_red(tprintf("%s has no motion.", Name(sdb[n].object))));
			return 0;
		}
	}

	if (speed < 1.0) {
		speed *= LIGHTSPEED;
	} else {
		speed = LIGHTSPEED * pow(speed, 3.333333) * 0.5 *
		  (xyz2cochranes(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z) +
		  xyz2cochranes(sdb[n].coords.xd, sdb[n].coords.yd, sdb[n].coords.zd));
	}

	dx = sdb[n].coords.xd - sdb[n].coords.x;
	dy = sdb[n].coords.yd - sdb[n].coords.y;
	dz = sdb[n].coords.zd - sdb[n].coords.z;

	seconds = sqrt(dx * dx + dy * dy + dz * dz) / speed;
	years = floor(seconds / 31557081.168);
	seconds -= years * 31557081.168;
	days = floor(seconds / 86400.0);
	seconds -= days * 86400.0;
	hours = floor(seconds / 3600.0);
	seconds -= hours * 3600.0;
	minutes = floor(seconds / 60.0);
	seconds -= minutes * 60.0;

	strncpy(buffer, "ETA is ", sizeof(buffer) - 1);
	if (years > 0.0)
		strncat(buffer, tprintf("%.0f years ", years), sizeof(buffer) - 1);
	if (days > 0.0)
		strncat(buffer, tprintf("%.0f days ", days), sizeof(buffer) - 1);
	if (hours > 0.0)
		strncat(buffer, tprintf("%.0f hrs ", hours), sizeof(buffer) - 1);
	if (minutes > 0.0)
		strncat(buffer, tprintf("%.0f min ", minutes), sizeof(buffer) - 1);
	strncat(buffer, tprintf("%.0f sec", seconds), sizeof(buffer) - 1);

	notify(enactor, ansi_alert(buffer));

	return 1;
}

/* ------------------------------------------------------------------------ */

int do_set_speed (double speed, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && (fabs(speed) >= 1.0)) {
		notify(enactor, ansi_red(tprintf("%s has no warp engines.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.impulse_exist && (fabs(speed) < 1.0)) {
		notify(enactor, ansi_red(tprintf("%s has no impulse engines.", Name(sdb[n].object))));
	} else if (sdb[n].engine.warp_damage <= 0.0 && (fabs(speed) >= 1.0)) {
		notify(enactor, ansi_red("Warp engines are inoperative."));
	} else if (sdb[n].engine.impulse_damage <= 0.0 && (speed != 0.0) && (fabs(speed) < 1.0)) {
		notify(enactor, ansi_red("Impulse engines are inoperative."));
	} else {
		sdb[n].move.in = speed;
		if ((sdb[n].move.in >= 1.0) && (sdb[n].move.in > sdb[n].engine.warp_max)) {
			sdb[n].move.in = sdb[n].engine.warp_max;
		} else if ((sdb[n].move.in <= -1.0) && (sdb[n].move.in < (-sdb[n].engine.warp_max / 2.0))) {
			sdb[n].move.in = -sdb[n].engine.warp_max / 2.0;
		} else if ((sdb[n].move.in >= 0.0) && (sdb[n].move.in < 1.0) && (sdb[n].move.in > sdb[n].engine.impulse_max)) {
			sdb[n].move.in = sdb[n].engine.impulse_max;
		} else if ((sdb[n].move.in <= 0.0) && (sdb[n].move.in > -1.0) && (sdb[n].move.in < (-sdb[n].engine.impulse_max / 2.0))) {
			sdb[n].move.in = -sdb[n].engine.impulse_max / 2.0;
		}
		if (fabs(sdb[n].move.in) >= 1.0) {
			if (fabs(sdb[n].move.in) > sdb[n].engine.warp_cruise) {
				console_message(n, "helm engineering",
				  ansi_cmd(enactor, tprintf("Speed set to warp %.6f %s%sOVERLOAD%s",
				  sdb[n].move.in, ANSI_BLINK, ANSI_RED, ANSI_NORMAL)));
			} else
				console_message(n, "helm engineering",
				  ansi_cmd(enactor, tprintf("Speed set to warp %.6f",
				  sdb[n].move.in)));
		} else {
			if (fabs(sdb[n].move.in) > sdb[n].engine.impulse_cruise) {
				console_message(n, "helm engineering",
				  ansi_cmd(enactor, tprintf("Speed set to %.3f%% impulse %s%sOVERLOAD%s",
				  sdb[n].move.in * 100.0, ANSI_BLINK, ANSI_RED, ANSI_NORMAL)));
			} else
				console_message(n, "helm engineering",
				  ansi_cmd(enactor, tprintf("Speed set to %.3f%% impulse",
				  sdb[n].move.in * 100.0)));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_shield (int shield, int active, dbref enactor)
{
	static char buffer[800];
	int a, b, flag;
	register int i;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].shield.exist) {
		notify(enactor, ansi_red(tprintf("%s has no shields.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.docked && active) {
		notify(enactor, ansi_red(tprintf("%s cannot raise shields while docked.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.landed && active) {
		notify(enactor, ansi_red(tprintf("%s cannot raise shields while landed.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot raise shields while cloaked.", Name(sdb[n].object))));
		return 0;
	}

	flag = 0;

	a = shield;
	if (a < 1 || a > MAX_SHIELD_NAME) {
		a = 0;
		b = MAX_SHIELD_NAME - 1;
	} else {
		--a;
		b = a;
	}

	strncpy(buffer, "", sizeof(buffer) - 1);
	for (i = a; i <= b; ++i)
		if (sdb[n].shield.damage[i] <= 0.0) {
			notify(enactor, ansi_red(tprintf("%s is inoperative.", unparse_shield(i))));
		} else if (sdb[n].shield.active[i] && active) {
			notify(enactor, ansi_red(tprintf("%s is already online.", unparse_shield(i))));
		} else if (!sdb[n].shield.active[i] && !active) {
			notify(enactor, ansi_red(tprintf("%s is already offline.", unparse_shield(i))));
		} else {
			if (flag)
				strncat(buffer, "\n", sizeof(buffer) - 1);
			if (active) {
				sdb[n].shield.active[i] = 1;
				strncat(buffer, ansi_cmd(enactor, tprintf("%s online", unparse_shield(i))), sizeof(buffer) - 1);
			} else {
				sdb[n].shield.active[i] = 0;
				strncat(buffer, ansi_cmd(enactor, tprintf("%s offline", unparse_shield(i))), sizeof(buffer) - 1);
			}
			++flag;
		}

	if (flag) {
		console_message(n, "helm", buffer);
		if (active) {
			write_spacelog(enactor, sdb[n].object, "LOG: Shields raised");
		} else
			write_spacelog(enactor, sdb[n].object, "LOG: Shields lowered");
		sdb[n].engine.version = 1;
	}

	return 1;
}

/* ------------------------------------------------------------------------ */

int do_set_weapon (int first, int last, int active, int weapon, dbref enactor)
{
	static char buffer[BUFFER_LEN];
	int a, b, flag_b = 0, flag_m = 0;
	register int i;

	if (error_on_console(enactor)) {
		return 0;
	} else if (weapon == 1 && !sdb[n].beam.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[3])));
		return 0;
	} else if (weapon == 2 && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[9])));
		return 0;
	} else if (!weapon && !sdb[n].beam.exist && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no weapons.", Name(sdb[n].object))));
		return 0;
	}

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (weapon == 1 || (!weapon && sdb[n].beam.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].beam.banks) {
			a = 0;
			b = sdb[n].beam.banks - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].beam.banks)
				b = sdb[n].beam.banks - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].blist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (active && sdb[n].blist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already online.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (!active && !sdb[n].blist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already offline.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (active) {
				sdb[n].blist.lock[i] = 0;
				sdb[n].blist.active[i] = 1;
				sdb[n].beam.in += sdb[n].blist.cost[i] * 10.0;
				if (!flag_b)
					strncat(buffer, system_name[3], sizeof(buffer) - 1);
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_b;
			} else {
				sdb[n].blist.lock[i] = 0;
				sdb[n].blist.active[i] = 0;
				sdb[n].beam.in -= sdb[n].blist.cost[i] * 10.0;
				if (!flag_b)
					strncat(buffer, system_name[3], sizeof(buffer) - 1);
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_b;
			}
	}

	if (weapon == 2 || (!weapon && sdb[n].missile.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].missile.tubes) {
			a = 0;
			b = sdb[n].missile.tubes - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].missile.tubes)
				b = sdb[n].missile.tubes - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].mlist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (active && sdb[n].mlist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already online.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (!active && !sdb[n].mlist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already offline.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (active) {
				sdb[n].mlist.lock[i] = 0;
				sdb[n].mlist.active[i] = 1;
				sdb[n].missile.in += sdb[n].mlist.cost[i];
				if (!flag_m) {
					if (flag_b)
						strncat(buffer, " ", sizeof(buffer) - 1);
					strncat(buffer, system_name[9], sizeof(buffer) - 1);
				}
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_m;
			} else {
				sdb[n].mlist.lock[i] = 0;
				sdb[n].mlist.active[i] = 0;
				sdb[n].missile.in -= sdb[n].mlist.cost[i];
				if (!flag_m) {
					if (flag_b)
						strncat(buffer, " ", sizeof(buffer) - 1);
					strncat(buffer, system_name[9], sizeof(buffer) - 1);
				}
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_m;
			}
	}

	if (flag_b || flag_m) {
		if (active) {
			console_message(n, "tactical",
			  ansi_cmd(enactor, tprintf("%s online", buffer)));
		} else
			console_message(n, "tactical",
			  ansi_cmd(enactor, tprintf("%s offline", buffer)));
	}

	return 1;
}

/* ------------------------------------------------------------------------ */

void do_fed_shield_bug_check (int x)
{
	static char buffer[800];
	int flag = 0;
	register int i;

	if (!sdb[x].shield.exist) /* no shields */
		return;
	if (!sdb[x].status.active) /* inactive */
		return;
	if (sdb[x].status.crippled) /* crippled or dead */
		return;
	if (sdb[x].cloak.active && sdb[x].tech.cloak < 2.0) /* cloaking */
		return;
	if (sdb[x].alloc.shields * sdb[x].power.total <= 0.0) /* shields powered */
		return;

	strncpy(buffer, "", sizeof(buffer) - 1);
	for (i = 0; i < MAX_SHIELD_NAME; ++i)
		if (sdb[x].shield.damage[i] > 0.0) {
			if (!sdb[x].shield.active[i]) {
				if (flag)
					strncat(buffer, "\n", sizeof(buffer) - 1);
				sdb[x].shield.active[i] = 1;
				strncat(buffer, ansi_warn(tprintf("%s online", unparse_shield(i))), sizeof(buffer) - 1);
			}
			++flag;
		}

	if (flag) {
		console_message(x, "helm", buffer);
		write_spacelog(sdb[x].object, sdb[x].object, "LOG: Shields raised automagically");
		sdb[x].engine.version = 1;
	}

	return;
}

int do_lock_weapon (int first, int last, int contact, int weapon, dbref enactor)
{
	static char buffer[BUFFER_LEN];
	int a, b, flag_b = 0, flag_m = 0, flag_lock = 0, x, y;
	register int i;

	if (error_on_console(enactor)) {
		return 0;
	} else if (weapon == 1 && !sdb[n].beam.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[3])));
		return 0;
	} else if (weapon == 2 && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[9])));
		return 0;
	} else if (!weapon && !sdb[n].beam.exist && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no weapons.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot lock weapons while cloaked.", Name(sdb[n].object))));
		return 0;
	}

	x = contact2sdb(n, contact);
	if (error_on_contact(n, x, enactor))
		return 0;

	for (i = 0; i < sdb[n].beam.banks; ++i)
		if (sdb[n].blist.lock[i] == x)
			++flag_lock;

	for (i = 0; i < sdb[n].missile.tubes; ++i)
		if (sdb[n].mlist.lock[i] == x)
			++flag_lock;

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (weapon == 1 || (!weapon && sdb[n].beam.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].beam.banks) {
			a = 0;
			b = sdb[n].beam.banks - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].beam.banks)
				b = sdb[n].beam.banks - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].blist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (!sdb[n].blist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is not online.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (sdb[n].blist.lock[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already locked.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else {
				sdb[n].blist.lock[i] = x;
				if (!flag_b)
					strncat(buffer, system_name[3], sizeof(buffer) - 1);
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_b;
			}
	}

	if (weapon == 2 || (!weapon && sdb[n].missile.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].missile.tubes) {
			a = 0;
			b = sdb[n].missile.tubes - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].missile.tubes)
				b = sdb[n].missile.tubes - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].mlist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (!sdb[n].mlist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is not online.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (sdb[n].mlist.lock[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already locked.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else {
				sdb[n].mlist.lock[i] = x;
				if (!flag_m) {
					if (flag_b)
						strncat(buffer, " ", sizeof(buffer) - 1);
					strncat(buffer, system_name[9], sizeof(buffer) - 1);
				}
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_m;
			}
	}

	if (flag_b || flag_m) {
		console_message(n, "tactical",
		  ansi_cmd(enactor, tprintf("%s locked on %s", buffer, unparse_identity(n, x))));
		if (!flag_lock) {
			y = sdb2contact(x, n);
			if (y != SENSOR_FAIL)
				console_message(x, "helm science tactical",
				  ansi_warn(tprintf("%cWeapon lock from %s detected", BEEP_CHAR, unparse_identity(x, n))));
		}
	}

	return 1;
}

/* ------------------------------------------------------------------------ */

int do_unlock_weapon (int first, int last, int weapon, dbref enactor)
{
	static char buffer[BUFFER_LEN];
	int a, b, flag_b = 0, flag_m = 0;
	//int flag_lock = 0, x;
	register int i;

	if (error_on_console(enactor)) {
		return 0;
	} else if (weapon == 1 && !sdb[n].beam.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[3])));
		return 0;
	} else if (weapon == 2 && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no %ss.", Name(sdb[n].object), system_name[9])));
		return 0;
	} else if (!weapon && !sdb[n].beam.exist && !sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no weapons.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
		return 0;
	}

	strncpy(buffer, "", sizeof(buffer) - 1);
	if (weapon == 1 || (!weapon && sdb[n].beam.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].beam.banks) {
			a = 0;
			b = sdb[n].beam.banks - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].beam.banks)
				b = sdb[n].beam.banks - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].blist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (!sdb[n].blist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is not online.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else if (!sdb[n].blist.lock[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already unlocked.",
					unparse_beam(sdb[n].blist.name[i]), i + 1)));
			} else {
				sdb[n].blist.lock[i] = 0;
				if (!flag_b)
					strncat(buffer, system_name[3], sizeof(buffer) - 1);
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_b;
			}
	}

	if (weapon == 2 || (!weapon && sdb[n].missile.exist)) {
		a = first;
		b = last;
		if (a < 1 || a > sdb[n].missile.tubes) {
			a = 0;
			b = sdb[n].missile.tubes - 1;
		} else {
			--a;
			--b;
		 	if (b >= sdb[n].missile.tubes)
				b = sdb[n].missile.tubes - 1;
			if (b < a)
				b = a;
		}
		for (i = a; i <= b ; ++i)
			if (sdb[n].mlist.damage[i] <= 0.0) {
				notify(enactor, ansi_red(tprintf("%s %d is inoperative.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (!sdb[n].mlist.active[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is not online.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else if (!sdb[n].mlist.lock[i]) {
				notify(enactor, ansi_red(tprintf("%s %d is already unlocked.",
					unparse_missile(sdb[n].mlist.name[i]), i + 1)));
			} else {
				sdb[n].mlist.lock[i] = 0;
				if (!flag_m) {
					if (flag_b)
						strncat(buffer, " ", sizeof(buffer) - 1);
					strncat(buffer, system_name[9], sizeof(buffer) - 1);
				}
				strncat(buffer, " ", sizeof(buffer) - 1);
				strncat(buffer, unparse_integer(i + 1), sizeof(buffer) - 1);
				++flag_m;
			}
	}

	if (flag_b || flag_m) {
		console_message(n, "tactical",
		  ansi_cmd(enactor, tprintf("%s unlocked", buffer)));
	}

	return 1;
}

/* ------------------------------------------------------------------------ */

int do_set_trans (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].trans.exist) {
		notify(enactor, ansi_red(tprintf("%s has no transporters.", Name(sdb[n].object))));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use transporters while cloaked.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].trans.damage <= 0.0) {
				notify(enactor, ansi_red("Transporters are inoperative."));
			} else if (sdb[n].trans.active) {
				notify(enactor, ansi_red("Transporters are already online."));
			} else {
				sdb[n].trans.active = 1;
				sdb[n].trans.d_lock = sdb[n].trans.s_lock = 0;
				console_message(n, "helm operation transporter",
					ansi_cmd(enactor, "Transporters online"));
				return 1;
			}
		} else {
			if (sdb[n].trans.damage <= 0.0) {
				notify(enactor, ansi_red("Transporters are inoperative."));
			} else if (!sdb[n].trans.active) {
				notify(enactor, ansi_red("Transporters are already offline."));
			} else {
				sdb[n].trans.active = 0;
				sdb[n].trans.d_lock = sdb[n].trans.s_lock = 0;
				console_message(n, "helm operation transporter",
					ansi_cmd(enactor, "Transporters offline"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_trans_locked (int contact, int target, dbref enactor)
{
	int x, y;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].trans.exist) {
		notify(enactor, ansi_red(tprintf("%s has no transporters.", Name(sdb[n].object))));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use transporters while cloaked.", Name(sdb[n].object))));
	} else {
		if (contact == -1) {
			x = n;
		} else
			x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else if (sdb[x].structure.type > 3) {
			notify(enactor, ansi_red("You cannot lock transporters on that."));
		} else {
			if (sdb[n].trans.damage <= 0.0) {
				notify(enactor, ansi_red("Transporters are inoperative."));
			} else if (!sdb[n].trans.active) {
				notify(enactor, ansi_red("Transporters are not online."));
			} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2) {
				notify(enactor, ansi_red("Cloaking device is still online."));
			} else {
				if (target) {
					sdb[n].trans.d_lock = x;
					console_message(n, "operation transporter",
					  ansi_cmd(enactor, tprintf("Transporter dest lock on %s", unparse_identity(n, x))));
				} else {
					sdb[n].trans.s_lock = x;
					console_message(n, "operation transporter",
					  ansi_cmd(enactor, tprintf("Transporter source lock on %s", unparse_identity(n, x))));
				}
			}
			if (x != n) {
				y = sdb2contact(x, n);
				if (y != SENSOR_FAIL)
					console_message(x, "operation tactical transporter",
					  ansi_warn(tprintf("%cTransporter lock from %s detected", BEEP_CHAR, unparse_identity(x, n))));
			}
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_trans_unlocked (dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].trans.exist) {
		notify(enactor, ansi_red(tprintf("%s has no transporters.", Name(sdb[n].object))));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use transporters while cloaked.", Name(sdb[n].object))));
	} else {
		if (sdb[n].trans.damage <= 0.0) {
			notify(enactor, ansi_red("Transporters are inoperative."));
		} else if (!sdb[n].trans.active) {
			notify(enactor, ansi_red("Transporters are not online."));
		} else if (!sdb[n].trans.d_lock && !sdb[n].trans.s_lock) {
			notify(enactor, ansi_red("Transporters are not locked."));
		} else {
			sdb[n].trans.d_lock = sdb[n].trans.s_lock = 0;
			console_message(n, "transporter operation",
				ansi_cmd(enactor, "Transporters unlocked"));
		}
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_tract (int active, dbref enactor)
{
	int x;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].tract.exist) {
		notify(enactor, ansi_red(tprintf("%s has no tractor beams.", Name(sdb[n].object))));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use tractor beams while cloaked.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].tract.damage <= 0.0) {
				notify(enactor, ansi_red("Tractor beams are inoperative."));
			} else if (sdb[n].tract.active) {
				notify(enactor, ansi_red("Tractor beams are already online."));
			} else {
				sdb[n].tract.active = 1;
				sdb[n].tract.lock = 0;
				sdb[n].status.tractoring = 0;
				sdb[n].power.version = 1;
				console_message(n, "helm operation",
					ansi_cmd(enactor, "Tractor beams online"));
				return 1;
			}
		} else {
			if (sdb[n].tract.damage <= 0.0) {
				notify(enactor, ansi_red("Tractor beams are inoperative."));
			} else if (!sdb[n].tract.active) {
				notify(enactor, ansi_red("Tractor beams are already offline."));
			} else {
				sdb[n].tract.active = 0;
				sdb[n].tract.lock = 0;
				if (sdb[n].status.tractoring) {
					x = sdb[n].status.tractoring;
					alert_tract_unlock(n, x, enactor);
					sdb[n].status.tractoring = 0;
					sdb[x].status.tractored = 0;
					sdb[n].power.version = 1;
					sdb[x].power.version = 1;
				}
				console_message(n, "helm operation",
					ansi_cmd(enactor, "Tractor beams offline"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_tract_locked (int contact, dbref enactor)
{
	int x, c;
	double pn, px, pc;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].tract.exist) {
		notify(enactor, ansi_red(tprintf("%s has no tractor beam.", Name(sdb[n].object))));
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.tractoring) {
		notify(enactor, ansi_red(tprintf("%s is already tractoring %s.", Name(sdb[n].object), Name(sdb[sdb[n].status.tractoring].object))));
	} else if (sdb[n].status.tractored) {
		notify(enactor, ansi_red(tprintf("%s is being tractored by %s.", Name(sdb[n].object), Name(sdb[sdb[n].status.tractored].object))));
	} else if (sdb[n].tract.damage <= 0.0) {
		notify(enactor, ansi_red("Tractor beam is inoperative."));
	} else if (!sdb[n].tract.active) {
		notify(enactor, ansi_red("Tractor beam is not online."));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use tractor beams while cloaked.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else if (sdb[x].structure.type > 3) {
			notify(enactor, ansi_red("You cannot lock tractor beams on that."));
		} else {
			if ((pn = sdb[n].tract.damage * sdb[n].power.total * sdb[n].alloc.tractors / (sdb2range(n, x) + 1.0)) < 1.0) {
				notify(enactor, ansi_red("Tractor beam is not sufficiently powered."));
			} else {
				if (sdb[x].status.tractored) {
					c = sdb[x].status.tractored;
					pc = sdb[c].tract.damage * sdb[c].power.total * sdb[c].alloc.tractors / (sdb2range(c, x) + 1.0);
					px = sdb[x].tract.damage * sdb[x].power.total * sdb[x].alloc.tractors / (sdb2range(n, x) + 1.0);
					if (pn > pc && pn > px) {
						alert_tract_lost(c, x);
						alert_tract_lock(n, x, enactor);
						sdb[c].tract.lock = 0;
						sdb[n].tract.lock = x;
						sdb[c].status.tractoring = 0;
						sdb[n].status.tractoring = x;
						sdb[x].status.tractored = n;
						sdb[c].power.version = 1;
						sdb[n].power.version = 1;
						sdb[x].power.version = 1;
						return 1;
					} else {
						alert_tract_attempt(n, x, enactor);
					}
				} else if (sdb[x].status.tractoring) {
					c = sdb[x].status.tractoring;
					px = sdb[x].tract.damage * sdb[x].power.total * sdb[x].alloc.tractors / (sdb2range(n, x) + 1.0);
					if (pn > px) {
						alert_tract_lost(x, c);
						alert_tract_lock(n, x, enactor);
						sdb[n].tract.lock = x;
						sdb[x].tract.lock = 0;
						sdb[n].status.tractoring = x;
						sdb[x].status.tractoring = 0;
						sdb[c].status.tractored = 0;
						sdb[x].status.tractored = n;
						sdb[c].power.version = 1;
						sdb[n].power.version = 1;
						sdb[x].power.version = 1;
						return 1;
					} else {
						alert_tract_attempt(n, x, enactor);
					}
				} else {
					px = sdb[x].tract.damage * sdb[x].power.total * sdb[x].alloc.tractors / (sdb2range(n, x) + 1.0);
					if (pn > px) {
						alert_tract_lock(n, x, enactor);
						sdb[n].tract.lock = x;
						sdb[n].status.tractoring = x;
						sdb[x].status.tractored = n;
						sdb[n].power.version = 1;
						sdb[x].power.version = 1;
						return 1;
					} else {
						alert_tract_attempt(n, x, enactor);
					}
				}
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_tract_unlocked (dbref enactor)
{
	int x;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].tract.exist) {
		notify(enactor, ansi_red(tprintf("%s has no tractor beam.", Name(sdb[n].object))));
	} else if (sdb[n].cloak.active && sdb[n].tech.cloak < 2.0) {
		notify(enactor, ansi_red(tprintf("%s cannot use tractor beams while cloaked.", Name(sdb[n].object))));
	} else if (sdb[n].tract.damage <= 0.0) {
		notify(enactor, ansi_red("Tractor beam is inoperative."));
	} else if (!sdb[n].tract.active) {
		notify(enactor, ansi_red("Tractor beam is not online."));
	} else if (!sdb[n].status.tractoring) {
		notify(enactor, ansi_red("Tractor beam is not locked."));
	} else {
		x = sdb[n].status.tractoring;
		alert_tract_unlock(n, x, enactor);
		sdb[n].tract.lock = 0;
		sdb[n].status.tractoring = 0;
		sdb[x].status.tractored = 0;
		sdb[n].power.version = 1;
		sdb[x].power.version = 1;
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_ftr_alloc (double helm, double tactical, double operations, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.helm = fabs(helm);
		sdb[n].alloc.tactical = fabs(tactical);
		sdb[n].alloc.operations = fabs(operations);
		balance_eng_power();
		balance_helm_power();
		balance_shield_power();
		balance_tact_power();
		balance_sensor_power();
		balance_ops_power();
		report_eng_power();
		sdb[n].engine.version = 1;
		sdb[n].sensor.version = 1;
		sdb[n].cloak.version = 1;
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_eng_alloc (double helm, double tactical, double operations, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.helm = fabs(helm);
		sdb[n].alloc.tactical = fabs(tactical);
		sdb[n].alloc.operations = fabs(operations);
		balance_eng_power();
		balance_helm_power();
		balance_shield_power();
		balance_tact_power();
		balance_sensor_power();
		balance_ops_power();
		report_eng_power();
		report_helm_power();
		report_tact_power();
		report_ops_power();
		sdb[n].engine.version = 1;
		sdb[n].sensor.version = 1;
		sdb[n].cloak.version = 1;
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_helm_alloc (double movement, double shields, double cloak, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.movement = fabs(movement);
		sdb[n].alloc.shields = fabs(shields);
		sdb[n].alloc.cloak = fabs(cloak);
		balance_helm_power();
		balance_shield_power();
		report_helm_power();
		sdb[n].engine.version = 1;
		sdb[n].sensor.version = 1;
		sdb[n].cloak.version = 1;
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_shield_alloc (double forward, double starboard, double aft, double port, double dorsal, double ventral, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.shield[0] = fabs(forward);
		sdb[n].alloc.shield[1] = fabs(starboard);
		sdb[n].alloc.shield[2] = fabs(aft);
		sdb[n].alloc.shield[3] = fabs(port);
		sdb[n].alloc.shield[4] = fabs(dorsal);
		sdb[n].alloc.shield[5] = fabs(ventral);
		balance_shield_power();
		report_shield_power();
		sdb[n].engine.version = 1;
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_tactical_alloc (double beams, double missiles, double sensors, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.beams = fabs(beams);
		sdb[n].alloc.missiles = fabs(missiles);
		sdb[n].alloc.sensors = fabs(sensors);
		balance_tact_power();
		balance_sensor_power();
		report_tact_power();
		sdb[n].sensor.version = 1;
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_sensor_alloc (double ecm, double eccm, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.ecm = fabs(ecm);
		sdb[n].alloc.eccm = fabs(eccm);
		balance_sensor_power();
		report_sensor_power();
		sdb[n].sensor.version = 1;
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_operations_alloc (double transporters, double tractors, double miscellaneous, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else {
		sdb[n].alloc.transporters = fabs(transporters);
		sdb[n].alloc.tractors = fabs(tractors);
		sdb[n].alloc.miscellaneous = fabs(miscellaneous);
		balance_ops_power();
		report_ops_power();
		sdb[n].sensor.version = 1;
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_active (dbref enactor)
{
	register int i;

	if (sdb[n].structure.type == 0) {
		notify(enactor, ansi_red("Space object not loaded."));
	} else if (sdb[n].status.crippled == 2) {
		notify(enactor, ansi_red("Space object destroyed."));
	} else if (sdb[n].status.active) {
		notify(enactor, ansi_red(tprintf("%s systems are already active.", Name(sdb[n].object))));
	} else {
		do_all_console_notify(n, ansi_cmd(enactor, "All systems initializing and starting up"));
		do_ship_notify(n, tprintf("%s activates all systems.", Name(sdb[n].object)));
		do_space_notify_one(n, aspace_config.helm, aspace_config.tactical, aspace_config.science,
		  "activates all systems");
		for (i = 0; i < MAX_SHIELD_NAME; ++i)
			sdb[n].shield.active[i] = 0;
		sdb[n].beam.in = 0.0;
		sdb[n].beam.out = 0.0;
		for (i = 0; i < sdb[n].beam.banks; ++i) {
			sdb[n].blist.lock[i] = 0;
			sdb[n].blist.active[i] = 0;
		}
		sdb[n].missile.in = 0.0;
		sdb[n].missile.out = 0.0;
		for (i = 0; i < sdb[n].missile.tubes; ++i) {
			sdb[n].mlist.lock[i] = 0;
			sdb[n].mlist.active[i] = 0;
		}
		sdb[n].main.in = 0.0;
		sdb[n].main.out = 0.0;
		sdb[n].batt.in = 0.0;
		sdb[n].batt.out = 0.0;
		sdb[n].aux.in = 0.0;
		sdb[n].aux.out = 0.0;
		sdb[n].move.in = 0.0;
		sdb[n].move.out = 0.0;
		sdb[n].move.v = 0.0;
		sdb[n].move.time = time(NULL);
		sdb[n].engine.warp_max = 0.0;
		sdb[n].engine.impulse_max = 0.0;
		sdb[n].power.main = 0.0;
		sdb[n].power.aux = 0.0;
		sdb[n].power.batt = 0.0;
		sdb[n].power.total = 0.0;
		sdb[n].sensor.lrs_active = 0;
		sdb[n].sensor.srs_active = 0;
		sdb[n].sensor.ew_active = 0;
		sdb[n].sensor.contacts = 0;
		sdb[n].sensor.counter = 0;
		sdb[n].cloak.active = 0;
		sdb[n].trans.active = 0;
		sdb[n].trans.d_lock = 0;
		sdb[n].trans.s_lock = 0;
		sdb[n].tract.active = 0;
		sdb[n].tract.lock = 0;
		if (GoodSDB(sdb[n].status.tractoring)) {
			sdb[sdb[n].status.tractoring].status.tractored = 0;
			sdb[n].status.tractoring = 0;
		}
		sdb[n].status.active = 1;
		sdb[n].status.time = sdb[n].move.time;
		sdb[n].status.autopilot = 0;
		up_cochranes();
		up_empire();
		up_quadrant();
		up_vectors();
		up_resolution();
		up_signature(n);
		up_visibility();
		do_space_db_write(sdb[n].object, enactor);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_inactive (dbref enactor)
{
	register int i;

	if (sdb[n].structure.type == 0) {
		notify(enactor, ansi_red("Space object not loaded."));
	} else if (sdb[n].status.crippled == 2) {
		notify(enactor, ansi_red("Space object destroyed."));
	} else if (!sdb[n].status.active) {
		notify(enactor, ansi_red(tprintf("%s systems are already inactive.", Name(sdb[n].object))));
	} else if (sdb[n].power.total != 0) {
		notify(enactor, ansi_red(tprintf("%s power systems are still online.", Name(sdb[n].object))));
	} else {
		do_all_console_notify(n, ansi_cmd(enactor, "All systems shutting down"));
		do_ship_notify(n, tprintf("%s deactivates all systems.",Name(sdb[n].object)));
		do_space_notify_one(n, aspace_config.helm, aspace_config.tactical, aspace_config.science,
		  "deactivates all systems");
		for (i = 0; i < MAX_SHIELD_NAME; ++i)
			sdb[n].shield.active[i] = 0;
		sdb[n].beam.in = 0.0;
		sdb[n].beam.out = 0.0;
		for (i = 0; i < sdb[n].beam.banks; ++i) {
			sdb[n].blist.lock[i] = 0;
			sdb[n].blist.active[i] = 0;
		}
		sdb[n].missile.in = 0.0;
		sdb[n].missile.out = 0.0;
		for (i = 0; i < sdb[n].missile.tubes; ++i) {
			sdb[n].mlist.lock[i] = 0;
			sdb[n].mlist.active[i] = 0;
		}
		sdb[n].main.in = 0.0;
		sdb[n].main.out = 0.0;
		sdb[n].batt.in = 0.0;
		sdb[n].batt.out = 0.0;
		sdb[n].aux.in = 0.0;
		sdb[n].aux.out = 0.0;
		sdb[n].move.in = 0.0;
		sdb[n].move.out = 0.0;
		sdb[n].move.v = 0.0;
		sdb[n].engine.warp_max = 0.0;
		sdb[n].engine.impulse_max = 0.0;
		sdb[n].power.main = 0.0;
		sdb[n].power.aux = 0.0;
		sdb[n].power.batt = 0.0;
		sdb[n].power.total = 0.0;
		sdb[n].sensor.lrs_active = 0;
		sdb[n].sensor.srs_active = 0;
		sdb[n].sensor.ew_active = 0;
		sdb[n].sensor.contacts = 0;
		sdb[n].sensor.counter = 0;
		sdb[n].cloak.active = 0;
		sdb[n].trans.active = 0;
		sdb[n].trans.d_lock = 0;
		sdb[n].trans.s_lock = 0;
		sdb[n].tract.active = 0;
		sdb[n].tract.lock = 0;
		if (GoodSDB(sdb[n].status.tractoring)) {
			sdb[sdb[n].status.tractoring].status.tractored = 0;
			sdb[n].status.tractoring = 0;
		}
		sdb[n].status.active = 0;
		sdb[n].status.time = sdb[n].move.time;
		sdb[n].status.autopilot = 0;
		up_cochranes();
		up_empire();
		up_quadrant();
		up_vectors();
		up_resolution();
		up_signature(n);
		up_visibility();
		do_space_db_write(sdb[n].object, enactor);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_docking_doors (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].structure.has_docking_bay) {
		notify(enactor, ansi_red(tprintf("%s has no docking bay.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].status.open_docking) {
				notify(enactor, ansi_red("Docking bay doors are already open."));
			} else {
				sdb[n].status.open_docking = 1;
				console_message(n, "engineering helm operation",
					ansi_cmd(enactor, "Docking bay doors open"));
				return 1;
			}
		} else {
			if (!sdb[n].status.open_docking) {
				notify(enactor, ansi_red("Docking bay doors are already closed."));
			} else {
				sdb[n].status.open_docking = 0;
				console_message(n, "engineering helm operation",
					ansi_cmd(enactor, "Docking bay doors closed"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_landing_doors (int active, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].structure.has_landing_pad) {
		notify(enactor, ansi_red(tprintf("%s has no landing bay.", Name(sdb[n].object))));
	} else {
		if (active) {
			if (sdb[n].status.open_landing) {
				notify(enactor, ansi_red("Landing bay doors are already open."));
			} else {
				sdb[n].status.open_landing = 1;
				console_message(n, "engineering helm operation",
					ansi_cmd(enactor, "Landing bay doors open"));
				return 1;
			}
		} else {
			if (!sdb[n].status.open_landing) {
				notify(enactor, ansi_red("Landing bay doors are already closed."));
			} else {
				sdb[n].status.open_landing = 0;
				console_message(n, "engineering helm operation",
					ansi_cmd(enactor, "Landing bay doors closed"));
				return 1;
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_dock (int contact, dbref enactor)
{
	register int i;
	int x, l = sdb[n].location;
	//int s;
	/* ASPACE TRAC DOCK CHANGES n is current ship, x is the dock sdb[n].status.tractoring is the tractoree */
    int tsize = 0; /* to put tractoree's size */
	//int tloc = 0; /* to put tractoree's loc */
	int trac = sdb[n].status.tractoring; /* the sdb of the tractoree. */
    if (trac) {
		tsize = sdb[sdb[n].status.tractoring].structure.displacement;
		//tloc = sdb[sdb[n].status.tractoring].location; /* Removed due to not being used. */
	}

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].structure.can_dock) {
		notify(enactor, ansi_red(tprintf("%s is incapable of docking.", Name(sdb[n].object))));
/* check if the tractoree can dock   ok'd 8/23/00 */
	} else if (trac && !sdb[trac].structure.can_dock) {
		notify(enactor, ansi_red(tprintf("The ship you are tractoring (%s) is incapable of docking.",Name(sdb[trac].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (sdb[n].status.tractored) {
		notify(enactor, ansi_red(tprintf("%s cannot dock while being tractored.", Name(sdb[n].object))));
	/* We don't need this really, but i'll leave it just in case you want to revert */
/*	} else if (sdb[n].status.tractoring) {
		notify(enactor, ansi_red(tprintf("%s cannot dock while using its tractor beam.", Name(sdb[n].object)))); */
	} else if (fabs(sdb[n].move.out) >= 1.0) {
		notify(enactor, ansi_red(tprintf("%s cannot dock while at warp.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else if (sdb2range(n, x) > MAX_DOCKING_DISTANCE) {
			notify(enactor, ansi_red("That is too far away to dock with."));
		} else if (!sdb[x].structure.has_docking_bay) {
			notify(enactor, ansi_red(tprintf("%s does not have a dock.", Name(sdb[x].object))));
		} else if (!sdb[x].status.open_docking) {
			notify(enactor, ansi_red(tprintf("%s's docking bay doors are closed.", Name(sdb[x].object))));
/* check size on dock. include the tractoree! */
		} else if ((tsize + sdb[n].structure.displacement + sdb[x].structure.cargo_mass) > sdb[x].structure.cargo_hold) {
				notify(enactor, ansi_red(tprintf("%s does not have enough room.", Name(sdb[x].object))));
		} else if (sdb[n].shield.active[0] || sdb[n].shield.active[1] || sdb[n].shield.active[2] || sdb[n].shield.active[3] || sdb[n].shield.active[4] || sdb[n].shield.active[5]) {
		    	notify(enactor, ansi_red(tprintf("%s must lower all shields to dock.", Name(sdb[n].object))));
		} else if (sdb2dissipation(x, sdb2shield(x, n)) != 0.0) {
			notify(enactor, ansi_red(tprintf("%s's facing shield must be lowered.", Name(sdb[x].object))));
		} else if (sdb[x].status.tractored) {
			notify(enactor, ansi_red(tprintf("%s is being tractored.", Name(sdb[x].object))));
		} else {
			if (sdb[n].status.docked) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s slides out of dock.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "undocking from");
			} else if (sdb[n].status.landed) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s lifts off from its landing pad.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "launching from");
			}

			moveit(sdb[n].object, sdb[x].object, 1);
			/* Aspace trac dock. Move the tractoree! */
			if (trac)
				moveit(sdb[sdb[n].status.tractoring].object, sdb[x].object, 1);

			console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s docking with %s", Name(sdb[n].object), Name(sdb[x].object))));
			console_message(x, "helm science tactical", ansi_alert(tprintf("%s docking with %s", Name(sdb[n].object), Name(sdb[x].object))));
			do_ship_notify(n, tprintf("The %s slides into dock.", Name(sdb[n].object)));
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "docking with");
			
			/* Notify space and consoles the tractoree slid in as well... */
			if (trac) {
				/* let the tractorer know what he did */
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s tractors %s into dock with %s", Name(sdb[n].object), Name(sdb[trac].object),Name(sdb[x].object))));
				console_message(x, "helm science tactical", ansi_alert(tprintf("%s tractors %s into dock with %s", Name(sdb[n].object), Name(sdb[trac].object),Name(sdb[x].object))));
				do_ship_notify(trac, tprintf("The %s pulls %s into dock.", Name(sdb[n].object),Name(sdb[trac].object)));
				do_space_notify_two(trac, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "docking with");
			}


			/* where we were */
			/* don't need to duplicate this process for tractoree */
			/* its always in space, since you can't tractor docked objects */
			if (l > 0 && l < max_space_objects) {
				sdb[l].structure.cargo_mass -= sdb[n].structure.displacement;
				if (sdb[l].structure.cargo_mass < 0.0)
					sdb[l].structure.cargo_mass = 0.0;
				sdb[l].engine.version = 1;
				up_signature(l);
			}
			
			
			/* where we are going */
			sdb[x].structure.cargo_mass += sdb[n].structure.displacement;
			sdb[x].engine.version = 1;
			up_signature(x);
			
			/* do the same for the tractoree */
			if(trac) {
			sdb[x].structure.cargo_mass += sdb[trac].structure.displacement;
			sdb[x].engine.version = 1;
			up_signature(x);
			}
			
			/* break the lock, but don't announce! */
			if(trac) {
			sdb[n].tract.lock = 0;
			sdb[n].status.tractoring = 0;
			sdb[trac].status.tractored = 0;
			sdb[n].power.version = 1;
			sdb[trac].power.version = 1;
			}

			/* the nitty gritty goes here. do the actual moving */
			sdb[n].space = sdb[x].space;
			sdb[n].location = x;
			for (i = 0; i < sdb[n].beam.banks; ++i)
				sdb[n].blist.lock[i] = 0;
			for (i = 0; i < sdb[n].missile.tubes; ++i)
				sdb[n].mlist.lock[i] = 0;
			sdb[n].sensor.contacts = 0;
			sdb[n].sensor.counter = 0;
			sdb[n].move.in = 0;
			sdb[n].move.out = 0;
			sdb[n].move.v = 0;
			sdb[n].move.empire = sdb[x].move.empire;
			sdb[n].move.quadrant = sdb[x].move.quadrant;
			if (sdb[n].trans.d_lock != n)
				sdb[n].trans.d_lock = 0;
			if (sdb[n].trans.s_lock != n)
				sdb[n].trans.s_lock = 0;
			sdb[n].coords.x = sdb[x].coords.x;
			sdb[n].coords.y = sdb[x].coords.y;
			sdb[n].coords.z = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb[n].course.yaw_out = sdb[x].course.yaw_out;
			sdb[n].course.pitch_in = sdb[n].course.pitch_out = sdb[x].course.pitch_out;
			sdb[n].course.roll_in = sdb[n].course.roll_out = sdb[x].course.roll_out;
			sdb[n].status.docked = x;
			sdb[n].status.landed = 0;
			sdb[n].status.autopilot = 0;
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "docking with");
			up_cochranes();
			up_empire();
			up_quadrant();
			up_vectors();
			up_resolution();
			up_signature(n);
			up_visibility();
			do_space_db_write(sdb[n].object, enactor);

			
			/* do the nitty gritty for the tractoree (assuming there is one) */
			if(trac) {
			sdb[trac].space = sdb[x].space;
			sdb[trac].location = x;
			for (i = 0; i < sdb[trac].beam.banks; ++i)
				sdb[n].blist.lock[trac] = 0;
			for (i = 0; i < sdb[trac].missile.tubes; ++i)
				sdb[n].mlist.lock[i] = 0;
			sdb[trac].sensor.contacts = 0;
			sdb[trac].sensor.counter = 0;
			sdb[trac].move.in = 0;
			sdb[trac].move.out = 0;
			sdb[trac].move.v = 0;
			sdb[trac].move.empire = sdb[x].move.empire;
			sdb[trac].move.quadrant = sdb[x].move.quadrant;
			if (sdb[trac].trans.d_lock != n)
				sdb[trac].trans.d_lock = 0;
			if (sdb[trac].trans.s_lock != n)
				sdb[trac].trans.s_lock = 0;
			sdb[trac].coords.x = sdb[x].coords.x;
			sdb[trac].coords.y = sdb[x].coords.y;
			sdb[trac].coords.z = sdb[x].coords.z;
			sdb[trac].course.yaw_in = sdb[trac].course.yaw_out = sdb[x].course.yaw_out;
			sdb[trac].course.pitch_in = sdb[trac].course.pitch_out = sdb[x].course.pitch_out;
			sdb[trac].course.roll_in = sdb[trac].course.roll_out = sdb[x].course.roll_out;
			sdb[trac].status.docked = x;
			sdb[trac].status.landed = 0;
			sdb[trac].status.autopilot = 0;
			do_space_notify_two(trac, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "docking with");
			up_cochranes();
			up_empire();
			up_quadrant();
			up_vectors();
			up_resolution();
			up_signature(n);
			up_visibility();
						
			do_space_db_write(sdb[trac].object, enactor);
			}
			
			return 1;

		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_undock (dbref enactor)
{
	register int i;
	int x, l = sdb[n].location;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is not in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!SpaceObj(sdb[l].object) || !GoodObject(sdb[l].object)) {
		notify(enactor, ansi_red("Undocking error."));
		write_spacelog(enactor, n, "BUG: Bad undocking location Object");
	} else {
		x = 0;
		for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i) {
			if (i == sdb[n].location)
				x = i;
		}
		if (!x) {
			notify(enactor, ansi_red("Undocking error."));
			write_spacelog(enactor, n, "BUG: Bad undocking location SDB");
		} else if (!sdb[x].status.open_docking) {
			notify(enactor, ansi_red(tprintf("%s's docking bay doors are closed.", Name(sdb[x].object))));
		} else {
			moveit(sdb[n].object, Location(sdb[x].object), 1);
			console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[x].object))));
			console_message(x, "helm science tactical", ansi_alert(tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[x].object))));
			do_ship_notify(n, tprintf("The %s slides out of dock.", Name(sdb[n].object)));
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "undocking from");

			/* where we are going */
			l = sdb[x].location;
			if (l > 0 && l <= max_space_objects) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s docking with %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s docking with %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s slides into dock.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "docking with");
				sdb[l].structure.cargo_mass += sdb[n].structure.displacement;
				sdb[l].engine.version = 1;
				up_signature(l);
			}

			/* where we were */
			sdb[x].structure.cargo_mass -= sdb[n].structure.displacement;
			if (sdb[x].structure.cargo_mass < 0.0)
				sdb[x].structure.cargo_mass = 0.0;
			sdb[x].engine.version = 1;
			up_signature(x);

			/* we */
			sdb[n].space = sdb[x].space;
			sdb[n].location = sdb[x].location;
			for (i = 0; i < sdb[n].beam.banks; ++i)
				sdb[n].blist.lock[i] = 0;
			for (i = 0; i < sdb[n].missile.tubes; ++i)
				sdb[n].mlist.lock[i] = 0;
			sdb[n].sensor.contacts = 0;
			sdb[n].sensor.counter = 0;
			sdb[n].move.in = 0;
			sdb[n].move.out = 0;
			sdb[n].move.v = 0;
			sdb[n].move.empire = sdb[x].move.empire;
			sdb[n].move.quadrant = sdb[x].move.quadrant;
			if (sdb[n].trans.d_lock != n)
				sdb[n].trans.d_lock = 0;
			if (sdb[n].trans.s_lock != n)
				sdb[n].trans.s_lock = 0;
			sdb[n].coords.x = sdb[x].coords.x;
			sdb[n].coords.y = sdb[x].coords.y;
			sdb[n].coords.z = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb[n].course.yaw_out = sdb[x].course.yaw_out;
			sdb[n].course.pitch_in = sdb[n].course.pitch_out = sdb[x].course.pitch_out;
			sdb[n].course.roll_in = sdb[n].course.roll_out = sdb[x].course.roll_out;
			sdb[n].status.docked = l;
			sdb[n].status.landed = 0;
			sdb[n].status.autopilot = 0;
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "undocking from");
			up_cochranes();
			up_empire();
			up_quadrant();
			up_vectors();
			up_resolution();
			up_signature(n);
			up_visibility();
			do_space_db_write(sdb[n].object, enactor);
			return 1;

		}
	}
	return 0;
}

/* -------------------------------------------------------------------- */

int do_set_land (int contact, dbref enactor)
{
	register int i;
	int x, l = sdb[n].location;
	//int s;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].structure.can_land) {
		notify(enactor, ansi_red(tprintf("%s is incapable of landing.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (sdb[n].status.tractored) {
		notify(enactor, ansi_red(tprintf("%s cannot land while being tractored.", Name(sdb[n].object))));
	} else if (sdb[n].status.tractoring) {
		notify(enactor, ansi_red(tprintf("%s cannot land while using its tractor beam.", Name(sdb[n].object))));
	} else if (fabs(sdb[n].move.out) >= 1.0) {
		notify(enactor, ansi_red(tprintf("%s cannot land while at warp.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else if (sdb2range(n, x) > MAX_LANDING_DISTANCE) {
			notify(enactor, ansi_red("That is too far away to land on."));
		} else if (!sdb[x].structure.has_landing_pad) {
			notify(enactor, ansi_red(tprintf("%s does not have a landing pad.", Name(sdb[x].object))));
		} else if (!sdb[x].status.open_landing) {
			notify(enactor, ansi_red(tprintf("%s's landing bay doors are closed.", Name(sdb[x].object))));
		} else if ((sdb[n].structure.displacement + sdb[x].structure.cargo_mass) > sdb[x].structure.cargo_hold) {
			notify(enactor, ansi_red(tprintf("%s does not have enough room.", Name(sdb[x].object))));
		} else if (sdb[n].shield.active[0] || sdb[n].shield.active[1] || sdb[n].shield.active[2] || sdb[n].shield.active[3] || sdb[n].shield.active[4] || sdb[n].shield.active[5]) {
			notify(enactor, ansi_red(tprintf("%s must lower all shields to land.", Name(sdb[n].object))));
		} else if (sdb2dissipation(x, sdb2shield(x, n)) != 0.0) {
			notify(enactor, ansi_red(tprintf("%s's facing shield must be lowered.", Name(sdb[x].object))));
		} else if (sdb[x].status.tractored) {
			notify(enactor, ansi_red(tprintf("%s is being tractored.", Name(sdb[x].object))));
		} else {
			if (sdb[n].status.docked) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s undocking from %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s slides out of dock.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "undocking from");
			} else if (sdb[n].status.landed) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s lifts off from its landing pad.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "launching from");
			}

			moveit(sdb[n].object, sdb[x].object, 1);
			console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s landing on %s", Name(sdb[n].object), Name(sdb[x].object))));
			console_message(x, "helm science tactical", ansi_alert(tprintf("%s landing on %s", Name(sdb[n].object), Name(sdb[x].object))));
			do_ship_notify(n, tprintf("The %s settles onto its landing pad.", Name(sdb[n].object)));
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "landing on");

			/* where we were */
			if (l > 0 && l < max_space_objects) {
				sdb[l].structure.cargo_mass -= sdb[n].structure.displacement;
				if (sdb[l].structure.cargo_mass < 0.0)
					sdb[l].structure.cargo_mass = 0.0;
				sdb[l].engine.version = 1;
				up_signature(l);
			}

			/* where we are going */
			sdb[x].structure.cargo_mass += sdb[n].structure.displacement;
			sdb[x].engine.version = 1;
			up_signature(x);

			/* we */
			for (i = 0; i < sdb[n].beam.banks; ++i)
				sdb[n].blist.lock[i] = 0;
			for (i = 0; i < sdb[n].missile.tubes; ++i)
				sdb[n].mlist.lock[i] = 0;
			sdb[n].space = sdb[x].space;
			sdb[n].location = x;
			sdb[n].sensor.contacts = 0;
			sdb[n].sensor.counter = 0;
			sdb[n].move.in = 0;
			sdb[n].move.out = 0;
			sdb[n].move.v = 0;
			sdb[n].move.empire = sdb[x].move.empire;
			sdb[n].move.quadrant = sdb[x].move.quadrant;
			if (sdb[n].trans.d_lock != n)
				sdb[n].trans.d_lock = 0;
			if (sdb[n].trans.s_lock != n)
				sdb[n].trans.s_lock = 0;
			sdb[n].coords.x = sdb[x].coords.x;
			sdb[n].coords.y = sdb[x].coords.y;
			sdb[n].coords.z = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb[n].course.yaw_out = sdb[x].course.yaw_out;
			sdb[n].course.pitch_in = sdb[n].course.pitch_out = sdb[x].course.pitch_out;
			sdb[n].course.roll_in = sdb[n].course.roll_out = sdb[x].course.roll_out;
			sdb[n].status.landed = x;
			sdb[n].status.docked = 0;
			sdb[n].status.autopilot = 0;
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "landing on");
			up_cochranes();
			up_empire();
			up_quadrant();
			up_vectors();
			up_resolution();
			up_signature(n);
			up_visibility();
			do_space_db_write(sdb[n].object, enactor);
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_launch (dbref enactor)
{
	register int i;
	int x, l = sdb[n].location;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is not on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!SpaceObj(sdb[l].object) || !GoodObject(sdb[l].object)) {
		notify(enactor, ansi_red("Launching error."));
		write_spacelog(enactor, n, "BUG: Bad launching location Object");
	} else {
		x = 0;
		for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i) {
			if (i == sdb[n].location)
				x = i;
		}
		if (!x) {
			notify(enactor, ansi_red("Launching error."));
			write_spacelog(enactor, n, "BUG: Bad launching location SDB");
		} else if (!sdb[x].status.open_landing) {
			notify(enactor, ansi_red(tprintf("%s's landing bay doors are closed.", Name(sdb[x].object))));
		} else {
			moveit(sdb[n].object, Location(sdb[x].object), 1);
			console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[x].object))));
			console_message(x, "helms cience tactical", ansi_alert(tprintf("%s launching from %s", Name(sdb[n].object), Name(sdb[x].object))));
			do_ship_notify(n, tprintf("The %s lifts off from its landing pad.", Name(sdb[n].object)));
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "launching from");

			/* where we are going */
			l = sdb[x].location;
			if (l > 0 && l <= max_space_objects) {
				console_message(n, "helm science tactical", ansi_cmd(enactor, tprintf("%s landing on %s", Name(sdb[n].object), Name(sdb[l].object))));
				console_message(l, "helm science tactical", ansi_alert(tprintf("%s landing on %s", Name(sdb[n].object), Name(sdb[l].object))));
				do_ship_notify(n, tprintf("The %s settles onto its landing pad.", Name(sdb[n].object)));
				do_space_notify_two(n, l, aspace_config.helm, aspace_config.tactical, aspace_config.science, "landing on");
				sdb[l].structure.cargo_mass += sdb[n].structure.displacement;
				sdb[l].engine.version = 1;
				up_signature(l);
			}

			/* where we were */
			sdb[x].structure.cargo_mass -= sdb[n].structure.displacement;
			if (sdb[x].structure.cargo_mass < 0.0)
				sdb[x].structure.cargo_mass = 0.0;
			sdb[x].engine.version = 1;
			up_signature(x);

			/* we */
			sdb[n].space = sdb[x].space;
			sdb[n].location = sdb[x].location;
			for (i = 0; i < sdb[n].beam.banks; ++i)
				sdb[n].blist.lock[i] = 0;
			for (i = 0; i < sdb[n].missile.tubes; ++i)
				sdb[n].mlist.lock[i] = 0;
			sdb[n].sensor.contacts = 0;
			sdb[n].sensor.counter = 0;
			sdb[n].move.in = 0;
			sdb[n].move.out = 0;
			sdb[n].move.v = 0;
			sdb[n].move.empire = sdb[x].move.empire;
			sdb[n].move.quadrant = sdb[x].move.quadrant;
			if (sdb[n].trans.d_lock != n)
				sdb[n].trans.d_lock = 0;
			if (sdb[n].trans.s_lock != n)
				sdb[n].trans.s_lock = 0;
			sdb[n].coords.x = sdb[x].coords.x;
			sdb[n].coords.y = sdb[x].coords.y;
			sdb[n].coords.z = sdb[x].coords.z;
			sdb[n].course.yaw_in = sdb[n].course.yaw_out = sdb[x].course.yaw_out;
			sdb[n].course.pitch_in = sdb[n].course.pitch_out = sdb[x].course.pitch_out;
			sdb[n].course.roll_in = sdb[n].course.roll_out = sdb[x].course.roll_out;
			sdb[n].status.landed = l;
			sdb[n].status.docked = 0;
			sdb[n].status.autopilot = 0;
			do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "launching from");
			up_cochranes();
			up_empire();
			up_quadrant();
			up_vectors();
			up_resolution();
			up_signature(n);
			up_visibility();
			do_space_db_write(sdb[n].object, enactor);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------- */

void up_wormhole (int n1, int n2)
{
	int x, tmp_n = n;
	/* Mordak Aspace v1.0.0p1 */
	int tmp_c;
	/* End Aspace v1.0.0p1 */

	n = n1;
	x = n2;
	
	do_ship_notify(n, tprintf("The %s shudders and rocks about violently for a few moments.", Name(sdb[n].object)));
	do_space_notify_two(n, x, aspace_config.helm, aspace_config.tactical, aspace_config.science, "enters");
	if (sdb[n].cloak.active) {
		alert_cloak_voided (n);
		alert_ship_cloak_offline (n);
		sdb[n].cloak.active = 0;
		sdb[n].sensor.version = 1;
		sdb[n].engine.version = 1;
	}
	/* Mordak Aspace v1.0.0p1 Clear Contacts than Gate and reset sensors*/
	for(tmp_c=0; tmp_c<sdb[n].sensor.contacts; tmp_c++) {
	console_message(n, "helm science tactical",
		ansi_warn(tprintf("%s contact lost: %s",
		unparse_type(sdb[n].slist.sdb[tmp_c]),
		unparse_identity(n, sdb[n].slist.sdb[tmp_c]))));
	}
	sdb[n].sensor.contacts=0;
	up_sensor_message(0);
	/* END Aspace v1.0.0p1 */
	sdb[n].space = sdb[sdb[x].status.link].space;
	sdb[n].coords.x = sdb[sdb[x].status.link].coords.x;
	sdb[n].coords.y = sdb[sdb[x].status.link].coords.y;
	sdb[n].coords.z = sdb[sdb[x].status.link].coords.z;
	sdb[n].status.autopilot = 0;
	do_space_notify_one(sdb[x].status.link, aspace_config.helm, aspace_config.tactical, aspace_config.science, "expells an unknown contact");
	up_cochranes();
	up_empire();
	up_quadrant();
	up_resolution();
	up_signature(n);
	up_visibility();
	n = tmp_n;
	return;
}

int do_set_enter_wormhole (int contact, dbref enactor)
{
	int x, t;
	
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].structure.type != 1) {
		notify(enactor, ansi_red(tprintf("%s is much too large to enter.", Name(sdb[n].object))));
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (fabs(sdb[n].move.out) >= 1.0) {
		notify(enactor, ansi_red(tprintf("%s cannot gate while at warp.", Name(sdb[n].object))));
	} else {
		x = contact2sdb(n, contact);
		if (error_on_contact(n, x, enactor)) {
			return 0;
		} else if (sdb[x].structure.type != 4) {
			notify(enactor, ansi_red("That is not an anomaly."));
		} else if (sdb2range(n, x) > MAX_WORMHOLE_DISTANCE) {
		} else if (sdb[x].move.out >= 1.0) {
			notify(enactor, ansi_red(tprintf("%s is moving too fast.", Name(sdb[x].object))));
		} else if (!SpaceObj(sdb[sdb[x].status.link].object) || !GoodObject(sdb[sdb[x].status.link].object)) {
			notify(enactor, ansi_red("Gating error."));
			write_spacelog(enactor, x, "BUG: Bad gating linked Object");
		} else {
			if (sdb[n].status.tractoring) {
				t = sdb[n].status.tractoring;
			} else if (sdb[n].status.tractored) {
				t = sdb[n].status.tractored;
			} else
				t = 0;
			if (t) {
				if (sdb[t].structure.type != 1) {
					notify(enactor, ansi_red(tprintf("%s is much too large to enter.", Name(sdb[t].object))));
					return 0;
				} else if (sdb2range(t, x) > MAX_WORMHOLE_DISTANCE) {
					notify(enactor, ansi_red(tprintf("%s is too far away to enter.", Name(sdb[t].object))));
					return 0;
				} else
					up_wormhole(t, x);
				}
			do_all_console_notify(n, ansi_cmd(enactor, tprintf("%s entering %s", Name(sdb[n].object), Name(sdb[x].object))));
			up_wormhole(n, x);
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_fix_damage (char *sys1, char *sys2, int type, char *name, dbref enactor)
{
	int num, unit = 0, x, flag = 0;
	double *dmg, fix_cost, ss_damage, mult;

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
		notify(enactor, ansi_red(tprintf("%s systems are inactive.", Name(sdb[n].object))));
		return 0;
	} else if (sdb[n].structure.max_repair <= 0) {
		notify(enactor, ansi_red(tprintf("%s has no repair capacity.", Name(sdb[n].object))));
		return 0;
	}

	if (type) {
		x = name2sdb(name);
		if (x == SENSOR_FAIL) {
			notify(enactor, ansi_red("That is not a valid repair recipient."));
			return 0;
		} else if (!GoodSDB(x)) {
			notify(enactor, ansi_red("That is not a valid repair recipient."));
			return 0;
		} else if (sdb[x].location != n) {
			notify(enactor, ansi_red("That is not a valid repair recipient."));
			return 0;
		} else if (sdb[x].structure.type == 0) {
			notify(enactor, ansi_red("That is not a valid repair recipient."));
			write_spacelog(enactor, x, "BUG:Repair recipient has bad TYPE");
			return 0;
		} else if (!sdb[x].status.connected) {
			notify(enactor, ansi_red(tprintf("%s is not connected.", Name(sdb[x].object))));
			return 0;
		} else if (!SpaceObj(sdb[x].object) || !GoodObject(sdb[x].object)) {
			notify(enactor, ansi_red("That is not a valid repair recipient."));
			write_spacelog(enactor, x, "BUG:Repair recipient has bad flags");
			return 0;
		}
	} else {
		x = n;
	}

	switch (sys1[0]) {
		case 'a': case 'A':
			switch (sys1[1]) {
				case 'f': case 'F':
					if (sdb[x].shield.exist) {
						num = 10;
						dmg = &sdb[x].shield.damage[2];
						break;
					}
				case 'u': case 'U':
					if (sdb[x].aux.exist) {
				 		num = 1;
				 		dmg = &sdb[x].aux.damage;
				 		break;
				 	}
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 'b': case 'B':
			switch (sys1[1]) {
				case 'a': case 'A':
					if (sdb[x].batt.exist) {
						num = 2;
						dmg = &sdb[x].batt.damage;
						break;
					}
				case 'e': case 'E':
					if (sdb[x].beam.exist) {
						unit = parse_integer(sys2);
						if (unit > 0 && unit <= sdb[x].beam.banks) {
							num = 3;
							dmg = &sdb[x].blist.damage[unit - 1];
							break;
						}
					}
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 'c': case 'C':
			if (sdb[x].cloak.exist) {
				num = 4;
				dmg = &sdb[x].cloak.damage;
				break;
			}
		case 'd': case 'D':
			if (sdb[x].shield.exist) {
				num = 10;
				dmg = &sdb[x].shield.damage[4];
				break;
			}
		case 'e': case 'E':
			if (sdb[x].sensor.ew_exist) {
				num = 5;
				dmg = &sdb[x].sensor.ew_damage;
				break;
			}
		case 'f': case 'F':
			switch (sys1[1]) {
				case 'o': case 'O':
					if (sdb[x].shield.exist) {
						num = 10;
						dmg = &sdb[x].shield.damage[0];
						break;
					}
				case 'u': case 'U':
					if (sdb[x].aux.exist) {
						num = 1;
						dmg = &sdb[x].aux.damage;
						break;
					}
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 'i': case 'I':
			if (sdb[x].engine.impulse_exist) {
				num = 6;
				dmg = &sdb[x].engine.impulse_damage;
				break;
			}
		case 'l': case 'L':
			if (sdb[x].sensor.lrs_exist) {
				num = 7;
				dmg = &sdb[x].sensor.lrs_damage;
				break;
			}
		case 'm': case 'M':
			switch (sys1[1]) {
				case 'a': case 'A': case '/':
					if (sdb[x].main.exist) {
						num = 8;
						dmg = &sdb[x].main.damage;
						break;
					}
				case 'i': case 'I':
					if (sdb[x].missile.exist) {
						unit = parse_integer(sys2);
						if (unit > 0 && unit <= sdb[x].missile.tubes) {
							num = 9;
							dmg = &sdb[x].mlist.damage[unit - 1];
							break;
						}
					}
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 'p': case 'P':
			if (sdb[x].shield.exist) {
				num = 10;
				dmg = &sdb[x].shield.damage[3];
				break;
			}
		case 's': case 'S':
			switch (sys1[1]) {
				case 'r': case 'R':
					if (sdb[x].sensor.srs_exist) {
						num = 11;
						dmg = &sdb[x].sensor.srs_damage;
						break;
					}
				case 't': case 'T':
					if (sdb[x].shield.exist) {
					 	num = 10;
					 	dmg = &sdb[x].shield.damage[1];
					 	break;
					 }
				case 'u': case 'U':
					num = 0;
					ss_damage = sdb[x].structure.superstructure / sdb[x].structure.max_structure;
					dmg = &ss_damage;
					break;
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 't': case 'T':
			switch (sys1[3]) {
				case 'c': case 'C':
					if (sdb[x].tract.exist) {
						num = 12;
						dmg = &sdb[x].tract.damage;
						break;
					}
				case 'n': case 'N':
					if (sdb[x].trans.exist) {
						num = 13;
						dmg = &sdb[x].trans.damage;
						break;
					}
				default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
			} break;
		case 'v': case 'V':
			if (sdb[x].shield.exist) {
				num = 10;
				dmg = &sdb[x].shield.damage[5];
				break;
			}
		case 'w': case 'W':
			if (sdb[x].engine.warp_exist) {
				num = 14;
				dmg = &sdb[x].engine.warp_damage;
				break;
			}
		default: notify(enactor, ansi_red("Invalid system specification.")); return 0; break;
	}

	if (*dmg >= 1.0) {
		notify(enactor, ansi_red(tprintf("%s on %s has no damage to repair.",
		  system_name[num], Name(sdb[x].object))));
		*dmg = 1.0;
		return 0;
	} else if ((*dmg <= -1.0) && (!sdb[n].structure.has_docking_bay) && num) {
		notify(enactor, ansi_red(tprintf("%s on %s has been totally destroyed.",
		  system_name[num], Name(sdb[x].object))));
		return 0;
	}

	if (num) {
		switch (num) {
			case 1: mult = sdb[x].aux.gw; break;
			case 2: mult = sdb[x].batt.gw; break;
			case 3: mult = sdb[x].blist.cost[unit - 1]; break;
			case 8: mult = sdb[x].main.gw; break;
			case 9: mult = sdb[x].mlist.warhead[unit - 1]; break;
			default: mult = (1.0 + (sdb[x].structure.max_structure / 10.0)); break;
		}
		fix_cost = (1 - *dmg) * repair_mult[num] * mult * 100.0;
		if (fix_cost > sdb[n].structure.repair) {
			*dmg += sdb[n].structure.repair / repair_mult[num] / mult / 100.0;
			sdb[n].structure.repair = 0.0;
			console_message(n, "damage",
			  ansi_cmd(enactor, tprintf("%s repaired to %s",
			  system_name[num], unparse_percent(*dmg))));
			if (type)
				console_message(x, "damage",
				  ansi_alert(tprintf("%s repaired to %s by %s",
				  system_name[num], unparse_percent(*dmg),
				  Name(sdb[n].object))));
		} else {
			*dmg = 1.0;
			sdb[n].structure.repair -= fix_cost;
			console_message(n, "damage",
			  ansi_cmd(enactor, tprintf("%s repairs complete", system_name[num])));
			if (type)
				console_message(x, "damage",
				  ansi_alert(tprintf("%s repairs completed by %s",
				  system_name[num], Name(sdb[n].object))));
		}
		sdb[x].sensor.version = 1;
		sdb[x].engine.version = 1;
		sdb[x].power.version = 1;
		sdb[x].cloak.version = 1;
		sdb[x].status.time = sdb[x].move.time;
		up_cochranes();
		up_empire();
		up_quadrant();
		up_vectors();
		up_resolution();
		up_signature(x);
		up_visibility();
		debug_space(x);
		return 1;
	} else {
		if (sdb[x].structure.superstructure < 0.0)
			flag = 1;
		fix_cost = (sdb[x].structure.max_structure - sdb[x].structure.superstructure) * repair_mult[0];
		if (fix_cost > sdb[n].structure.repair) {
			sdb[x].structure.superstructure += sdb[n].structure.repair / repair_mult[0];
			sdb[n].structure.repair = 0.0;
			console_message(n, "damage",
			  ansi_cmd(enactor, tprintf("%s repaired to %s", system_name[0],
			  unparse_percent(sdb[x].structure.superstructure / sdb[x].structure.max_structure))));
			if (type)
				console_message(x, "damage",
				  ansi_alert(tprintf("%s repaired to %s by %s", system_name[0],
				  unparse_percent(sdb[x].structure.superstructure / sdb[x].structure.max_structure),
				  Name(sdb[n].object))));
		} else {
			sdb[x].structure.superstructure = sdb[x].structure.max_structure;
			sdb[n].structure.repair -= fix_cost;
			console_message(n, "damage",
			  ansi_cmd(enactor, tprintf("%s repairs complete", system_name[0])));
			if (type)
				console_message(x, "damage",
				  ansi_alert(tprintf("%s repairs completed by %s", system_name[0],
				  Name(sdb[n].object))));
		}
		if (flag && sdb[x].structure.superstructure >= 0.0) {
			sdb[x].status.crippled = 0;
			do_ship_notify(x, tprintf("%s recovers from total systems failure.",
			  Name(sdb[x].object)));
		}
		sdb[x].sensor.version = 1;
		sdb[x].engine.version = 1;
		sdb[x].power.version = 1;
		sdb[x].cloak.version = 1;
		sdb[x].status.time = sdb[x].move.time;
		up_cochranes();
		up_empire();
		up_quadrant();
		up_vectors();
		up_resolution();
		up_signature(x);
		up_visibility();
		debug_space(x);
		return 1;
	}
}

/* ------------------------------------------------------------------------ */

int do_set_shield_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].shield.exist) {
		notify(enactor, ansi_red(tprintf("%s has no shields.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid shield frequencies are from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].shield.freq = frequency;
		console_message(n, "engineering helm",
			ansi_cmd(enactor, tprintf("Shield frequencies set to %.3f GHz",
				sdb[n].shield.freq)));
		write_spacelog(enactor, sdb[n].object, tprintf("LOG: Shield frequencies set to %.6f GHz",
			sdb[n].shield.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_cloak_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].cloak.exist) {
		notify(enactor, ansi_red(tprintf("%s has no cloaking device.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid cloaking device frequencies are from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].cloak.freq = frequency;
		console_message(n, "engineering helm",
			ansi_cmd(enactor, tprintf("Cloaking device frequency set to %.3f GHz",
				sdb[n].cloak.freq)));
                write_spacelog(enactor, sdb[n].object, tprintf("LOG: Cloaking device frequency set to %.6f GHz",
                	sdb[n].cloak.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_beam_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].beam.exist) {
		notify(enactor, ansi_red(tprintf("%s has no beam weapons.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid beam weapon frequencies are from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].beam.freq = frequency;
		console_message(n, "engineering tactical",
			ansi_cmd(enactor, tprintf("Beam weapon frequencies set to %.3f GHz", sdb[n].beam.freq)));
                write_spacelog(enactor, sdb[n].object, tprintf("LOG: Beam weapon frequencies set to %.6f GHz",
                	sdb[n].beam.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_missile_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].missile.exist) {
		notify(enactor, ansi_red(tprintf("%s has no missile weapons.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid missile weapon frequencies are from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].missile.freq = frequency;
		console_message(n, "engineering tactical",
			ansi_cmd(enactor, tprintf("Missile weapon frequencies set to %.3f GHz", sdb[n].missile.freq)));
                write_spacelog(enactor, sdb[n].object, tprintf("LOG: Missile weapon frequencies set to %.6f GHz",
                	sdb[n].missile.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_trans_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].trans.exist) {
		notify(enactor, ansi_red(tprintf("%s has no transporters.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid transporter frequencies are from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].trans.freq = frequency;
		console_message(n, "engineering operation transporter",
			ansi_cmd(enactor, tprintf("Transporter frequency set to %.3f GHz", sdb[n].trans.freq)));
                write_spacelog(enactor, sdb[n].object, tprintf("LOG: Transporter frequency set to %.6f GHz",
                	sdb[n].trans.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_tract_freq (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].tract.exist) {
		notify(enactor, ansi_red(tprintf("%s has no tractor beam.", Name(sdb[n].object))));
	} else if (frequency < 1.0 || frequency >= 1000.0) {
		notify(enactor, ansi_red("Valid tractor beam frequency is from 1.00 to 999.999 GHz."));
	} else {
		sdb[n].tract.freq = frequency;
		console_message(n, "engineering operation",
			ansi_cmd(enactor, tprintf("Tractor beam frequency set to %.3f GHz", sdb[n].tract.freq)));
                write_spacelog(enactor, sdb[n].object, tprintf("LOG: Tractor beam frequency set to %.6f GHz",
                	sdb[n].tract.freq));
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_refuel (char *name, char *fuel, double tons, dbref enactor)
{
	int x, f;
	double amount, capacity, available;

	if (error_on_console(enactor))
		return 0;

	switch (fuel[0]) {
		case 'A': case 'a': f = 1; break;
		case 'D': case 'd': f = 2; break;
		case 'R': case 'r': f = 3; break;
		default: f = 0; break;
	}
	if (!f) {
		notify(enactor, ansi_red("That is not a valid fuel type."));
		return 0;
	}
	if (tons <= 0.0) {
		notify(enactor, ansi_red("That is not a valid fuel amount."));
		return 0;
	}
	x = name2sdb(name);
	if (x == SENSOR_FAIL) {
		notify(enactor, ansi_red("That is not a valid fuel recipient."));
	} else if (!GoodSDB(x)) {
		notify(enactor, ansi_red("That is not a valid fuel recipient."));
	} else if (sdb[x].location != n) {
		notify(enactor, ansi_red("That is not a valid fuel recipient."));
	} else if (sdb[x].structure.type == 0) {
		notify(enactor, ansi_red("That is not a valid fuel recipient."));
		write_spacelog(enactor, x, "BUG:Fuel recipient has bad TYPE");
	} else if (!sdb[x].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is not connected.", Name(sdb[x].object))));
	} else if (!SpaceObj(sdb[x].object) || !GoodObject(sdb[x].object)) {
		notify(enactor, ansi_red("That is not a valid fuel recipient."));
		write_spacelog(enactor, x, "BUG:Fuel recipient has bad flags");
	} else if (f == 1) {
		amount = tons * 1000000.0;
		available = sdb[n].fuel.antimatter;
		capacity = sdb2max_antimatter(x) - sdb[x].fuel.antimatter;
		if (amount > available) {
			notify(enactor, ansi_red(tprintf("%s has only %f tons of antimatter available",
				Name(sdb[n].object), available / 1000000.0)));
		} else if (capacity <= 0) {
			notify(enactor, ansi_red(tprintf("%s has no antimatter capacity",
				Name(sdb[x].object))));
		} else if (amount > capacity) {
			notify(enactor, ansi_red(tprintf("%s has only %f tons of antimatter capacity",
				Name(sdb[x].object), capacity / 1000000.0)));
		} else {
			sdb[n].fuel.antimatter -= amount;
			sdb[x].fuel.antimatter += amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f tons of antimatter transferred to %s",
				amount / 1000000.0, Name(sdb[x].object))));
			console_message(x, "engineering operation",
				ansi_alert(tprintf("%f tons of antimatter transferred from %s",
				amount / 1000000.0, Name(sdb[n].object))));
			return 1;
		}
	} else if (f == 2) {
		amount = tons * 1000000.0;
		available = sdb[n].fuel.deuterium;
		capacity = sdb2max_deuterium(x) - sdb[x].fuel.deuterium;
		if (amount > available) {
			notify(enactor, ansi_red(tprintf("%s has only %f tons of deuterium available",
				Name(sdb[n].object), available / 1000000.0)));
		} else if (capacity <= 0) {
			notify(enactor, ansi_red(tprintf("%s has no deuterium capacity",
				Name(sdb[x].object))));
		} else if (amount > capacity) {
			notify(enactor, ansi_red(tprintf("%s has only %f tons of deuterium capacity",
				Name(sdb[x].object), capacity / 1000000.0)));
		} else {
			sdb[n].fuel.deuterium -= amount;
			sdb[x].fuel.deuterium += amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f tons of deuterium transferred to %s",
				amount / 1000000.0, Name(sdb[x].object))));
			console_message(x, "engineering operation",
				ansi_alert(tprintf("%f tons of deuterium transferred from %s",
				amount / 1000000.0, Name(sdb[n].object))));
			return 1;
		}
	} else if (f == 3) {
		amount = tons * 3600;
		available = sdb[n].fuel.reserves;
		capacity = sdb2max_reserve(x) - sdb[x].fuel.reserves;
		if (amount > available) {
			notify(enactor, ansi_red(tprintf("%s has only %f GW hours of reserves available",
				Name(sdb[n].object), available / 3600.0)));
		} else if (capacity <= 0) {
			notify(enactor, ansi_red(tprintf("%s has no reserve capacity",
				Name(sdb[x].object))));
		} else if (amount > capacity) {
			notify(enactor, ansi_red(tprintf("%s has only %f GW hours of reserves capacity",
				Name(sdb[x].object), capacity / 3600.0)));
		} else {
			sdb[n].fuel.reserves -= amount;
			sdb[x].fuel.reserves += amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f GW hours of reserves transferred to %s",
				amount / 3600.0, Name(sdb[x].object))));
			console_message(x, "engineering operation",
				ansi_alert(tprintf("%f GW hours of reserves transferred from %s",
				amount / 3600.0, Name(sdb[n].object))));
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_defuel (char *fuel, double tons, dbref enactor)
{
	int x, f;
	double amount, capacity, available;

	if (error_on_console(enactor))
		return 0;

	switch (fuel[0]) {
		case 'A': case 'a': f = 1; break;
		case 'D': case 'd': f = 2; break;
		case 'R': case 'r': f = 3; break;
		default: f = 0; break;
	}
	if (!f) {
		notify(enactor, ansi_red("That is not a valid fuel type."));
		return 0;
	}
	if (tons <= 0.0) {
		notify(enactor, ansi_red("That is not a valid fuel amount."));
		return 0;
	}
	x = sdb[n].location;
	if (GoodSDB(x)) {
		if (!sdb[n].status.connected) {
			notify(enactor, ansi_red(tprintf("%s is not connected.", Name(sdb[n].object))));
		} else if (f == 1) {
			amount = tons * 1000000.0;
			available = sdb[n].fuel.antimatter;
			capacity = sdb2max_antimatter(x) - sdb[x].fuel.antimatter;
			if (amount > available)
				amount = available;
			if (capacity <= 0) {
				notify(enactor, ansi_red(tprintf("%s has no antimatter capacity",
					Name(sdb[x].object))));
			} else if (amount > capacity) {
				notify(enactor, ansi_red(tprintf("%s has only %f tons of antimatter capacity",
					Name(sdb[x].object), capacity / 1000000.0)));
			} else {
				sdb[n].fuel.antimatter -= amount;
				sdb[x].fuel.antimatter += amount;
				console_message(n, "engineering operation",
					ansi_cmd(enactor, tprintf("%f tons of antimatter transferred to %s",
					amount / 1000000.0, Name(sdb[x].object))));
				console_message(x, "engineering operation",
					ansi_alert(tprintf("%f tons of antimatter transferred from %s",
					amount / 1000000.0, Name(sdb[n].object))));
				return 1;
			}
		} else if (f == 2) {
			amount = tons * 1000000.0;
			available = sdb[n].fuel.deuterium;
			capacity = sdb2max_deuterium(x) - sdb[x].fuel.deuterium;
			if (amount > available)
				amount = available;
			if (capacity <= 0) {
				notify(enactor, ansi_red(tprintf("%s has no deuterium capacity",
					Name(sdb[x].object))));
			} else if (amount > capacity) {
				notify(enactor, ansi_red(tprintf("%s has only %f tons of deuterium capacity",
					Name(sdb[x].object), capacity / 1000000.0)));
			} else {
				sdb[n].fuel.deuterium -= amount;
				sdb[x].fuel.deuterium += amount;
				console_message(n, "engineering operation",
					ansi_cmd(enactor, tprintf("%f tons of deuterium transferred to %s",
					amount / 1000000.0, Name(sdb[x].object))));
				console_message(x, "engineering operation",
					ansi_alert(tprintf("%f tons of deuterium transferred from %s",
					amount / 1000000.0, Name(sdb[n].object))));
				return 1;
			}
		} else if (f == 3) {
			amount = tons * 3600;
			available = sdb[n].fuel.reserves;
			capacity = sdb2max_reserve(x) - sdb[x].fuel.reserves;
			if (amount > available)
				amount = available;
			if (capacity <= 0) {
				notify(enactor, ansi_red(tprintf("%s has no reserve capacity",
					Name(sdb[x].object))));
			} else if (amount > capacity) {
				notify(enactor, ansi_red(tprintf("%s has only %f GW hours of reserves capacity",
					Name(sdb[x].object), capacity / 3600.0)));
			} else {
				sdb[n].fuel.reserves -= amount;
				sdb[x].fuel.reserves += amount;
				console_message(n, "engineering operation",
					ansi_cmd(enactor, tprintf("%f GW hours of reserves transferred to %s",
					amount / 3600.0, Name(sdb[x].object))));
				console_message(x, "engineering operation",
					ansi_alert(tprintf("%f GW hours of reserves transferred from %s",
					amount / 3600.0, Name(sdb[n].object))));
				return 1;
			}
		}
	} else {
		if (f == 1) {
			amount = tons * 1000000.0;
			available = sdb[n].fuel.antimatter;
			if (amount > available)
				amount = available;
			sdb[n].fuel.antimatter -= amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f tons of antimatter dumped into space",
				amount / 1000000.0)));
			return 1;
		} else if (f == 2) {
			amount = tons * 1000000.0;
			available = sdb[n].fuel.deuterium;
			if (amount > available)
				amount = available;
			sdb[n].fuel.deuterium -= amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f tons of deuterium dumped into space",
				amount / 1000000.0)));
			return 1;
		} else if (f == 3) {
			amount = tons * 3600;
			available = sdb[n].fuel.reserves;
			if (amount > available)
				amount = available;
			sdb[n].fuel.reserves -= amount;
			console_message(n, "engineering operation",
				ansi_cmd(enactor, tprintf("%f GW hours of reserves dumped into space",
				amount / 3600.0)));
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_autopilot (int flag, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if (sdb[n].status.docked) {
		notify(enactor, ansi_red(tprintf("%s is in dock.", Name(sdb[n].object))));
	} else if (sdb[n].status.landed) {
		notify(enactor, ansi_red(tprintf("%s is on a landing pad.", Name(sdb[n].object))));
	} else if (sdb[n].status.connected) {
		notify(enactor, ansi_red(tprintf("%s is still connected.", Name(sdb[n].object))));
	} else if (!sdb[n].engine.warp_exist && !sdb[n].engine.impulse_exist) {
		notify(enactor, ansi_red(tprintf("%s cannot be maneuvered.", Name(sdb[n].object))));
	} else if (sdb[n].status.autopilot) {
		if (flag) {
			notify(enactor, ansi_red("Autopilot is already engaged."));
		} else {
			sdb[n].status.autopilot = 0;
			console_message(n, "helm",
			  ansi_cmd(enactor, "Autopilot disengaged"));
			return 1;
		}
	} else {
		if (!flag) {
			notify(enactor, ansi_red("Autopilot is already disengaged."));
		} else if (sdb[n].coords.xd == sdb[n].coords.x && sdb[n].coords.yd ==
		  sdb[n].coords.y && sdb[n].coords.zd == sdb[n].coords.z) {
			notify(enactor, ansi_red(tprintf("%s is already there.", Name(sdb[n].object))));
		} else {
			sdb[n].status.autopilot = 100;
			console_message(n, "helm",
		  		ansi_cmd(enactor, "Autopilot engaged"));
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

int do_set_iff_frequency (double frequency, dbref enactor)
{
	if (error_on_console(enactor)) {
		return 0;
	} else if ( frequency < 1.0 || frequency >= 1000.0 ) {
		notify(enactor, ansi_red("Valid IFF frequency is from 1.0 to 999.999 GHz."));
	} else {
		sdb[n].iff.frequency = frequency;
		console_message(n, "operation tactical",
			ansi_cmd(enactor, tprintf("IFF Frequency set to %.3f GHz", sdb[n].iff.frequency)));
		write_spacelog(enactor, sdb[n].object, tprintf("LOG: IFF Frequency set to %.6f GHz", sdb[n].iff.frequency));
		return 1;
	}
	return 0;
}


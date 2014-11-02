/* space_iterate.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

int    temp_sdb[MAX_SENSOR_CONTACTS];
int    temp_num[MAX_SENSOR_CONTACTS];
double temp_lev[MAX_SENSOR_CONTACTS];

extern time_t mudtime;

/* ------------------------------------------------------------------------ */

void up_alloc_balance (void)
{
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
	sdb[n].alloc.version = 0;
	sdb[n].engine.version = 1;
	sdb[n].sensor.version = 1;
	sdb[n].cloak.version = 1;
	return;
}

/* -------------------------------------------------------------------- */

void up_main_io (void)
{
	if (sdb[n].main.gw) {
		if (sdb[n].main.out > sdb[n].main.in) {
			sdb[n].main.out -= sdb[n].move.dt / 30.0;
			if (sdb[n].main.out <= sdb[n].main.in) {
				sdb[n].main.out = sdb[n].main.in;
				alert_main_balance(n);
			}
		} else if (sdb[n].main.out < sdb[n].main.in) {
			sdb[n].main.out += sdb[n].move.dt / 60.0;
			if (sdb[n].main.out >= sdb[n].main.in) {
				sdb[n].main.out = sdb[n].main.in;
				alert_main_balance(n);
			}
		}
	}
	sdb[n].power.main = sdb[n].main.gw * sdb[n].main.out;
	sdb[n].power.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_aux_io (void)
{
	if (sdb[n].aux.gw) {
		if (sdb[n].aux.out > sdb[n].aux.in) {
			sdb[n].aux.out -= sdb[n].move.dt / 5.0;
			if (sdb[n].aux.out <= sdb[n].aux.in) {
				sdb[n].aux.out = sdb[n].aux.in;
				alert_aux_balance(n);
			}
		} else if (sdb[n].aux.out < sdb[n].aux.in) {
			sdb[n].aux.out += sdb[n].move.dt / 10.0;
			if (sdb[n].aux.out >= sdb[n].aux.in) {
				sdb[n].aux.out = sdb[n].aux.in;
				alert_aux_balance(n);
			}
		}
	}
	sdb[n].power.aux = sdb[n].aux.gw * sdb[n].aux.out;
	sdb[n].power.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_batt_io (void)
{
	sdb[n].batt.out = sdb[n].batt.in;
	alert_batt_balance(n);
	sdb[n].power.batt = sdb[n].batt.gw * sdb[n].batt.out;
	sdb[n].power.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_main_damage (void)
{
	if (sdb[n].main.exist) {
		double dmg = (sdb[n].main.out - sdb[n].main.damage) *
		  sdb[n].move.dt / sdb[n].tech.main_max / 1000.0;

		if (sdb[n].main.damage > 0.0 && (sdb[n].main.damage - dmg) <= 0.0) {
			alert_main_overload(n);
		}
		sdb[n].main.damage -= dmg;
		if (sdb[n].main.damage <= -1.0) {
			sdb[n].main.damage = -1.0;
			do_all_console_notify(n, tprintf(ansi_warn("%s core breach."), system_name[8]));
			damage_structure(n, sdb[n].power.main * (get_random_long(0,100) + 1.0));
			sdb[n].main.in = 0.0;
			sdb[n].main.out = 0.0;
			sdb[n].power.main = 0.0;
			sdb[n].power.version = 1;
		}
	}
	return;
}

/* ------------------------------------------------------------------------ */

void up_aux_damage (void)
{
	if (sdb[n].aux.exist) {
		double dmg = (sdb[n].aux.out - sdb[n].aux.damage) *
		  sdb[n].move.dt / sdb[n].tech.aux_max / 1000.0;

		if (sdb[n].aux.damage > 0.0 && (sdb[n].aux.damage - dmg) <= 0.0) {
			alert_aux_overload(n);
		}
		sdb[n].aux.damage -= dmg;
		if (sdb[n].aux.damage <= -1.0) {
			sdb[n].aux.damage = -1.0;
			do_all_console_notify(n, tprintf(ansi_warn("%s core breach."), system_name[1]));
			damage_structure(n, sdb[n].power.aux * (get_random_long(0,10) + 1.0));
			sdb[n].aux.in = 0.0;
			sdb[n].aux.out = 0.0;
			sdb[n].power.aux = 0.0;
			sdb[n].power.version = 1;
		}
	}
	return;
}

/* -------------------------------------------------------------------- */

void up_fuel (void)
{
	double mloss = sdb[n].main.out * sdb[n].main.out * sdb[n].main.gw *
	  100.0 / sdb[n].tech.fuel * sdb[n].move.dt;
	double aloss = sdb[n].aux.out * sdb[n].aux.out * sdb[n].aux.gw *
	  100.0 / sdb[n].tech.fuel * sdb[n].move.dt;

	sdb[n].fuel.antimatter -= mloss;
	sdb[n].fuel.deuterium -= mloss + aloss;
	if (sdb[n].fuel.antimatter < 0.0) {
		if (sdb[n].main.out > 0.0)
			alert_anti_runout(n);
		sdb[n].fuel.antimatter = sdb[n].main.in = sdb[n].main.out = sdb[n].power.main = 0.0;
		sdb[n].power.version = 1;
	}
	if (sdb[n].fuel.deuterium < 0.0) {
		if (sdb[n].aux.out > 0.0 || sdb[n].main.out > 0.0)
			alert_deut_runout(n);
		sdb[n].fuel.deuterium = sdb[n].main.in = sdb[n].main.out = sdb[n].power.main = sdb[n].aux.in = sdb[n].aux.out = sdb[n].power.aux = 0.0;
		sdb[n].power.version = 1;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void up_reserve (void)
{
	sdb[n].fuel.reserves += (((sdb[n].power.main + sdb[n].power.aux + sdb[n].power.batt) * sdb[n].alloc.miscellaneous) - sdb[n].power.batt) * sdb[n].move.dt;
	if (sdb[n].fuel.reserves < 0.0) {
		sdb[n].fuel.reserves = sdb[n].batt.in = sdb[n].batt.out = sdb[n].power.batt = 0.0;
		alert_batt_runout(n);
		sdb[n].power.version = 1;
	} else if (sdb[n].fuel.reserves > sdb2max_reserve(n))
		sdb[n].fuel.reserves = sdb2max_reserve(n);
	return;
}

/* ------------------------------------------------------------------------ */

void up_total_power (void)
{
	sdb[n].power.total = sdb[n].power.main + sdb[n].power.aux + sdb[n].power.batt;
	sdb[n].power.version = 0;
	sdb[n].engine.version = 1;
	sdb[n].sensor.version = 1;
	sdb[n].cloak.version = 1;
	up_turn_rate();
	return;
}

/* ------------------------------------------------------------------------ */

void up_warp_damage (void)
{
	if (sdb[n].engine.warp_exist)
		if (fabs(sdb[n].move.out) >= 1.0)
			if (fabs(sdb[n].move.out) > sdb[n].engine.warp_cruise) {
				sdb[n].engine.warp_damage -= (fabs(sdb[n].move.out) - sdb[n].engine.warp_cruise) * sdb[n].move.dt / sdb[n].tech.main_max / 10000.0;
				if (sdb[n].engine.warp_damage < 0.0) {
					sdb[n].move.in = sdb[n].move.out = sdb[n].move.v = 0.0;
					alert_warp_overload(n);
					alert_speed_stop(n);
					alert_ship_exit_warp(n);
				}
				up_warp_max();
			}
	return;
}

/* ------------------------------------------------------------------------ */

void up_impulse_damage (void)
{
	if (sdb[n].engine.impulse_exist)
		if (fabs(sdb[n].move.out) < 1.0)
			if (fabs(sdb[n].move.out) > sdb[n].engine.impulse_cruise && fabs(sdb[n].move.in) < 1.0) {
				sdb[n].engine.impulse_damage -= (fabs(sdb[n].move.out) - sdb[n].engine.impulse_cruise) * sdb[n].move.dt / sdb[n].tech.aux_max / 10000.0;
				if (sdb[n].engine.impulse_damage < 0.0) {
					sdb[n].move.in = sdb[n].move.out = sdb[n].move.v = 0.0;
					alert_impulse_overload(n);
					alert_speed_stop(n);
				}
				up_impulse_max();
			}
	return;
}

/* ------------------------------------------------------------------------ */

void up_warp_max (void)
{
	sdb[n].engine.warp_max = sdb2max_warp(n);
	sdb[n].engine.warp_cruise = sdb2cruise_warp(n);

	if ((sdb[n].move.in >= 1.0) && (sdb[n].move.in > sdb[n].engine.warp_max)) {
		sdb[n].move.in = sdb[n].engine.warp_max;
	} else if ((sdb[n].move.in <= -1.0) && (sdb[n].move.in < (-sdb[n].engine.warp_max / 2.0)))
		sdb[n].move.in = -sdb[n].engine.warp_max / 2.0;

	return;
}

/* ------------------------------------------------------------------------ */

void up_impulse_max (void)
{
	sdb[n].engine.impulse_max = sdb2max_impulse(n);
	sdb[n].engine.impulse_cruise = sdb2cruise_impulse(n);

	if ((sdb[n].move.in > 0.0) && (sdb[n].move.in < 1.0) && (sdb[n].move.in > sdb[n].engine.impulse_max)) {
		sdb[n].move.in = sdb[n].engine.impulse_max;
	} else if ((sdb[n].move.in > -1.0) && (sdb[n].move.in < 0.0) && (sdb[n].move.in < (-sdb[n].engine.impulse_max / 2.0)))
		sdb[n].move.in = -sdb[n].engine.impulse_max / 2.0;

	return;
}

/* ------------------------------------------------------------------------ */

void up_tract_status (void)
{
	register int x;
	double p;

	if (sdb[n].status.tractoring) {
		x = sdb[n].status.tractoring;
		p = sdb[n].tract.damage * sdb[n].power.total * sdb[n].alloc.tractors / (sdb2range(n, x) + 1.0);
		if ((sdb[x].tract.active && p < sdb[x].tract.damage * sdb[x].power.total * sdb[x].alloc.tractors) || p < 1.0) {
			alert_tract_lost(n, x);
			sdb[n].tract.lock = 0;
			sdb[n].status.tractoring = 0;
			sdb[x].status.tractored = 0;
			sdb[n].power.version = 1;
			sdb[x].power.version = 1;
		}
	} else if (sdb[n].status.tractored && sdb[n].tract.active) {
		x = sdb[n].status.tractored;
		p = sdb[x].tract.damage * sdb[x].power.total * sdb[x].alloc.tractors / (sdb2range(x, n) + 1.0);
		if (p < (sdb[n].tract.damage * sdb[n].power.total * sdb[n].alloc.tractors)) {
			alert_tract_lost(x, n);
			sdb[x].tract.lock = 0;
			sdb[x].status.tractoring = 0;
			sdb[n].status.tractored = 0;
			sdb[n].power.version = 1;
			sdb[x].power.version = 1;
		}
	}
	return;
}

/* ------------------------------------------------------------------------ */

void up_cloak_status (void)
{
	if (sdb[n].cloak.active)
		if (sdb[n].alloc.cloak * sdb[n].power.total < sdb[n].cloak.cost) {
			alert_cloak_failure(n);
			alert_ship_cloak_offline(n);
			sdb[n].cloak.active = 0;
			sdb[n].sensor.version = 1;
			sdb[n].engine.version = 1;
		}
	sdb[n].cloak.version = 0;
	return;
}

/* ------------------------------------------------------------------------ */

void up_beam_io (void)
{
	if (sdb[n].beam.out > sdb[n].beam.in) {
		sdb[n].beam.out = sdb[n].beam.in;
		alert_beam_balance(n);
	} else if (sdb[n].alloc.beams * sdb[n].power.total > 0.0) {
		sdb[n].beam.out += sdb[n].alloc.beams * sdb[n].power.total * sdb[n].move.dt;
		if (sdb[n].beam.out >= sdb[n].beam.in) {
			sdb[n].beam.out = sdb[n].beam.in;
			alert_beam_charged(n);
		}
	}
	if (sdb[n].beam.out < 0.0)
		sdb[n].beam.out = 0.0;
	sdb[n].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_missile_io (void)
{
	if (sdb[n].missile.out > sdb[n].missile.in) {
		sdb[n].missile.out = sdb[n].missile.in;
		alert_missile_balance(n);
	} else if (sdb[n].alloc.missiles * sdb[n].power.total > 0.0) {
		sdb[n].missile.out += sdb[n].alloc.missiles * sdb[n].power.total * sdb[n].move.dt;
		if (sdb[n].missile.out >= sdb[n].missile.in) {
			sdb[n].missile.out = sdb[n].missile.in;
			alert_missile_charged(n);
		}
	}
	if (sdb[n].missile.out < 0.0)
		sdb[n].missile.out = 0.0;
	sdb[n].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_autopilot (void)
{
	double r = xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z,
	  sdb[n].coords.xd, sdb[n].coords.yd, sdb[n].coords.zd);
	double s = 99;
	int a = sdb[n].status.autopilot;

	if (r < 1.0) { s = 0; a = 0;
		console_message(n, "helm", ansi_cmd(sdb[n].object, "Autopilot destination reached"));
	} else if (r < 2) { s = 0.01; a = 1;
	} else if (r < 4) { s = 0.02; a = 2;
	} else if (r < 8) { s = 0.04; a = 3;
	} else if (r < 16) { s = 0.08; a = 4;
	} else if (r < 32) { s = 0.16; a = 5;
	} else if (r < 64) { s = 0.32; a = 6;
	} else if (r < 128) { s = 0.64; a = 7;
	} else {
		r /= sdb[n].move.cochranes * LIGHTSPEED;
		if (r < 1.0) { s = 0.999; a = 8;
		} else if (r < 10.0) { s = pow(r / (int) (r), 0.3); a = 9;
		} else if (r < 20.0) { s = 1.2; a = 10;
		} else if (r < 40.0) { s = 1.5; a = 11;
		} else if (r < 80.0) { s = 1.9; a = 12;
		} else if (r < 160.0) { s = 2.3; a = 13;
		} else if (r < 320.0) { s = 2.8; a = 14;
		} else if (r < 640.0) { s = 3.5; a = 15;
		} else if (r < 1280.0) { s = 4.3; a = 16;
		} else if (r < 2560.0) { s = 5.2; a = 17;
		} else if (r < 5120.0) { s = 6.5; a = 18;
		} else if (r < 10240.0) { s = 8.0; a = 19;
		} else if (r < 20480.0) { s = 9.8; a = 20;
		} else if (r < 40960.0) { s = 12.1; a = 21;
		} else if (r < 81920.0) { s = 14.9; a = 22;
		} else if (r < 163840.0) { s = 18.4; a = 23;
		} else if (r < 327680.0) { s = 22.6; a = 24;
		} else if (r < 655360.0) { s = 27.9; a = 25;
		} else if (r < 1310720.0) { s = 34.3; a = 26;
		} else if (r < 2621440.0) { s = 42.2; a = 27;
		} else if (r < 5242880.0) { s = 52.0; a = 28;
		} else if (r < 10485760.0) { s = 64.0; a = 29;
		}
	}

	if (sdb[n].status.autopilot != a) {
		sdb[n].status.autopilot = a;
		sdb[n].course.yaw_in = xy2bearing(sdb[n].coords.xd -
		  sdb[n].coords.x, sdb[n].coords.yd - sdb[n].coords.y);
		sdb[n].course.pitch_in = xyz2elevation(sdb[n].coords.xd -
		  sdb[n].coords.x, sdb[n].coords.yd - sdb[n].coords.y,
		  sdb[n].coords.zd -sdb[n].coords.z);
		if (sdb[n].move.in > s) {
			if (s >= 1.0 && s > sdb[n].engine.warp_cruise)
				s = sdb[n].engine.warp_cruise;
			if (s < 1.0 && s > sdb[n].engine.impulse_cruise)
				s = sdb[n].engine.impulse_cruise;
			sdb[n].move.in = s;
		}
	}

	return;
}

/* ------------------------------------------------------------------------ */

void up_speed_io (void)
{
	double a;

	if (sdb[n].move.ratio <= 0.0)
		return;

	if (fabs(sdb[n].move.out) < 1.0) {
		if (fabs(sdb[n].move.in) >= 1.0) {
			a = sdb[n].power.main * 0.99 + sdb[n].power.total *
			  sdb[n].alloc.movement * 0.01;
		} else
			a = sdb[n].power.aux * 0.9 + sdb[n].power.total *
			  sdb[n].alloc.movement * 0.1;
		a *= (1.0 - fabs(sdb[n].move.out)) / sdb[n].move.ratio / 50.0;
	} else
		a = (sdb[n].power.main * 0.99 + sdb[n].power.total *
		  sdb[n].alloc.movement * 0.0) / sdb[n].move.ratio /
		  fabs(sdb[n].move.out) / 5.0;

	a *= (sdb[n].move.ratio + 1.0) / sdb[n].move.ratio * sdb[n].move.dt;

	if (sdb[n].move.out < 0.0)
		a /= 2.0;

	if (sdb[n].status.tractoring) {
		a *= (sdb[n].structure.displacement + 0.1) /
		  (sdb[sdb[n].status.tractoring].structure.displacement +
		  sdb[n].structure.displacement + 0.1);
	} else if (sdb[n].status.tractored)
		a *= (sdb[n].structure.displacement + 0.1) /
		  (sdb[sdb[n].status.tractored].structure.displacement +
		  sdb[n].structure.displacement + 0.1);

	if (a < 0.01)
		a = 0.01;

	if ((sdb[n].move.in >= 1.0) && (sdb[n].move.in > sdb[n].engine.warp_max)) {
		sdb[n].move.in = sdb[n].engine.warp_max;
	} else if ((sdb[n].move.in <= -1.0) && (sdb[n].move.in < (-sdb[n].engine.warp_max / 2.0))) {
		sdb[n].move.in = -sdb[n].engine.warp_max / 2.0;
	} else if ((sdb[n].move.in >= 0.0) && (sdb[n].move.in < 1.0) && (sdb[n].move.in > sdb[n].engine.impulse_max)) {
		sdb[n].move.in = sdb[n].engine.impulse_max;
	} else if ((sdb[n].move.in <= 0.0) && (sdb[n].move.in > -1.0) && (sdb[n].move.in < (-sdb[n].engine.impulse_max / 2.0))) {
		sdb[n].move.in = -sdb[n].engine.impulse_max / 2.0;
	}

	if (sdb[n].move.out > sdb[n].move.in) {
		if (sdb[n].move.out >= 1.0) {
			if (sdb[n].move.in >= 1.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_warp(n);
			} else if (sdb[n].move.in > 0.0 && sdb[n].move.in < 1.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_impulse(n);
				alert_ship_exit_warp(n);
			} else if (sdb[n].move.in <= 0.0) {
				sdb[n].move.out = 0.0;
				alert_speed_stop(n);
				alert_ship_exit_warp(n);
			}
		} else if (sdb[n].move.out > 0.0 && sdb[n].move.out < 1.0) {
			if (sdb[n].move.in > 0.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_impulse(n);
			} else if (sdb[n].move.in <= 0.0) {
				sdb[n].move.out = 0.0;
				alert_speed_stop(n);
			}
		} else if (sdb[n].move.out <= 0.0) {
			if (sdb[n].move.out > -1.0) {
				sdb[n].move.out -= a;
				if (sdb[n].move.out <= sdb[n].move.in) {
					sdb[n].move.out = sdb[n].move.in;
					if (sdb[n].move.out > -1.0) {
						alert_speed_impulse(n);
					} else {
						alert_speed_warp(n);
						alert_ship_enter_warp(n);
					}
				} else if (sdb[n].move.out <= -1.0) {
					alert_ship_enter_warp(n);
				}
			} else {
				sdb[n].move.out -= a;
				if (sdb[n].move.out <= sdb[n].move.in) {
					sdb[n].move.out = sdb[n].move.in;
					alert_speed_warp(n);
				}
			}
		}
	} else if (sdb[n].move.out < sdb[n].move.in) {
		if (sdb[n].move.out <= -1.0) {
			if (sdb[n].move.in <= -1.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_warp(n);
			} else if (sdb[n].move.in < 0.0 && sdb[n].move.in > -1.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_impulse(n);
				alert_ship_exit_warp(n);
			} else if (sdb[n].move.in >= 0.0) {
				sdb[n].move.out = 0.0;
				alert_speed_stop(n);
				alert_ship_exit_warp(n);
			}
		} else if (sdb[n].move.out < 0.0 && sdb[n].move.out > -1.0) {
			if (sdb[n].move.in < 0.0) {
				sdb[n].move.out = sdb[n].move.in;
				alert_speed_impulse(n);
			} else if (sdb[n].move.in >= 0.0) {
				sdb[n].move.out = 0.0;
				alert_speed_stop(n);
			}
		} else if (sdb[n].move.out >= 0.0) {
			if (sdb[n].move.out < 1.0) {
				sdb[n].move.out += a;
				if (sdb[n].move.out >= sdb[n].move.in) {
					sdb[n].move.out = sdb[n].move.in;
					if (sdb[n].move.out < 1.0) {
						alert_speed_impulse(n);
					} else {
						alert_speed_warp(n);
						alert_ship_enter_warp(n);
					}
				} else if (sdb[n].move.out >= 1.0) {
					alert_ship_enter_warp(n);
				}
			} else {
				sdb[n].move.out += a;
				if (sdb[n].move.out >= sdb[n].move.in) {
					sdb[n].move.out = sdb[n].move.in;
					alert_speed_warp(n);
				}
			}
		}
	}
	sdb[n].sensor.version = 1;
	return;
}

/* ------------------------------------------------------------------------ */

void up_turn_rate (void)
{
	double a;

	if (sdb[n].move.ratio <= 0.0)
		return;

	if (fabs(sdb[n].move.out) < 1.0) {
		if (fabs(sdb[n].move.in) >= 1.0) {
			a = sdb[n].power.main * 0.99 + sdb[n].power.total *
			  sdb[n].alloc.movement * 0.01;
		} else
			a = sdb[n].power.aux * 0.9 + sdb[n].power.total *
			  sdb[n].alloc.movement * 0.1;
		a *= 3.6 * (1.0 - fabs(sdb[n].move.out)) / sdb[n].move.ratio;
	} else
		a = 3.6 * (sdb[n].power.main * 0.99 + sdb[n].power.total *
		  sdb[n].alloc.movement * 0.0) / sdb[n].move.ratio /
		  fabs(sdb[n].move.out);

	a *= (sdb[n].move.ratio + 1.0) / sdb[n].move.ratio * sdb[n].move.dt;

	if (sdb[n].move.out < 0.0)
		a /= 2.0;

	if (sdb[n].status.tractoring) {
		a *= (sdb[n].structure.displacement + 0.1) /
		  (sdb[sdb[n].status.tractoring].structure.displacement +
		  sdb[n].structure.displacement + 0.1);
	} else if (sdb[n].status.tractored)
		a *= (sdb[n].structure.displacement + 0.1) /
		  (sdb[sdb[n].status.tractored].structure.displacement +
		  sdb[n].structure.displacement + 0.1);

	if (a < 1.0)
		a = 1.0;

	sdb[n].course.rate = a;

	return;
}

/* ------------------------------------------------------------------------ */

void up_cochranes (void)
{
	sdb[n].move.cochranes = xyz2cochranes(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z);

	return;
}

/* ------------------------------------------------------------------------ */

void up_velocity (void)
{
	if (sdb[n].engine.warp_exist || sdb[n].engine.impulse_exist) {
		if (sdb[n].move.out >= 1.0) {
			sdb[n].move.v = LIGHTSPEED * pow(sdb[n].move.out, 3.333333);
		} else if (sdb[n].move.out <= -1.0) {
			sdb[n].move.v = LIGHTSPEED * -pow(fabs(sdb[n].move.out), 3.333333);
		} else
			sdb[n].move.v = LIGHTSPEED * sdb[n].move.out;
	}

	return;
}

/* ------------------------------------------------------------------------ */

void up_quadrant (void)
{
	register int q;

	if (sdb[n].coords.x > 0.0) {
		if (sdb[n].coords.y > 0.0) {
			q = 3;
		} else
			q = 2;
	} else
		if (sdb[n].coords.y > 0.0) {
			q = 1;
		} else
			q = 0;
	if (sdb[n].move.quadrant != q) {
		sdb[n].move.quadrant = q;
		alert_enter_quadrant(n);
	}

	return;
}

/* ------------------------------------------------------------------------ */

void up_visibility (void)
{
	double vis = xyz2vis(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z);
	
	if (sdb[n].status.docked || sdb[n].status.landed) {
		sdb[n].sensor.visibility = 1.0;
	} else {
		if (sdb[n].sensor.visibility == 1.0 && vis < 1.0)
			alert_enter_nebula(n);
		else if (sdb[n].sensor.visibility < 1.0 && vis == 1.0) 
			alert_exit_nebula(n);
		sdb[n].sensor.visibility = vis;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void up_yaw_io (void)
{
	if (sdb[n].course.yaw_out < sdb[n].course.yaw_in) {
		if ((sdb[n].course.yaw_in - sdb[n].course.yaw_out) <= 180.0) {
			sdb[n].course.yaw_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.yaw_out >= sdb[n].course.yaw_in) {
				sdb[n].course.yaw_out = sdb[n].course.yaw_in;
				alert_yaw(n);
			}
		} else {
			sdb[n].course.yaw_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.yaw_out < 0.0) {
				sdb[n].course.yaw_out += 360.0;
				if (sdb[n].course.yaw_out <= sdb[n].course.yaw_in) {
					sdb[n].course.yaw_out = sdb[n].course.yaw_in;
					alert_yaw(n);
				}
			}
		}
	} else {
		if ((sdb[n].course.yaw_out - sdb[n].course.yaw_in) <= 180.0) {
			sdb[n].course.yaw_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.yaw_out <= sdb[n].course.yaw_in) {
				sdb[n].course.yaw_out = sdb[n].course.yaw_in;
				alert_yaw(n);
			}
		} else {
			sdb[n].course.yaw_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.yaw_out >= 360.0) {
				sdb[n].course.yaw_out -= 360.0;
				if (sdb[n].course.yaw_out >= sdb[n].course.yaw_in) {
					sdb[n].course.yaw_out = sdb[n].course.yaw_in;
					alert_yaw(n);
				}
			}
		}
	}
	sdb[n].course.version = 1;

	return;
}

/* ------------------------------------------------------------------------ */

void up_pitch_io (void)
{
	if (sdb[n].course.pitch_out < sdb[n].course.pitch_in) {
		if ((sdb[n].course.pitch_in - sdb[n].course.pitch_out) <= 180.0) {
			sdb[n].course.pitch_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.pitch_out >= sdb[n].course.pitch_in) {
				sdb[n].course.pitch_out = sdb[n].course.pitch_in;
				alert_pitch(n);
			}
		} else {
			sdb[n].course.pitch_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.pitch_out < 0.0) {
				sdb[n].course.pitch_out += 360.0;
				if (sdb[n].course.pitch_out <= sdb[n].course.pitch_in) {
					sdb[n].course.pitch_out = sdb[n].course.pitch_in;
					alert_pitch(n);
				}
			}
		}
	} else {
		if ((sdb[n].course.pitch_out - sdb[n].course.pitch_in) <= 180.0) {
			sdb[n].course.pitch_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.pitch_out <= sdb[n].course.pitch_in) {
				sdb[n].course.pitch_out = sdb[n].course.pitch_in;
				alert_pitch(n);
			}
		} else {
			sdb[n].course.pitch_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.pitch_out >= 360.0) {
				sdb[n].course.pitch_out -= 360.0;
				if (sdb[n].course.pitch_out >= sdb[n].course.pitch_in) {
					sdb[n].course.pitch_out = sdb[n].course.pitch_in;
					alert_pitch(n);
				}
			}
		}
	}
	sdb[n].course.version = 1;

	return;
}

/* ------------------------------------------------------------------------ */

void up_roll_io (void)
{
	if (sdb[n].course.roll_out < sdb[n].course.roll_in) {
		if ((sdb[n].course.roll_in - sdb[n].course.roll_out) <= 180.0) {
			sdb[n].course.roll_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.roll_out >= sdb[n].course.roll_in) {
				sdb[n].course.roll_out = sdb[n].course.roll_in;
				alert_roll(n);
			}
		} else {
			sdb[n].course.roll_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.roll_out < 0.0) {
				sdb[n].course.roll_out += 360.0;
				if (sdb[n].course.roll_out <= sdb[n].course.roll_in) {
					sdb[n].course.roll_out = sdb[n].course.roll_in;
					alert_roll(n);
				}
			}
		}
	} else {
		if ((sdb[n].course.roll_out - sdb[n].course.roll_in) <= 180.0) {
			sdb[n].course.roll_out -= sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.roll_out <= sdb[n].course.roll_in) {
				sdb[n].course.roll_out = sdb[n].course.roll_in;
				alert_roll(n);
			}
		} else {
			sdb[n].course.roll_out += sdb[n].course.rate * sdb[n].move.dt;
			if (sdb[n].course.roll_out >= 360.0) {
				sdb[n].course.roll_out -= 360.0;
				if (sdb[n].course.roll_out >= sdb[n].course.roll_in) {
					sdb[n].course.roll_out = sdb[n].course.roll_in;
					alert_roll(n);
				}
			}
		}
	}
	sdb[n].course.version = 1;

	return;
}

/* ------------------------------------------------------------------------ */

void up_vectors (void)
{
	double d2r = PI / 180.0;
	double sy = sin(-sdb[n].course.yaw_out * d2r); 
	double cy = cos(sdb[n].course.yaw_out * d2r);
	double sp = sin(sdb[n].course.pitch_out * d2r);
	double cp = cos(sdb[n].course.pitch_out * d2r);
	double sr = sin(sdb[n].course.roll_out * d2r);
	double cr = cos(sdb[n].course.roll_out * d2r);

/* Original rotation matrix. Has issues with 90 degree angles 
// Working on a solution still
*/

	sdb[n].course.d[0][0] = cy * cp;
	sdb[n].course.d[0][1] = sy * cp;
	sdb[n].course.d[0][2] = sp;
	sdb[n].course.d[1][0] = -(sy * cr) + (cy * sp * sr);
	sdb[n].course.d[1][1] = (cy * cr) + (sy * sp * sr);
	sdb[n].course.d[1][2] = -(cp * sr);
	sdb[n].course.d[2][0] = -(sy * sr) - (cy * sp * cr);
	sdb[n].course.d[2][1] = (cy * sr) - (sy * sp * cr);
	sdb[n].course.d[2][2] = (cp * cr);
	sdb[n].course.version = 0;

	return;
}

/* ------------------------------------------------------------------------ */

void up_position (void)
{
	double dv = sdb[n].move.v * sdb[n].move.dt;

	if (fabs(sdb[n].move.out) >= 1.0)
		dv *= sdb[n].move.cochranes;
	if (sdb[n].status.tractoring) {
		sdb[sdb[n].status.tractoring].coords.x += dv * sdb[n].course.d[0][0];
		sdb[sdb[n].status.tractoring].coords.y += dv * sdb[n].course.d[0][1];
		sdb[sdb[n].status.tractoring].coords.z += dv * sdb[n].course.d[0][2];
	} else if (sdb[n].status.tractored) {
		sdb[sdb[n].status.tractored].coords.x += dv * sdb[n].course.d[0][0];
		sdb[sdb[n].status.tractored].coords.y += dv * sdb[n].course.d[0][1];
		sdb[sdb[n].status.tractored].coords.z += dv * sdb[n].course.d[0][2];
	}
	sdb[n].coords.x += dv * sdb[n].course.d[0][0];
	sdb[n].coords.y += dv * sdb[n].course.d[0][1];
	sdb[n].coords.z += dv * sdb[n].course.d[0][2];

	return;
}

/* ------------------------------------------------------------------------ */

void up_resolution (void)
{
	//int i;

	if (sdb[n].sensor.lrs_active) {
		sdb[n].sensor.lrs_resolution = sdb[n].tech.sensors * sdb[n].sensor.lrs_damage;
		if (sdb[n].sensor.ew_active)
			 sdb[n].sensor.lrs_resolution *= sdb2eccm_lrs(n);
		if (sdb[n].cloak.active)
			sdb[n].sensor.lrs_resolution /= 10.0;
	} else
		sdb[n].sensor.lrs_resolution = 0.0;
	if (sdb[n].sensor.srs_active) {
		sdb[n].sensor.srs_resolution = sdb[n].tech.sensors * sdb[n].sensor.srs_damage;
		if (sdb[n].sensor.ew_active)
			sdb[n].sensor.srs_resolution *= sdb2eccm_srs(n);
		if (sdb[n].cloak.active)
			sdb[n].sensor.srs_resolution /= 10.0;
	} else
		sdb[n].sensor.srs_resolution = 0.0;
	return;
}

/* ------------------------------------------------------------------------ */

void up_signature (int x)
{
	double base = pow(sdb[x].structure.displacement, 0.333333) / sdb[x].tech.stealth / 100.0;
	double sig = base;
	//double cloak;

	if (sdb[x].cloak.active) {
		sdb[x].cloak.level = 0.001 / sdb[x].power.total / sdb[x].alloc.cloak
		  / sdb[x].tech.cloak * sdb[x].cloak.cost;
		if (sdb[x].status.tractored)
			sdb[x].cloak.level *= 100.0;
		if (sdb[x].status.tractoring)
			sdb[x].cloak.level *= 100.0;
		if (sdb[x].beam.out > 1.0)
			sdb[x].cloak.level *= sdb[x].beam.out;
		if (sdb[x].missile.out > 1.0)
			sdb[x].cloak.level *= sdb[x].missile.out;
		if (sdb[x].sensor.visibility < 1.0)
			sdb[x].cloak.level *= (1.0 - sdb[x].sensor.visibility) * 10000.0;
		if (sdb[x].cloak.level > 1.0)
			sdb[x].cloak.level = 1.0;
	} else
		sdb[x].cloak.level = 1.0;

	sdb[x].sensor.lrs_signature = sig;
	sdb[x].sensor.srs_signature = sig * 10.0;
	sdb[x].sensor.lrs_signature *= sdb[x].move.out * sdb[n].move.out + 1.0;
	sdb[x].sensor.srs_signature *= 1.0 + sdb[x].power.main +
	  (sdb[x].power.aux / 10.0) + (sdb[x].power.batt / 100.0);
	if (sdb[x].sensor.ew_active) {
		sdb[x].sensor.lrs_signature /= sdb2ecm_lrs(x);
		sdb[x].sensor.srs_signature /= sdb2ecm_srs(x);
	}
	sdb[x].sensor.version = 0;
	return;
}

/* ------------------------------------------------------------------------ */

void up_sensor_message (int contacts)
{
	register int i, j, lose, gain, flag;

	for (i = 0 ; i < contacts ; ++i) {
		gain = 0;
		for (j = 0 ; j < sdb[n].sensor.contacts ; ++j) {
			if (temp_sdb[i] == sdb[n].slist.sdb[j]) {
				gain = 1;
				temp_num[i] = sdb[n].slist.num[j];
				break;
			}
		}
		if (!gain) {
			++sdb[n].sensor.counter;
			if (sdb[n].sensor.counter > 999) {
				sdb[n].sensor.counter = 1;
			}
			temp_num[i] = sdb[n].sensor.counter;
			console_message(n, "helm science tactical", ansi_warn(tprintf("New sensor contact (%d): %s", temp_num[i], unparse_type(temp_sdb[i]))));
		}
	}
	for (i = 0 ; i < sdb[n].sensor.contacts ; ++i) {
		lose = 0;
		for (j = 0 ; j < contacts ; ++j) {
			if (temp_sdb[j] == sdb[n].slist.sdb[i]) {
				lose = 1;
				break;
			}
		}
		if (!lose) {
			console_message(n, "helm science tactical", ansi_warn(tprintf("%s contact lost: %s", unparse_type(sdb[n].slist.sdb[i]), unparse_identity(n, sdb[n].slist.sdb[i]))));
			if (sdb[n].trans.s_lock == sdb[n].slist.sdb[i]) {
				console_message(n, "operation transporter", ansi_warn(tprintf("%s source lock on %s lost", system_name[13], unparse_identity(n, sdb[n].slist.sdb[i]))));
				sdb[n].trans.s_lock = 0;
			}
			if (sdb[n].trans.d_lock == sdb[n].slist.sdb[i]) {
				console_message(n, "operation transporter", ansi_warn(tprintf("%s lock on %s lost", system_name[13], unparse_identity(n, sdb[n].slist.sdb[i]))));
				sdb[n].trans.d_lock = 0;
			}
			if (sdb[n].tract.lock == sdb[n].slist.sdb[i]) {
				console_message(n, "helm operation", ansi_warn(tprintf("%s lock on %s lost", system_name[12], unparse_identity(n, sdb[n].slist.sdb[i]))));
				sdb[n].tract.lock = 0;
				sdb[n].status.tractoring = 0;
				sdb[sdb[n].slist.sdb[i]].status.tractored = 0;
				sdb[n].engine.version = 1;
				sdb[sdb[n].slist.sdb[i]].engine.version = 1;
			}
			flag = 0;
			for ( j = 0 ; j < sdb[n].beam.banks ; ++j )
				if (sdb[n].blist.lock[j] == sdb[n].slist.sdb[i]) {
					flag = 1;
					sdb[n].blist.lock[j] = 0;
				}
			if (flag)
				console_message(n, "tactical", ansi_warn(tprintf("%s lock on %s lost", system_name[3], unparse_identity(n, sdb[n].slist.sdb[i]))));
			flag = 0;
			for ( j = 0 ; j < sdb[n].missile.tubes ; ++j )
				if (sdb[n].mlist.lock[j] == sdb[n].slist.sdb[i]) {
					flag = 1;
					sdb[n].mlist.lock[j] = 0;
				}
			if (flag)
				console_message(n, "tactical", ansi_warn(tprintf("%s lock on %s lost", system_name[9], unparse_identity(n, sdb[n].slist.sdb[i]))));
		}
	}
	sdb[n].sensor.contacts = contacts;
	if (contacts == 0) {
		sdb[n].sensor.counter = 0;
	} else {
		for (i = 0 ; i < contacts ; ++i) {
			sdb[n].slist.sdb[i] = temp_sdb[i];
			sdb[n].slist.num[i] = temp_num[i];
			sdb[n].slist.lev[i] = temp_lev[i];
		}
	}
	return;
}

/* ------------------------------------------------------------------------ */

void up_sensor_list (void)
{
	register int object, i;
	register int contacts = 0;
	double x, y, z, level, limit = PARSEC * 100.0;

	for (object = MIN_SPACE_OBJECTS ; object <= max_space_objects ; ++object)
		if (sdb[n].location == sdb[object].location &&
		  sdb[n].space == sdb[object].space &&
		  sdb[object].structure.type &&
		  n != object) {
			x = fabs(sdb[n].coords.x - sdb[object].coords.x);
			if (x > limit)
				continue;
			y = fabs(sdb[n].coords.y - sdb[object].coords.y);
			if (y > limit)
				continue;
			z = fabs(sdb[n].coords.z - sdb[object].coords.z);
			if (z > limit)
				continue;
			level = (sdb[n].sensor.srs_resolution + 0.01) * sdb[object].sensor.srs_signature / (0.1 + (x * x + y * y + z * z) / 10101.010101);
			x /= PARSEC;
			y /= PARSEC;
			z /= PARSEC;
			level += sdb[n].sensor.lrs_resolution * sdb[object].sensor.lrs_signature / (1.0 + (x * x + y * y + z * z) * 99.0);
			level *= sdb[n].sensor.visibility * sdb[object].sensor.visibility;
			if (level < 0.01)
				continue;
			if (sdb[object].cloak.active)
				if (sdb[n].tech.sensors < 2.0)
					level *= sdb[object].cloak.level;
			if (level < 0.01)
				continue;
			temp_sdb[contacts] = object;
			temp_lev[contacts] = level;
			++contacts;
			if (contacts == MAX_SENSOR_CONTACTS) {
				break;
			}
		}

	if (contacts != sdb[n].sensor.contacts) {
		up_sensor_message(contacts);
	} else
		for (i = 0 ; i < sdb[n].sensor.contacts ; ++i) {
			sdb[n].slist.sdb[i] = temp_sdb[i];
			sdb[n].slist.lev[i] = temp_lev[i];
		}

	return;
}

/* ------------------------------------------------------------------------ */

void up_repair (void)
{
	sdb[n].structure.repair += sdb[n].move.dt * sdb[n].structure.max_repair / 1000.0 *
	   (1.0 + sqrt(sdb[n].alloc.miscellaneous * sdb[n].power.total));
	if (sdb[n].structure.repair >= sdb[n].structure.max_repair) {
		sdb[n].structure.repair = sdb[n].structure.max_repair;
		alert_max_repair(n);
	}
	return;
}

/* ------------------------------------------------------------------------ */

int do_space_db_iterate (void)
{
	register int count = 0;
	
	for (n = MIN_SPACE_OBJECTS; n <= max_space_objects; ++n)
		if (sdb[n].status.active && sdb[n].structure.type) {
				++count;
				sdb[n].move.dt = mudtime - sdb[n].move.time;
				sdb[n].move.time = mudtime;
				if (sdb[n].move.dt > 0.0) {
					if (sdb[n].structure.type == 1)
						if (sdb[n].move.time - sdb[n].status.time > 3600) {
							if (sdb[n].main.in > 0.0) {
								do_set_main_reactor(0.0, sdb[n].object);
								sdb[n].main.in = 0.0;
							}
							if (sdb[n].aux.in > 0.0) {
								do_set_aux_reactor(0.0, sdb[n].object);
								sdb[n].aux.in = 0.0;
							}
							if (sdb[n].batt.in > 0.0) {
								do_set_battery(0.0, sdb[n].object);
								sdb[n].batt.in = 0.0;
							}
							if (sdb[n].power.total == 0.0) {
								do_set_inactive(sdb[n].object);
							}
						}
					if (sdb[n].move.dt > 60.0)
						sdb[n].move.dt = 60;
					if(sdb[n].alloc.version)
						up_alloc_balance();
					if (sdb[n].main.out != sdb[n].main.in)
						up_main_io();
					if (sdb[n].aux.out != sdb[n].aux.in)
						up_aux_io();
					if (sdb[n].batt.out != sdb[n].batt.in)
						up_batt_io();
					if (sdb[n].main.out > 0.0)
						if (sdb[n].main.out > sdb[n].main.damage)
							up_main_damage();
					if (sdb[n].aux.out > 0.0)
						if (sdb[n].aux.out > sdb[n].aux.damage)
							up_aux_damage();
					if (sdb[n].power.main > 0.0 || sdb[n].power.aux > 0.0)
						up_fuel();
					if (sdb[n].power.batt > 0.0 || sdb[n].alloc.miscellaneous > 0.0)
						up_reserve();
					if (sdb[n].power.version)
						up_total_power();
						up_tract_status();
					if (sdb[n].beam.in != sdb[n].beam.out)
						up_beam_io();
					if (sdb[n].missile.in != sdb[n].missile.out)
						up_missile_io();
					if (sdb[n].engine.version) {
						up_warp_max();
						up_impulse_max();
						up_turn_rate();
						sdb[n].engine.version = 0;
					}
					if (sdb[n].status.autopilot)
						up_autopilot();
					if (sdb[n].move.in != sdb[n].move.out) {
						up_speed_io();
						up_velocity();
						up_turn_rate();
					}
					if (sdb[n].move.out != 0.0) {
						up_warp_damage();
						up_impulse_damage();
					}
					if (sdb[n].course.yaw_in != sdb[n].course.yaw_out)
						up_yaw_io();
					if (sdb[n].course.pitch_in != sdb[n].course.pitch_out)
						up_pitch_io();
					if (sdb[n].course.roll_in != sdb[n].course.roll_out)
						up_roll_io();
					if (sdb[n].course.version)
						up_vectors();
					if (sdb[n].move.v != 0.0) {
						up_position();
						up_cochranes();
						up_empire();
						up_quadrant();
						up_visibility();
					}
					if (sdb[n].cloak.version)
						up_cloak_status();
					if (sdb[n].sensor.version) {
						up_resolution();
						up_signature(n);
					}
					up_sensor_list();
					if (sdb[n].structure.repair != sdb[n].structure.max_repair)
						up_repair();
				}
		}

	return count;
}

/* ------------------------------------------------------------------------ */						


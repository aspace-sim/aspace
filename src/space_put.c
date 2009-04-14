/* space_put.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void do_space_db_put (int x, char *value, char *f1, char *f2, char *f3, char *f4, char *buff, char **bp)
{
	switch (f1[0]) {
		case 'a': switch (f1[1]) {
/* alloc ------------------------------------------------------------------ */
			case 'l': switch (f2[0]) {
				case 'b': sdb[x].alloc.beams = parse_number(value); break;
				case 'c': sdb[x].alloc.cloak = parse_number(value); break;
				case 'e': switch (f2[2]) {
					case 'm': sdb[x].alloc.ecm = parse_number(value); break;
					case 'c': sdb[x].alloc.eccm = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'h': sdb[x].alloc.helm = parse_number(value); break;
				case 'm': switch (f2[3]) {
					case 'c': sdb[x].alloc.miscellaneous = parse_number(value); break;
					case 'e': sdb[x].alloc.movement = parse_number(value); break;
					case 's': sdb[x].alloc.missiles = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'o': sdb[x].alloc.operations = parse_number(value); break;
				case 's': switch (f2[1]) {
					case 'e': sdb[x].alloc.sensors = parse_number(value); break;
					case 'h': switch (f3[0]) {
						case 'f': sdb[x].alloc.shield[0] = parse_number(value); break;
						case 's': sdb[x].alloc.shield[1] = parse_number(value); break;
						case 'a': sdb[x].alloc.shield[2] = parse_number(value); break;
						case 'p': sdb[x].alloc.shield[3] = parse_number(value); break;
						case 'd': sdb[x].alloc.shield[4] = parse_number(value); break; 
						case 'v': sdb[x].alloc.shield[5] = parse_number(value); break;
						default: sdb[x].alloc.shields = parse_number(value); break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 't': switch (f2[3]) {
					case 't': sdb[x].alloc.tactical = parse_number(value); break;
					case 'c': sdb[x].alloc.tractors = parse_number(value); break;
					case 'n': sdb[x].alloc.transporters = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'v': sdb[x].alloc.version = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* aux -------------------------------------------------------------------- */
			case 'u': switch (f2[0]) {
				case 'i': sdb[x].aux.in = parse_number(value); break;
				case 'o': sdb[x].aux.out = parse_number(value); break;
				case 'd': sdb[x].aux.damage = parse_number(value); break;
				case 'g': sdb[x].aux.gw = parse_number(value); break;
				case 'e': sdb[x].aux.exist = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		case 'b': switch (f1[1]) {
/* batt ------------------------------------------------------------------- */
			case 'a': switch (f2[0]) {
				case 'i': sdb[x].batt.in = parse_number(value); break;
				case 'o': sdb[x].batt.out = parse_number(value); break;
				case 'd': sdb[x].batt.damage = parse_number(value); break;
				case 'g': sdb[x].batt.gw = parse_number(value); break;
				case 'e': sdb[x].batt.exist = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* beam ------------------------------------------------------------------- */
			case 'e': switch (f2[0]) {
				case 'b': sdb[x].beam.banks = parse_number(value); break;
				case 'e': sdb[x].beam.exist = parse_number(value); break;
				case 'f': sdb[x].beam.freq = parse_number(value); break;
				case 'i': sdb[x].beam.in = parse_number(value); break;
				case 'o': sdb[x].beam.out = parse_number(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		case 'c': switch (f1[1]) {
/* cloak ------------------------------------------------------------------ */
			case 'l': switch (f2[0]) {
				case 'a': sdb[x].cloak.active = parse_integer(value); break;
				case 'c': sdb[x].cloak.cost = parse_integer(value); break;
				case 'd': sdb[x].cloak.damage = parse_number(value); break;
				case 'e': sdb[x].cloak.exist = parse_integer(value); break;
				case 'f': sdb[x].cloak.freq = parse_number(value); break;
				case 'v': sdb[x].cloak.version = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			case 'o': switch (f1[2]) {
/* coords ----------------------------------------------------------------- */
				case 'o': switch (f3[0]) {
					case 'd': switch (f2[0]) {
						case 'x': sdb[x].coords.xd = parse_number(value); break;
						case 'y': sdb[x].coords.yd = parse_number(value); break;
						case 'z': sdb[x].coords.zd = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'o': switch (f2[0]) {
						case 'x': sdb[x].coords.xo = parse_number(value); break;
						case 'y': sdb[x].coords.yo = parse_number(value); break;
						case 'z': sdb[x].coords.zo = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					default: switch (f2[0]) {
						case 'x': sdb[x].coords.x = parse_number(value); break;
						case 'y': sdb[x].coords.y = parse_number(value); break;
						case 'z': sdb[x].coords.z = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					} break;
/* course ----------------------------------------------------------------- */
				case 'u': switch (f2[0]) {
					case 'd': switch (f3[0]) {
						case 'f': switch (f4[0]) {
							case 'x': sdb[x].course.d[0][0] = parse_number(value); break;
							case 'y': sdb[x].course.d[0][1] = parse_number(value); break;
							case 'z': sdb[x].course.d[0][2] = parse_number(value); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
							} break;
						case 'r': sdb[x].course.rate = parse_number(value); break;
						case 's': switch (f4[0]) {
							case 'x': sdb[x].course.d[1][0] = parse_number(value); break;
							case 'y': sdb[x].course.d[1][1] = parse_number(value); break;
							case 'z': sdb[x].course.d[1][2] = parse_number(value); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
							} break;
						case 'u': switch (f4[0]) {
							case 'x': sdb[x].course.d[2][0] = parse_number(value); break;
							case 'y': sdb[x].course.d[2][1] = parse_number(value); break;
							case 'z': sdb[x].course.d[2][2] = parse_number(value); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
							} break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'p': switch (f3[0]) {
						case 'i': sdb[x].course.pitch_in = parse_number(value); break;
						case 'o': sdb[x].course.pitch_out = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'r': switch (f3[0]) {
						case 'i': sdb[x].course.roll_in = parse_number(value); break;
						case 'o': sdb[x].course.roll_out = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'v': sdb[x].course.version = parse_integer(value); break;
					case 'y': switch (f3[0]) {
						case 'i': sdb[x].course.yaw_in = parse_number(value); break;
						case 'o': sdb[x].course.yaw_out = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		case 'e': switch (f2[0]) {
/* engine ----------------------------------------------------------------- */
			case 'w': switch (f3[0]) {
				case 'c': sdb[x].engine.warp_cruise = parse_number(value); break;
				case 'd': sdb[x].engine.warp_damage = parse_number(value); break;
				case 'm': sdb[x].engine.warp_max = parse_number(value); break;
				case 'e': sdb[x].engine.warp_exist = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			case 'i': switch (f3[0]) {
				case 'c': sdb[x].engine.impulse_cruise = parse_number(value); break;
				case 'd': sdb[x].engine.impulse_damage = parse_number(value); break;
				case 'm': sdb[x].engine.impulse_max = parse_number(value); break;
				case 'e': sdb[x].engine.impulse_exist = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			case 'v': sdb[x].engine.version = parse_integer(value); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
/* fuel ------------------------------------------------------------------- */
		case 'f': switch (f2[0]) {
			case 'a': sdb[x].fuel.antimatter = parse_number(value); break;
			case 'd': sdb[x].fuel.deuterium = parse_number(value); break;
			case 'r': sdb[x].fuel.reserves = parse_number(value); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
/* location --------------------------------------------------------------- */
		case 'l': sdb[x].location = parse_integer(value); break;
		case 'm': switch (f1[1]) {
/* main ------------------------------------------------------------------- */
			case 'a': switch (f2[0]) {
				case 'i': sdb[x].main.in = parse_number(value); break;
				case 'o': sdb[x].main.out = parse_number(value); break;
				case 'd': sdb[x].main.damage = parse_number(value); break;
				case 'g': sdb[x].main.gw = parse_number(value); break;
				case 'e': sdb[x].main.exist = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* missile ---------------------------------------------------------------- */
			case 'i': switch (f2[0]) {
				case 'e': sdb[x].missile.exist = parse_number(value); break;
				case 'f': sdb[x].missile.freq = parse_number(value); break;
				case 'i': sdb[x].missile.in = parse_number(value); break;
				case 'o': sdb[x].missile.out = parse_number(value); break;
				case 't': sdb[x].missile.tubes = parse_number(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* move ------------------------------------------------------------------- */
			case 'o': switch (f2[0]) {
				case 't': sdb[x].move.time = parse_integer(value); break;
				case 'd': sdb[x].move.dt = parse_integer(value); break;
				case 'i': sdb[x].move.in = parse_number(value); break;
				case 'o': sdb[x].move.out = parse_number(value); break;
				case 'r': sdb[x].move.ratio = parse_number(value); break;
				case 'c': sdb[x].move.cochranes = parse_number(value); break;
				case 'v': sdb[x].move.v = parse_number(value); break;
				case 'e': sdb[x].move.empire = parse_integer(value); break;
				case 'q': sdb[x].move.quadrant = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
/* object ----------------------------------------------------------------- */
		case 'o': sdb[x].object = parse_integer(value); break;
/* power ------------------------------------------------------------------ */
		case 'p': switch (f2[0]) {
			case 'v': sdb[x].power.version = parse_integer(value); break;
			case 'm': sdb[x].power.main = parse_number(value); break;
			case 'a': sdb[x].power.aux = parse_number(value); break;
			case 'b': sdb[x].power.batt = parse_number(value); break;
			case 't': sdb[x].power.total = parse_number(value); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		case 's': switch (f1[1]) {
/* sensor ----------------------------------------------------------------- */
			case 'e': switch (f2[0]) {
				case 'c': switch (f2[2]) {
					case 'n': sdb[x].sensor.contacts = parse_integer(value); break;
					case 'u': sdb[x].sensor.counter = parse_integer(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'e': switch (f3[0]) {
					case 'a': sdb[x].sensor.ew_active = parse_integer(value); break;
					case 'd': sdb[x].sensor.ew_damage = parse_number(value); break;
					case 'e': sdb[x].sensor.ew_exist = parse_integer(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'l': switch (f3[0]) {
					case 'a': sdb[x].sensor.lrs_active = parse_integer(value); break;
					case 'd': sdb[x].sensor.lrs_damage = parse_number(value); break;
					case 'e': sdb[x].sensor.lrs_exist = parse_integer(value); break;
					case 'r': sdb[x].sensor.lrs_resolution = parse_number(value); break;
					case 's': sdb[x].sensor.lrs_signature = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 's': switch (f3[0]) {
					case 'a': sdb[x].sensor.srs_active = parse_integer(value); break;
					case 'd': sdb[x].sensor.srs_damage = parse_number(value); break;
					case 'e': sdb[x].sensor.srs_exist = parse_integer(value); break;
					case 'r': sdb[x].sensor.srs_resolution = parse_number(value); break;
					case 's': sdb[x].sensor.srs_signature = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'v': switch (f2[1]) {
					case 'e': sdb[x].sensor.version = parse_integer(value); break;
					case 'i': sdb[x].sensor.visibility = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* shield ----------------------------------------------------------------- */
			case 'h': switch (f2[0]) {
				case 'a': switch (f3[0]) {
					case 'f': sdb[x].shield.active[0] = parse_integer(value); break;
					case 's': sdb[x].shield.active[1] = parse_integer(value); break;
					case 'a': sdb[x].shield.active[2] = parse_integer(value); break;
					case 'p': sdb[x].shield.active[3] = parse_integer(value); break;
					case 'd': sdb[x].shield.active[4] = parse_integer(value); break;
					case 'v': sdb[x].shield.active[5] = parse_integer(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'd': switch (f3[0]) {
					case 'f': sdb[x].shield.damage[0] = parse_number(value); break;
					case 's': sdb[x].shield.damage[1] = parse_number(value); break;
					case 'a': sdb[x].shield.damage[2] = parse_number(value); break;
					case 'p': sdb[x].shield.damage[3] = parse_number(value); break;
					case 'd': sdb[x].shield.damage[4] = parse_number(value); break;
					case 'v': sdb[x].shield.damage[5] = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'e': sdb[x].shield.exist = parse_integer(value); break;
				case 'f': sdb[x].shield.freq = parse_number(value); break;
				case 'm': sdb[x].shield.maximum = parse_integer(value); break;
				case 'r': sdb[x].shield.ratio = parse_number(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
/* space ------------------------------------------------------------------ */
			case 'p': sdb[x].space = parse_integer(value); break;
			case 't': switch (f1[2]) {
/* status ----------------------------------------------------------------- */
				case 'a': switch (f2[0]) {
					case 'a': switch (f2[1]) {
						case 'c': sdb[x].status.active = parse_integer(value); break;
						case 'u': sdb[x].status.autopilot = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'c': switch (f2[1]) {
						case 'o': sdb[x].status.connected = parse_integer(value); break;
						case 'r': sdb[x].status.crippled = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'd': sdb[x].status.docked = parse_integer(value); break;
					case 'i': sdb[x].status.time = parse_integer(value); break;
					case 'l': switch (f2[1]) {
						case 'a': sdb[x].status.landed = parse_integer(value); break;
						case 'i': sdb[x].status.link = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'o': switch (f3[0]) {
						case 'l': sdb[x].status.open_landing = parse_integer(value); break;
						case 'd': sdb[x].status.open_docking = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 't': switch (f2[7]) {
						case 'e': sdb[x].status.tractored = parse_integer(value); break;
						case 'i': sdb[x].status.tractoring = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
/* structure -------------------------------------------------------------- */
				case 'r': switch (f2[0]) {
					case 'c': switch (f3[0]) {
						case 'd': sdb[x].structure.can_dock = parse_integer(value); break;
						case 'l': sdb[x].structure.can_land = parse_integer(value); break;
						case 'h': sdb[x].structure.cargo_hold = parse_number(value); break;
						case 'm': sdb[x].structure.cargo_mass = parse_number(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'd': sdb[x].structure.displacement = parse_number(value); break;
					case 'h': switch (f3[0]) {
						case 'd': sdb[x].structure.has_docking_bay = parse_integer(value); break;
						case 'l': sdb[x].structure.has_landing_pad = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'm': switch (f3[0]) {
						case 'r': sdb[x].structure.max_repair = parse_integer(value); break;
						case 's': sdb[x].structure.max_structure = parse_integer(value); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
						} break;
					case 'r': sdb[x].structure.repair = parse_number(value); break;
					case 's': sdb[x].structure.superstructure = parse_number(value); break;
					case 't': sdb[x].structure.type = parse_integer(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		case 't': switch (f1[3]) {
/* tract ------------------------------------------------------------------ */
			case 'c': switch (f2[0]) {
				case 'c': sdb[x].tract.cost = parse_integer(value); break;
				case 'f': sdb[x].tract.freq = parse_number(value); break;
				case 'e': sdb[x].tract.exist = parse_integer(value); break;
				case 'a': sdb[x].tract.active = parse_integer(value); break;
				case 'd': sdb[x].tract.damage = parse_number(value); break;
				case 'l': sdb[x].tract.lock = parse_integer(value); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* tech ------------------------------------------------------------------- */
			case 'h': switch (f2[0]) {
				case 'a': switch (f2[1]) {
					case 'r': sdb[x].tech.armor = parse_number(value); break;
					case 'u': sdb[x].tech.aux_max = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'c': sdb[x].tech.cloak = parse_number(value); break;
				case 'f': switch (f2[1]) {
					case 'i': sdb[x].tech.firing = parse_number(value); break;
					case 'u': sdb[x].tech.fuel = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				case 'l': sdb[x].tech.ly_range = parse_number(value); break;
				case 'm': sdb[x].tech.main_max = parse_number(value); break;
				case 's': switch (f2[1]) {
					case 't': sdb[x].tech.stealth = parse_number(value); break;
					case 'e': sdb[x].tech.sensors = parse_number(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
/* trans ------------------------------------------------------------------ */
			case 'n': switch (f2[0]) {
				case 'c': sdb[x].trans.cost = parse_integer(value); break;
				case 'f': sdb[x].trans.freq = parse_number(value); break;
				case 'e': sdb[x].trans.exist = parse_integer(value); break;
				case 'a': sdb[x].trans.active = parse_integer(value); break;
				case 'd': sdb[x].trans.damage = parse_number(value); break;
				case 'l': switch (f3[0]) {
					case 'd': sdb[x].trans.d_lock = parse_integer(value); break;
					case 's': sdb[x].trans.s_lock = parse_integer(value); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break;
			} break;
		default: safe_str("#-1 NO SUCH FIELD", buff, bp); return; break; }

	safe_chr('1', buff, bp);
	return;
}

/* ------------------------------------------------------------------------ */

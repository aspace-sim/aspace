/* space_get.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void do_space_db_get (int x, char *f1, char *f2, char *f3, char *f4, char *buff, char **bp)
{
	switch (f1[0]) {
		case 'a': switch (f1[1]) {
/* alloc ------------------------------------------------------------------ */
			case 'l': switch (f2[0]) {
				case 'b': safe_str(unparse_number(sdb[x].alloc.beams), buff, bp); break;
				case 'c': safe_str(unparse_number(sdb[x].alloc.cloak), buff, bp); break;
				case 'e': switch (f2[2]) {
					case 'm': safe_str(unparse_number(sdb[x].alloc.ecm), buff, bp); break;
					case 'c': safe_str(unparse_number(sdb[x].alloc.eccm), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'h': safe_str(unparse_number(sdb[x].alloc.helm), buff, bp); break;
				case 'm': switch (f2[3]) {
					case 'c': safe_str(unparse_number(sdb[x].alloc.miscellaneous), buff, bp); break;
					case 'e': safe_str(unparse_number(sdb[x].alloc.movement), buff, bp); break;
					case 's': safe_str(unparse_number(sdb[x].alloc.missiles), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'o':	safe_str(unparse_number(sdb[x].alloc.operations), buff, bp); break;
				case 's': switch (f2[1]) {
					case 'e':	safe_str(unparse_number(sdb[x].alloc.sensors), buff, bp); break;
					case 'h': switch (f3[0]) {
						case 'f': safe_str(unparse_number(sdb[x].alloc.shield[0]), buff, bp); break;
						case 's': safe_str(unparse_number(sdb[x].alloc.shield[1]), buff, bp); break;
						case 'a': safe_str(unparse_number(sdb[x].alloc.shield[2]), buff, bp); break;
						case 'p': safe_str(unparse_number(sdb[x].alloc.shield[3]), buff, bp); break;
						default: safe_str(unparse_number(sdb[x].alloc.shields), buff, bp); break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 't': switch (f2[3]) {
					case 't':	safe_str(unparse_number(sdb[x].alloc.tactical), buff, bp); break;
					case 'c':	safe_str(unparse_number(sdb[x].alloc.tractors), buff, bp); break;
					case 'n': safe_str(unparse_number(sdb[x].alloc.transporters), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'v': safe_str(unparse_integer(sdb[x].alloc.version), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* aux -------------------------------------------------------------------- */
			case 'u': switch (f2[0]) {
				case 'i': safe_str(unparse_number(sdb[x].aux.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].aux.out), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].aux.damage), buff, bp); break;
				case 'g': safe_str(unparse_number(sdb[x].aux.gw), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].aux.exist), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
		case 'b': switch (f1[1]) {
/* batt ------------------------------------------------------------------- */
			case 'a': switch (f2[0]) {
				case 'i': safe_str(unparse_number(sdb[x].batt.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].batt.out), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].batt.damage), buff, bp); break;
				case 'g': safe_str(unparse_number(sdb[x].batt.gw), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].batt.exist), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* beam ------------------------------------------------------------------- */
			case 'e': switch (f2[0]) {
				case 'b': safe_str(unparse_integer(sdb[x].beam.banks), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].beam.exist), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].beam.freq), buff, bp); break;
				case 'i': safe_str(unparse_number(sdb[x].beam.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].beam.out), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
		case 'c': switch (f1[1]) {
/* cloak ------------------------------------------------------------------ */
			case 'l': switch (f2[0]) {
				case 'a': safe_str(unparse_integer(sdb[x].cloak.active), buff, bp); break;
				case 'c': safe_str(unparse_integer(sdb[x].cloak.cost), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].cloak.damage), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].cloak.exist), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].cloak.freq), buff, bp); break;
				case 'v': safe_str(unparse_integer(sdb[x].cloak.version), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			case 'o': switch (f1[2]) {
/* coord ------------------------------------------------------------------ */
				case 'o': switch (f3[0]) {
					case 'o': switch (f2[0]) {
						case 'x': safe_str(unparse_number(sdb[x].coords.xo), buff, bp); break;
						case 'y': safe_str(unparse_number(sdb[x].coords.yo), buff, bp); break;
						case 'z': safe_str(unparse_number(sdb[x].coords.zo), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'd': switch (f2[0]) {
						case 'x': safe_str(unparse_number(sdb[x].coords.xd), buff, bp); break;
						case 'y': safe_str(unparse_number(sdb[x].coords.yd), buff, bp); break;
						case 'z': safe_str(unparse_number(sdb[x].coords.zd), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					default: switch (f2[0]) {
						case 'x': safe_str(unparse_number(sdb[x].coords.x), buff, bp); break;
						case 'y': safe_str(unparse_number(sdb[x].coords.y), buff, bp); break;
						case 'z': safe_str(unparse_number(sdb[x].coords.z), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					} break;
/* course ----------------------------------------------------------------- */
				case 'u': switch (f2[0]) {
					case 'd': switch (f3[0]) {
						case 'f': switch (f4[0]) {
							case 'x': safe_str(unparse_number(sdb[x].course.d[0][0]), buff, bp); break;
							case 'y': safe_str(unparse_number(sdb[x].course.d[0][1]), buff, bp); break;
							case 'z': safe_str(unparse_number(sdb[x].course.d[0][2]), buff, bp); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
							} break;
						case 'r': safe_str(unparse_number(sdb[x].course.rate), buff, bp); break;
						case 's': switch (f4[0]) {
							case 'x': safe_str(unparse_number(sdb[x].course.d[1][0]), buff, bp); break;
							case 'y': safe_str(unparse_number(sdb[x].course.d[1][1]), buff, bp); break;
							case 'z': safe_str(unparse_number(sdb[x].course.d[1][2]), buff, bp); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
							} break;
						case 'u': switch (f4[0]) {
							case 'x': safe_str(unparse_number(sdb[x].course.d[2][0]), buff, bp); break;
							case 'y': safe_str(unparse_number(sdb[x].course.d[2][1]), buff, bp); break;
							case 'z': safe_str(unparse_number(sdb[x].course.d[2][2]), buff, bp); break;
							default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
							} break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'p': switch (f3[0]) {
						case 'i': safe_str(unparse_number(sdb[x].course.pitch_in), buff, bp); break;
						case 'o': safe_str(unparse_number(sdb[x].course.pitch_out), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'r': switch (f3[0]) {
						case 'i': safe_str(unparse_number(sdb[x].course.roll_in), buff, bp); break;
						case 'o': safe_str(unparse_number(sdb[x].course.roll_out), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'v': safe_str(unparse_integer(sdb[x].course.version), buff, bp); break;
					case 'y': switch (f3[0]) {
						case 'i': safe_str(unparse_number(sdb[x].course.yaw_in), buff, bp); break;
						case 'o': safe_str(unparse_number(sdb[x].course.yaw_out), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
/* engine ----------------------------------------------------------------- */
		case 'e': switch (f2[0]) {
			case 'w': switch (f3[0]) {
				case 'c': safe_str(unparse_number(sdb[x].engine.warp_cruise), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].engine.warp_damage), buff, bp); break;
				case 'm': safe_str(unparse_number(sdb[x].engine.warp_max), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].engine.warp_exist), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			case 'i': switch (f3[0]) {
				case 'c': safe_str(unparse_number(sdb[x].engine.impulse_cruise), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].engine.impulse_damage), buff, bp); break;
				case 'm': safe_str(unparse_number(sdb[x].engine.impulse_max), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].engine.impulse_exist), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			case 'v': safe_str(unparse_integer(sdb[x].engine.version), buff, bp); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
/* fuel ------------------------------------------------------------------- */
		case 'f': switch (f2[0]) {
			case 'a': safe_str(unparse_number(sdb[x].fuel.antimatter), buff, bp); break;
			case 'd': safe_str(unparse_number(sdb[x].fuel.deuterium), buff, bp); break;
			case 'r': safe_str(unparse_number(sdb[x].fuel.reserves), buff, bp); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
/* location --------------------------------------------------------------- */
		case 'l': safe_str(unparse_integer(sdb[x].location), buff, bp); break;
		case 'm': switch (f1[1]) {
/* main ------------------------------------------------------------------- */
			case 'a': switch (f2[0]) {
				case 'i': safe_str(unparse_number(sdb[x].main.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].main.out), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].main.damage), buff, bp); break;
				case 'g': safe_str(unparse_number(sdb[x].main.gw), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].main.exist), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* missile ---------------------------------------------------------------- */
			case 'i': switch (f2[0]) {
				case 'e': safe_str(unparse_integer(sdb[x].missile.exist), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].missile.freq), buff, bp); break;
				case 'i': safe_str(unparse_number(sdb[x].missile.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].missile.out), buff, bp); break;
				case 't': safe_str(unparse_integer(sdb[x].missile.tubes), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* move ------------------------------------------------------------------- */
			case 'o': switch (f2[0]) {
				case 't': safe_str(unparse_integer(sdb[x].move.time), buff, bp); break;
				case 'd': safe_str(unparse_integer(sdb[x].move.dt), buff, bp); break;
				case 'i': safe_str(unparse_number(sdb[x].move.in), buff, bp); break;
				case 'o': safe_str(unparse_number(sdb[x].move.out), buff, bp); break;
				case 'r': safe_str(unparse_number(sdb[x].move.ratio), buff, bp); break;
				case 'c': safe_str(unparse_number(sdb[x].move.cochranes), buff, bp); break;
				case 'v': safe_str(unparse_number(sdb[x].move.v), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].move.empire), buff, bp); break;
				case 'q': safe_str(unparse_integer(sdb[x].move.quadrant), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
/* object ----------------------------------------------------------------- */
		case 'o': safe_str(unparse_dbref(sdb[x].object), buff, bp); break;
/* power ------------------------------------------------------------------ */
		case 'p': switch (f2[0]) {
			case 'v': safe_str(unparse_integer(sdb[x].power.version), buff, bp); break;
			case 'm': safe_str(unparse_number(sdb[x].power.main), buff, bp); break;
			case 'a': safe_str(unparse_number(sdb[x].power.aux), buff, bp); break;
			case 'b': safe_str(unparse_number(sdb[x].power.batt), buff, bp); break;
			case 't': safe_str(unparse_number(sdb[x].power.total), buff, bp); break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
		case 's': switch (f1[1]) {
/* sensor ----------------------------------------------------------------- */
			case 'e': switch (f2[0]) {
				case 'c': switch (f2[2]) {
					case 'n': safe_str(unparse_integer(sdb[x].sensor.contacts), buff, bp); break;
					case 'u': safe_str(unparse_integer(sdb[x].sensor.counter), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'e': switch (f3[0]) {
					case 'a': safe_str(unparse_integer(sdb[x].sensor.ew_active), buff, bp); break;
					case 'd': safe_str(unparse_number(sdb[x].sensor.ew_damage), buff, bp); break;
					case 'e': safe_str(unparse_integer(sdb[x].sensor.ew_exist), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'l': switch (f3[0]) {
					case 'a': safe_str(unparse_integer(sdb[x].sensor.lrs_active), buff, bp); break;
					case 'd': safe_str(unparse_number(sdb[x].sensor.lrs_damage), buff, bp); break;
					case 'e': safe_str(unparse_integer(sdb[x].sensor.lrs_exist), buff, bp); break;
					case 'r': safe_str(unparse_number(sdb[x].sensor.lrs_resolution), buff, bp); break;
					case 's': safe_str(unparse_number(sdb[x].sensor.lrs_signature), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 's': switch (f3[0]) {
					case 'a': safe_str(unparse_integer(sdb[x].sensor.srs_active), buff, bp); break;
					case 'd': safe_str(unparse_number(sdb[x].sensor.srs_damage), buff, bp); break;
					case 'e': safe_str(unparse_integer(sdb[x].sensor.srs_exist), buff, bp); break;
					case 'r': safe_str(unparse_number(sdb[x].sensor.srs_resolution), buff, bp); break;
					case 's': safe_str(unparse_number(sdb[x].sensor.srs_signature), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'v': switch (f2[1]) {
					case 'e': safe_str(unparse_integer(sdb[x].sensor.version), buff, bp); break;
					case 'i': safe_str(unparse_number(sdb[x].sensor.visibility), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* shield ----------------------------------------------------------------- */
			case 'h': switch (f2[0]) {
				case 'a': switch (f3[0]) {
					case 'f': safe_str(unparse_integer(sdb[x].shield.active[0]), buff, bp); break;
					case 's': safe_str(unparse_integer(sdb[x].shield.active[1]), buff, bp); break;
					case 'a': safe_str(unparse_integer(sdb[x].shield.active[2]), buff, bp); break;
					case 'p': safe_str(unparse_integer(sdb[x].shield.active[3]), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'd': switch (f3[0]) {
					case 'f': safe_str(unparse_number(sdb[x].shield.damage[0]), buff, bp); break;
					case 's': safe_str(unparse_number(sdb[x].shield.damage[1]), buff, bp); break;
					case 'a': safe_str(unparse_number(sdb[x].shield.damage[2]), buff, bp); break;
					case 'p': safe_str(unparse_number(sdb[x].shield.damage[3]), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'e': safe_str(unparse_integer(sdb[x].shield.exist), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].shield.freq), buff, bp); break;
				case 'm': safe_str(unparse_integer(sdb[x].shield.maximum), buff, bp); break;
				case 'r': safe_str(unparse_number(sdb[x].shield.ratio), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* space ------------------------------------------------------------------ */
			case 'p': safe_str(unparse_integer(sdb[x].space), buff, bp); break;
			case 't': switch (f1[2]) {
/* status ----------------------------------------------------------------- */
				case 'a': switch (f2[0]) {
					case 'a': switch (f2[1]) {
						case 'c': safe_str(unparse_integer(sdb[x].status.active), buff, bp); break;
						case 'u': safe_str(unparse_integer(sdb[x].status.autopilot), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'c': switch (f2[1]) {
						case 'o': safe_str(unparse_integer(sdb[x].status.connected), buff, bp); break;
						case 'r': safe_str(unparse_integer(sdb[x].status.crippled), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'd': safe_str(unparse_integer(sdb[x].status.docked), buff, bp); break;
					case 'i': safe_str(unparse_integer(sdb[x].status.time), buff, bp); break;
					case 'l': switch (f2[1]) {
						case 'a': safe_str(unparse_integer(sdb[x].status.landed), buff, bp); break;
						case 'i': safe_str(unparse_integer(sdb[x].status.link), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'o': switch (f3[0]) {
						case 'l': safe_str(unparse_integer(sdb[x].status.open_landing), buff, bp); break;
						case 'd': safe_str(unparse_integer(sdb[x].status.open_docking), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 't': switch (f2[7]) {
						case 'e': safe_str(unparse_integer(sdb[x].status.tractored), buff, bp); break;
						case 'i': safe_str(unparse_integer(sdb[x].status.tractoring), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
/* structure -------------------------------------------------------------- */
				case 'r': switch (f2[0]) {
					case 'c': switch (f3[0]) {
						case 'd': safe_str(unparse_integer(sdb[x].structure.can_dock), buff, bp); break;
						case 'l': safe_str(unparse_integer(sdb[x].structure.can_land), buff, bp); break;
						case 'h': safe_str(unparse_number(sdb[x].structure.cargo_hold), buff, bp); break;
						case 'm': safe_str(unparse_number(sdb[x].structure.cargo_mass), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'd': safe_str(unparse_number(sdb[x].structure.displacement), buff, bp); break;
					case 'h': switch (f3[0]) {
						case 'd': safe_str(unparse_integer(sdb[x].structure.has_docking_bay), buff, bp); break;
						case 'l': safe_str(unparse_integer(sdb[x].structure.has_landing_pad), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'm': switch (f3[0]) {
						case 'r': safe_str(unparse_integer(sdb[x].structure.max_repair), buff, bp); break;
						case 's': safe_str(unparse_integer(sdb[x].structure.max_structure), buff, bp); break;
						default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
						} break;
					case 'r': safe_str(unparse_number(sdb[x].structure.repair), buff, bp); break;
					case 's': safe_str(unparse_number(sdb[x].structure.superstructure), buff, bp); break;
					case 't': safe_str(unparse_integer(sdb[x].structure.type), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
		case 't': switch (f1[3]) {
/* tract ------------------------------------------------------------------ */
			case 'c': switch (f2[0]) {
				case 'c': safe_str(unparse_integer(sdb[x].tract.cost), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].tract.freq), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].tract.exist), buff, bp); break;
				case 'a': safe_str(unparse_integer(sdb[x].tract.active), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].tract.damage), buff, bp); break;
				case 'l': safe_str(unparse_integer(sdb[x].tract.lock), buff, bp); break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* tech ------------------------------------------------------------------- */
			case 'h': switch (f2[0]) {
				case 'a': switch (f2[1]) {
					case 'r': safe_str(unparse_number(sdb[x].tech.armor), buff, bp); break;
					case 'u': safe_str(unparse_number(sdb[x].tech.aux_max), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'c': safe_str(unparse_number(sdb[x].tech.cloak), buff, bp); break;
				case 'f': switch (f2[1]) {
					case 'i': safe_str(unparse_number(sdb[x].tech.firing), buff, bp); break;
					case 'u': safe_str(unparse_number(sdb[x].tech.fuel), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				case 'l': safe_str(unparse_number(sdb[x].tech.ly_range), buff, bp); break;
				case 'm': safe_str(unparse_number(sdb[x].tech.main_max), buff, bp); break;
				case 's': switch (f2[1]) {
					case 't': safe_str(unparse_number(sdb[x].tech.stealth), buff, bp); break;
					case 'e': safe_str(unparse_number(sdb[x].tech.sensors), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
/* trans ------------------------------------------------------------------ */
			case 'n': switch (f2[0]) {
				case 'c': safe_str(unparse_integer(sdb[x].trans.cost), buff, bp); break;
				case 'f': safe_str(unparse_number(sdb[x].trans.freq), buff, bp); break;
				case 'e': safe_str(unparse_integer(sdb[x].trans.exist), buff, bp); break;
				case 'a': safe_str(unparse_integer(sdb[x].trans.active), buff, bp); break;
				case 'd': safe_str(unparse_number(sdb[x].trans.damage), buff, bp); break;
				case 'l': switch (f3[0]) {
					case 'd': safe_str(unparse_integer(sdb[x].trans.d_lock), buff, bp); break;
					case 's': safe_str(unparse_integer(sdb[x].trans.s_lock), buff, bp); break;
					default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
					} break;
				default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
				} break;
			default: safe_str("#-1 NO SUCH FIELD", buff, bp); break;
			} break;
		default: safe_str("#-1 NO SUCH FIELD", buff, bp); break; }

	return;
}

/* ------------------------------------------------------------------------ */

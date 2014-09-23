/* space_utils.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

int GoodSDB (int x)
{
	if (x < MIN_SPACE_OBJECTS || x > max_space_objects) {
		return 0;
	} else if (!sdb[x].structure.type) {
		return 0;
	} else if (!SpaceObj(sdb[x].object) || !GoodObject(sdb[x].object)) {
		return 0;
	} else
		return 1;
}

/* ------------------------------------------------------------------------ */

double ly2pc (double dist)
{
	return (dist * LIGHTYEAR / PARSEC);
}

double pc2ly (double dist)
{
	return (dist * PARSEC / LIGHTYEAR);
}

double ly2su (double dist)
{
	return (dist * LIGHTYEAR);
}

double pc2su (double dist)
{
	return (dist * PARSEC);
}

double su2ly (double dist)
{
	return (dist / LIGHTYEAR);
}

double su2pc (double dist)
{
	return (dist / PARSEC);
}

/* ------------------------------------------------------------------------ */

int db2sdb (dbref name) /* Returns the sdb# of a DB# space object */
{
	register int i;

	
	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
		if (sdb[i].structure.type)
			if (SpaceObj(sdb[i].object) && GoodObject(sdb[i].object))
				if ( name==sdb[i].object)
					return i;
	return SENSOR_FAIL;
}


/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */

double xy2bearing (double x, double y)
{
	if (y == 0.0) {
		if (x == 0.0) {
			return 0.0;
		} else if (x > 0.0) {
			return 0.0;
		} else
			return 180.0;
	} else if (x == 0.0) {
		if (y > 0.0) {
			return 90.0;
		} else
			return 270.0;
	} else if (x > 0.0) {
		if (y > 0.0) {
			return atan(y / x) * 180.0 / PI;
		} else
			return atan(y / x) * 180.0 / PI + 360.0;
	} else if (x < 0.0)
		return atan(y / x) * 180.0 / PI + 180.0;
	return 0.0;
}

double xyz2elevation (double x, double y, double z)
{
	double r = sqrt(x * x + y * y);

	if (r == 0.0) {
		if (z == 0.0) {
			return 0.0;
		} else if (z > 0.0) {
			return 90.0;
		} else
			return 270.0;
	} else if (z > 0.0) {
		return atan(z / r) * 180.0 / PI;
	} else if (z < 0.0) {
		return atan(z / r) * 180.0 / PI + 360;
	} else
		return 0.0;
}

double xyz2range (double xa, double ya, double za, double xb, double yb, double zb)
{
	double x = xb - xa;
	double y = yb - ya;
	double z = zb - za;

	return sqrt(x * x + y * y + z * z);
}

#ifdef WIN32
static int round (double x)
{
	if (x < 0.0) {
		return (int) (x - 0.5);
	} else {
		return (int) (x + 0.5);
	}
}
#endif

double xyz2vis (double x, double y, double z)
{
	double px = x / PARSEC;
	double py = y / PARSEC;
	double pz = z / PARSEC;
	double dx = fabs(px - (round(px / 100.0) * 100.0));
	double dy = fabs(py - (round(py / 100.0) * 100.0));
	double dz = fabs(pz - (round(pz / 100.0) * 100.0));
	double vis;
	register int i = 0;
	aspace_borders *nebula;
	double range;
		
	if (aspace_config.nebula >= 1 && im_count(nebula_map) > 0) {
		for (i = 1; i <= im_count(nebula_map); i++) {
			nebula = im_find(nebula_map, i);
			if (!nebula)
				continue;
			range = xyz2range(x, y, z, nebula->x, nebula->y, nebula->z) / PARSEC;
			if (range > nebula->radius)
				continue;
			// If we get this far, we are inside a custom nebula. 
			dx = (px - (nebula->x / PARSEC)) / (0.5 * nebula->radius);
			dy = (py - (nebula->y / PARSEC)) / (0.5 * nebula->radius);
			dz = (pz - (nebula->z / PARSEC)) / (0.5 * nebula->radius);
			vis = 1.1 - (1.0 / (1.0 + dx * dx + dy * dy + dz * dz));
			return ( vis > 1.0 ? 1.0 : vis < 0 ? 0.0 : vis);
		}
	} else if (aspace_config.nebula <= 1)  {
		vis = 1.1 - (1.0 / (1.0 + dx * dx + dy * dy + dz * dz));
		return ( vis > 1.0 ? 1.0 : vis < 0 ? 0.0 : vis);
	}
	return 1.0;
}
/* ------------------------------------------------------------------------ */

double sdb2bearing (int n1, int n2)
{
	double x = sdb[n2].coords.x - sdb[n1].coords.x;
	double y = sdb[n2].coords.y - sdb[n1].coords.y;

	return xy2bearing(x, y);
}

/* ------------------------------------------------------------------------ */

double sdb2elevation (int n1, int n2)
{
	double x = sdb[n2].coords.x - sdb[n1].coords.x;
	double y = sdb[n2].coords.y - sdb[n1].coords.y;
	double z = sdb[n2].coords.z - sdb[n1].coords.z;

	return xyz2elevation(x, y, z);
}

/* ------------------------------------------------------------------------ */

double sdb2range (int n1, int n2)
{
	return xyz2range(sdb[n1].coords.x, sdb[n1].coords.y, sdb[n1].coords.z,
	  sdb[n2].coords.x, sdb[n2].coords.y, sdb[n2].coords.z);
}

/* ------------------------------------------------------------------------ */

int sdb2arc (int n1, int n2)
{
	int firing_arc = 0;
	double x = sdb[n2].coords.x - sdb[n1].coords.x - 0.1;
	double y = sdb[n2].coords.y - sdb[n1].coords.y - 0.1;
	double z = sdb[n2].coords.z - sdb[n1].coords.z - 0.1;
	double r = sqrt(x * x + y * y + z * z);
	double v1, v2, v3;
	double forward_arc;
	double starboard_arc;
	double up_arc;

	if (r == 0.0) { // We should never get here, but just incase. Dividing by 0 explodes the universe.
		firing_arc = 63;
	} else {
		v1 = (x * sdb[n1].course.d[0][0] + y * sdb[n1].course.d[0][1] +
		  z * sdb[n1].course.d[0][2]) / r / sqrt(sdb[n1].course.d[0][0] *
		  sdb[n1].course.d[0][0] + sdb[n1].course.d[0][1] *
		  sdb[n1].course.d[0][1] + sdb[n1].course.d[0][2] *
		  sdb[n1].course.d[0][2]);
		v2 = (x * sdb[n1].course.d[1][0] + y * sdb[n1].course.d[1][1] +
		  z * sdb[n1].course.d[1][2]) / r / sqrt(sdb[n1].course.d[1][0] *
		  sdb[n1].course.d[1][0] + sdb[n1].course.d[1][1] *
		  sdb[n1].course.d[1][1] + sdb[n1].course.d[1][2] *
		  sdb[n1].course.d[1][2]);
		v3 = (x * sdb[n1].course.d[2][0] + y * sdb[n1].course.d[2][1] +
		  z * sdb[n1].course.d[2][2]) / r / sqrt(sdb[n1].course.d[2][0] *
		  sdb[n1].course.d[2][0] + sdb[n1].course.d[2][1] *
		  sdb[n1].course.d[2][1] + sdb[n1].course.d[2][2] *
		  sdb[n1].course.d[2][2]);
		v1 = (v1 > 1.0) ? 1.0 : (v1 < -1.0) ? -1.0 : v1;
		v2 = (v2 > 1.0) ? 1.0 : (v2 < -1.0) ? -1.0 : v2;
		v3 = (v3 > 1.0) ? 1.0 : (v3 < -1.0) ? -1.0 : v3;
		forward_arc = acos(v1) * 180 / PI;
		starboard_arc = acos(v2) * 180 / PI;
		up_arc = acos(v3) * 180 / PI;
		if (forward_arc < 89.0) {
			firing_arc += 1;
		} else if (forward_arc > 91.0) {
			firing_arc += 4;
		} else {
			firing_arc += 5;
		}
		if (starboard_arc < 89.0) {
			firing_arc += 2;
		} else if (starboard_arc > 91.0) {
			firing_arc += 8;
		} else {
			firing_arc += 10;
		}
		if (up_arc < 89.0) {
			firing_arc += 16;
		} else if (up_arc > 91.0) {
			firing_arc += 32;
		} else {
			firing_arc += 48;
		}
	}
	return firing_arc;
}

/* ------------------------------------------------------------------------ */

int sdb2shield (int n1, int n2)
{
	double x = sdb[n2].coords.x - sdb[n1].coords.x - 0.1;
	double y = sdb[n2].coords.y - sdb[n1].coords.y - 0.1;
	double z = sdb[n2].coords.z - sdb[n1].coords.z - 0.1;
	double r = sqrt(x * x + y * y + z * z);
	double v1, v2, v3;
	double fv1, fv2, fv3;
	double forward_arc;
	double starboard_arc;
	double up_arc;

	if (r == 0.0) {
		return 0;
	} else {
		v1 = (x * sdb[n1].course.d[0][0] + y * sdb[n1].course.d[0][1] +
		  z * sdb[n1].course.d[0][2]) / r / sqrt(sdb[n1].course.d[0][0] *
		  sdb[n1].course.d[0][0] +	sdb[n1].course.d[0][1] *
		  sdb[n1].course.d[0][1] + sdb[n1].course.d[0][2] *
		  sdb[n1].course.d[0][2]);
		v2 = (x * sdb[n1].course.d[1][0] + y * sdb[n1].course.d[1][1] +
		  z * sdb[n1].course.d[1][2]) / r / sqrt(sdb[n1].course.d[1][0] *
		  sdb[n1].course.d[1][0] +	sdb[n1].course.d[1][1] *
		  sdb[n1].course.d[1][1] + sdb[n1].course.d[1][2] *
		  sdb[n1].course.d[1][2]);
		v3 = (x * sdb[n1].course.d[2][0] + y * sdb[n1].course.d[2][1] +
		  z * sdb[n1].course.d[2][2]) / r / sqrt(sdb[n1].course.d[2][0] *
		  sdb[n1].course.d[2][0] + sdb[n1].course.d[2][1] *
		  sdb[n1].course.d[2][1] + sdb[n1].course.d[2][2] *
		  sdb[n1].course.d[2][2]);
		v1 = (v1 > 1.0) ? 1.0 : (v1 < -1.0) ? -1.0 : v1;
		v2 = (v2 > 1.0) ? 1.0 : (v2 < -1.0) ? -1.0 : v2;
		v3 = (v3 > 1.0) ? 1.0 : (v3 < -1.0) ? -1.0 : v3;
		fv1 = fabs(v1);
		fv2 = fabs(v2);
		fv3 = fabs(v3);
		
		if (fv1 >= 0.5 && fv1 > fv2 && fv1 > fv3)  // Forward and aft
			return (v1 > 0.0 ? 0 : 2);
		else if (fv2 >= 0.5 && fv2 > fv1 && fv2 > fv3) // Starboard and Port
			return (v2 > 0.0 ? 1 : 3);
		else if (fv3 >= 0.5 && fv3 > fv1 && fv3 > fv2) // Dorsal and Ventral
			return (v3 > 0.0 ? 4 : 5);
		else {
			write_spacelog(sdb[n1].object, sdb[n2].object, "SDB2Shield(): Shield facing error.");
			return 0;
		}
		
	}
}

/* ------------------------------------------------------------------------ */

int arc_check (int contact, int weapon)
{
	int x = (contact & weapon);

	if (((x & 16) || (x & 32)) && ((x & 1) || (x & 4)) && ((x & 2) || (x & 8))) {
		return x;
	} else
		return ARC_FAIL;
}

/* ------------------------------------------------------------------------ */

int get_empty_sdb ()  /* Returns empty sdb slot, or VACANCY_FAIL if there is none */
{
	register int i;

	for (i = MIN_SPACE_OBJECTS ; i <= MAX_SPACE_OBJECTS ; ++i) {
		if (sdb[i].structure.type == 0) {
			return i;
			break;
		}
	}
	return VACANCY_FAIL;
}

/* ------------------------------------------------------------------------ */

int contact2sdb (int x, int c) /* Returns the sdb# of a contact# */
{
	register int i;

	for (i = 0 ; i < sdb[x].sensor.contacts ; ++i)
		if (c == sdb[x].slist.num[i]) {
			return (sdb[x].slist.sdb[i]);
			break;
		}
	return SENSOR_FAIL;
}

/* ------------------------------------------------------------------------ */

int sdb2contact (int x, int s) /* Returns the contact# of an sdb# */
{
	register int i;

	for (i = 0 ; i < sdb[x].sensor.contacts ; ++i)
		if (s == sdb[x].slist.sdb[i]) {
			return (sdb[x].slist.num[i]);
			break;
		}
	return SENSOR_FAIL;
}

/* ------------------------------------------------------------------------ */

int contact2slist (int x, int c) /* Returns the slist# of a contact# */
{
	register int i;

	for (i = 0 ; i < sdb[x].sensor.contacts ; ++i)
		if (c == sdb[x].slist.num[i]) {
			return i;
			break;
		}
	return SENSOR_FAIL;
}

/* ------------------------------------------------------------------------ */

int sdb2slist (int x, int s) /* Returns the slist# of an sdb# */
{
	register int i;

	for (i = 0 ; i < sdb[x].sensor.contacts ; ++i)
		if (s == sdb[x].slist.sdb[i]) {
			return i;
			break;
		}
	return SENSOR_FAIL;
}

/* ------------------------------------------------------------------------ */

int name2sdb (char *name) /* Returns the sdb# of a named space object */
{
	register int i;

	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i)
		if (sdb[i].structure.type)
			if (SpaceObj(sdb[i].object) && GoodObject(sdb[i].object))
				if (local_wild_match(name, Name(sdb[i].object), NULL))
					return i;
	return SENSOR_FAIL;
}

/* ------------------------------------------------------------------------ */

double sdb2max_antimatter (int x)
{
	return (sdb[x].move.ratio * sdb[x].tech.ly_range * 320000000.0);
}

/* ------------------------------------------------------------------------ */

double sdb2max_deuterium (int x)
{
	return (sdb[x].move.ratio * sdb[x].tech.ly_range * 640000000.0);
}

/* ------------------------------------------------------------------------ */

double sdb2max_reserve (int x)
{
	return (sdb[x].batt.gw * 3600.0);
}

/* ------------------------------------------------------------------------ */

double sdb2max_warp (int x)
{
	double a = sdb[x].move.ratio;
	double p = (0.99 * sdb[x].power.main) + (0.01 * sdb[x].power.total * sdb[x].alloc.movement);

	if (a <= 0.0)
		return 0.0;
	if (p <= 0.0)
		return 0.0;
	if (sdb[x].status.tractoring) {
		a *= (sdb[x].structure.displacement + sdb[sdb[x].status.tractoring].structure.displacement + 0.1) /
		  (sdb[x].structure.displacement + 0.1);
	} else if (sdb[x].status.tractored)
		a *= (sdb[x].structure.displacement + sdb[sdb[x].status.tractored].structure.displacement + 0.1) /
		  (sdb[x].structure.displacement + 0.1);

	a = sqrt(10.0 * p / a);
	if (a < 1.0) {
		return 0.0;
	} else
		return a/2;
}

/* ------------------------------------------------------------------------ */

double sdb2max_impulse (int x)
{
	double a = sdb[x].move.ratio;
	double p = (0.9 * sdb[x].power.aux) + (0.1 * sdb[x].power.total * sdb[x].alloc.movement);

	if (a <= 0.0)
		return 0.0;
	if (p <= 0.0)
		return 0.0;

	if (sdb[x].status.tractoring) {
		a *= (sdb[x].structure.displacement + sdb[sdb[x].status.tractoring].structure.displacement + 0.1) /
		  (sdb[x].structure.displacement + 0.1);
	} else if (sdb[x].status.tractored)
		a *= (sdb[x].structure.displacement + sdb[sdb[x].status.tractored].structure.displacement + 0.1) /
		  (sdb[x].structure.displacement + 0.1);

	a = 1.0 - 0.5 * a / p;
	if (a <= 0.0 || a >= 1.0) {
		return 0.0;
	} else
		return a;
}

/* ------------------------------------------------------------------------ */

double sdb2cruise_warp (int x)
{
	double a;

	if (sdb[x].move.ratio <= 0.0)
		return 0.0;
	if (sdb[x].main.gw <= 0.0)
		return 0.0;
	if (sdb[x].engine.warp_damage <= 0.0)
		return 0.0;

	a = sqrt(10.0 * sdb[x].main.gw / sdb[x].move.ratio);
	if (a < 1.0)
		return 0.0;
	a *= sdb[x].engine.warp_damage;
	if (a < 1.0) {
		return 0.0;
	} else
		return a/2;
}

/* ------------------------------------------------------------------------ */

double sdb2cruise_impulse (int x)
{
	double a;

	if (sdb[x].move.ratio <= 0.0)
		return 0.0;
	if (sdb[x].aux.gw <= 0.0)
		return 0.0;
	if (sdb[x].engine.impulse_damage <= 0.0)
		return 0.0;

	a = 1.0 - 0.5 * sdb[x].move.ratio / sdb[x].aux.gw;
	if (a <= 0.0 || a >= 1.0)
		return 0.0;
	a *= sdb[x].engine.impulse_damage;
	if (a < 0.0) {
		return 0.0;
	} else
		return a;
}

/* ------------------------------------------------------------------------ */

double sdb2ecm_lrs (int x)
{
	if (sdb[x].sensor.ew_active) {
		return sqrt(1.0 + sdb[x].power.total * sdb[x].alloc.ecm * sdb[x].sensor.ew_damage * sdb[x].tech.sensors / 10.0);
	} else
		return 1.0;
}

/* ------------------------------------------------------------------------ */

double sdb2eccm_lrs (int x)
{
	if (sdb[x].sensor.ew_active) {
		return sqrt(1.0 + sdb[x].power.total * sdb[x].alloc.eccm * sdb[x].sensor.ew_damage * sdb[x].tech.sensors / 10.0);
	} else
		return 1.0;
}

/* ------------------------------------------------------------------------ */

double sdb2ecm_srs (int x)
{
	if (sdb[x].sensor.ew_active) {
		return sqrt(1.0 + sdb[x].power.total * sdb[x].alloc.ecm * sdb[x].sensor.ew_damage * sdb[x].tech.sensors);
	} else
		return 1.0;
}

/* ------------------------------------------------------------------------ */

double sdb2eccm_srs (int x)
{
	if (sdb[x].sensor.ew_active) {
		return sqrt(1.0 + sdb[x].power.total * sdb[x].alloc.eccm * sdb[x].sensor.ew_damage * sdb[x].tech.sensors);
	} else
		return 1.0;
}

/* ------------------------------------------------------------------------ */

double sdb2dissipation (int x, int shield)
{
	double d;

	if (sdb[x].shield.active[shield] && sdb[x].shield.damage[shield] > 0.0) {
		d = (2 - pow(2, (1 - sdb[x].alloc.shield[shield] *
		  sdb[x].power.total * sdb[x].shield.damage[shield] *
		  sdb[x].shield.ratio * sdb[x].sensor.visibility /
		  sdb[x].shield.maximum))) * sdb[x].shield.maximum;
		if (d > 1.0) {
			return d;
		} else
			return 0.0;
	} else
		return 0.0;
}

/* ------------------------------------------------------------------------ */

double xyz2cochranes (double x, double y, double z)
{
	double cochranes = atof(aspace_config.cochrane);
	
	double px = x / PARSEC;
	double py = y / PARSEC;
	double pz = z / PARSEC;
	double r = (px * px + py * py) / 256000000.0 + (pz * pz) / 240000.0;

	if (r < 1.0) {
		return ((1.0 - r) / 0.671223 * cochranes) + 1.0;
	} else
		return 1.0;
}
/* ------------------------------------------------------------------------ */

double sdb2angular (int n1, int n2)
{
	double a[3], b[3], dot, mag, x;

	a[0] = sdb[n2].coords.x - sdb[n1].coords.x;
	a[1] = sdb[n2].coords.y - sdb[n1].coords.y;
	a[2] = sdb[n2].coords.z - sdb[n1].coords.z;

	b[0] = (sdb[n2].move.v * sdb[n2].course.d[0][0]) - (sdb[n1].move.v * sdb[n1].course.d[0][0]) + a[0];
	b[1] = (sdb[n2].move.v * sdb[n2].course.d[0][1]) - (sdb[n1].move.v * sdb[n1].course.d[0][1]) + a[1];
	b[2] = (sdb[n2].move.v * sdb[n2].course.d[0][2]) - (sdb[n1].move.v * sdb[n1].course.d[0][2]) + a[2];

	dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	mag = sqrt((a[0] * a[0] + a[1] * a[1] + a[2] * a[2]) * (b[0] * b[0] + b[1] * b[1] + b[2] * b[2]));

	if (mag == 0)
		return 0;

	x = dot / mag;
	if (x > 1.0) {
		x = 1.0;
	} else if (x < -1.0)
		x = -1.0;

	return fabs(acos(x) * 180 / PI);
}

/* ------------------------------------------------------------------------ */

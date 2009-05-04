/* space_nebula.c */

#include "config.h"
#include "space.h"


/* ------------------------------------------------------------------------ */

/* Nebula Table: Format: Nebula Name, Radius, X (SU), Y (SU), Z(SU)
 *
 * Please Note: The NULL at the bottom IS REQUIRED
 */
struct aspace_empire_info aspace_nebula_table[] =
{
	{"Horse-Head Nebula",	7.0,	-28307977141382.1,		  57217386371.0,	-37716017559.8},
	{NULL,					0.0,				  0.0,					0.0,			   0.0}
};

int round2 (double x)
{
	if (x < 0.0) {
		return (int) (x - 0.5);
	} else
		return (int) (x + 0.5);
}

int do_nebula_report (dbref enactor)
{
	static char buffer[BUFFER_LEN], nbuff[20];
	double px = sdb[n].coords.x / PARSEC;
	double py = sdb[n].coords.y / PARSEC;
	double pz = sdb[n].coords.z / PARSEC;
	double cx[2], cy[2], cz[2];
	double dx, dy, dz;
	register int x, y, z;

	cx[0] = ceil(px / 100.0) * 100.0;
	cx[1] = floor(px / 100.0) * 100.0;
	cy[0] = ceil(py / 100.0) * 100.0;
	cy[1] = floor(py / 100.0) * 100.0;
	cz[0] = ceil(pz / 100.0) * 100.0;
	cz[1] = floor(pz / 100.0) * 100.0;

	if (error_on_console(enactor)) {
		return 0;
	} else if (!sdb[n].sensor.lrs_exist) {
		notify(enactor, ansi_red(tprintf("%s has no long-range sensors.", Name(sdb[n].object))));
	} else if (!sdb[n].sensor.lrs_active) {
		notify(enactor, ansi_red("Long-range sensors are inactive."));
	} else {
		snprintf(buffer, sizeof(buffer),
		  "%s%s--[%sNebula Report%s]--------------------------------------------------------------%s\n%sNebulae              Bearing Range   Core Vis Core Coordinates%s\n%s-------------------- ------- ------- -------- ---------------------------------%s\n",
		  ANSI_HILITE, ANSI_BLUE, ANSI_YELLOW, ANSI_BLUE, ANSI_NORMAL,
		  ANSI_CYAN, ANSI_WHITE, ANSI_BLUE, ANSI_WHITE);

		for (z = 0; z < 2; ++z) {
			for (y = 0; y < 2; ++y) {
				for (x = 0; x < 2; ++x) {
					snprintf(nbuff, sizeof(nbuff), "Nebula %d-%d-%d",
					  round2(fabs(cx[x] / 100.0)), round2(fabs(cy[y] / 100.0)), round2(fabs(cz[z] / 100.0)));
					dx = cx[x] * PARSEC;
					dy = cy[y] * PARSEC;
					dz = cz[z] * PARSEC;
					strncat(buffer, tprintf("%-20.20s %3d %-3d %-7.7s %7.3f%% %10.3f %10.3f %10.3f\n", nbuff,
					  (int) round2(xy2bearing((dx - sdb[n].coords.x), (dy - sdb[n].coords.y))),
					  (int) round2(xyz2elevation((dx - sdb[n].coords.x), (dy - sdb[n].coords.y), (dz - sdb[n].coords.z))),
					  unparse_range(xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, dx, dy, dz)),
					  xyz2vis(dx, dy, dz) * 100.0,
					  cx[x] - sdb[n].coords.xo / PARSEC,
					  cy[y] - sdb[n].coords.yo / PARSEC,
					  cz[z] - sdb[n].coords.zo / PARSEC), sizeof(buffer) - 1);
				}
			}
		}

		strncat(buffer, format_l_line(), sizeof(buffer) - 1);
		strncat(buffer, format_Course(n), sizeof(buffer) - 1);
		strncat(buffer, format_Speed(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_Location(n), sizeof(buffer) - 1);
		strncat(buffer, format_Velocity(n), sizeof(buffer) - 1);
		strncat(buffer, "\n", sizeof(buffer) - 1);
		strncat(buffer, format_l_end(), sizeof(buffer) - 1);

		notify(enactor, buffer);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */


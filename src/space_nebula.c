/* space_nebula.c */

#include "config.h"
#include "space.h"


/* ------------------------------------------------------------------------ */


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
	register int i = 0;
	aspace_borders *nebula;
	double range;

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
		if (aspace_config.nebula >= 1 && im_count(nebula_map) > 0) {
			for (i = 1; i <= im_count(nebula_map); i++) {
				nebula = im_find(nebula_map, i);
				if (!nebula)
					continue;
				range = xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, nebula->x, nebula->y, nebula->z) / PARSEC;
				if (fabs(range - nebula->radius) >= 150.0)
					continue;
				strncat(buffer, tprintf("%-20.20s %3d %-3d %-7.7s %7.3f%% %10.3f %10.3f %10.3f\n", nebula->name,
					  (int) round2(xy2bearing((nebula->x - sdb[n].coords.x), (nebula->y - sdb[n].coords.y))),
					  (int) round2(xyz2elevation((nebula->x - sdb[n].coords.x), (nebula->y - sdb[n].coords.y), (nebula->z - sdb[n].coords.z))),
					  unparse_range(xyz2range(sdb[n].coords.x, sdb[n].coords.y, sdb[n].coords.z, nebula->x, nebula->y, nebula->z)),
					  xyz2vis(nebula->x, nebula->y, nebula->z) * 100.0,
					  nebula->x / PARSEC, nebula->y / PARSEC, nebula->z / PARSEC), sizeof(buffer) - 1);
			}
		}
		if (aspace_config.nebula <= 1) {
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
void free_nebulainfo(void *ptr) {
  aspace_borders *nebula = (aspace_borders *) ptr;
  mush_free(nebula->name, "spacenebula_name");
  mush_free(nebula, "nebula_info");
}

void addNewNebula(dbref executor, int index, const char* name, double radius, double x, double y, double z, char *buff, char **bp)
{
	aspace_borders* newNebula;
	newNebula = im_find(nebula_map, index);
	
	if (newNebula != NULL) {
		safe_str("#-1 NEBULA # ALREADY IN USE", buff, bp);
		return;
	}
		
	newNebula = mush_malloc(sizeof(aspace_borders), "nebula_info");
	
	newNebula->name = mush_strdup(name, "spacenebula_name");
	newNebula->empire_id = 0;
	newNebula->radius = radius;
	newNebula->x = x;
	newNebula->y = y;
	newNebula->z = z;
	
	if( im_insert(nebula_map, index, newNebula )) {
		safe_str("New nebula Created.", buff, bp);
		write_spacelog(executor, executor, tprintf("Nebula Created: %s", newNebula->name));
	} else
		safe_str("#-1 NEBULA NOT CREATED.", buff, bp);
}

void deleteNebula(dbref executor, int index, char *buff, char **bp)
{
	aspace_borders *nebula;

	if (!index) {
		safe_str("#-1 NEBULA NOT SUPPLIED", buff, bp);
		return;
	}
	if (aspace_config.nebula < 1) {
		safe_str("#-1 DISABLED IN CONFIG", buff, bp);
		return;
	}
	
	nebula = im_find(nebula_map, index);
	if (nebula !=NULL) {
		write_spacelog(executor, executor, tprintf("Nebula deleted: %s", nebula->name));
		free_borderinfo(nebula);
		im_delete(nebula_map, index);
	} else {
		safe_str("#-1 NEBULA NOT FOUND", buff, bp);
		return;
	}
}

void list_nebulae(char *buff, char **bp)
{
	int index = 0;
	aspace_borders *nebula;
   
	safe_format(buff, bp, "%-20.20s %-7.7s %-10.10s %-10.10s %-10.10s\n", "Name", "Radius", "X", "Y", "Z");
	for (index = 1; index <= im_count(border_map); index++) {
		nebula = im_find(nebula_map, index);
		if (!nebula)
			continue;
		safe_format(buff, bp, "%-20.20s %-7.7s %-10.3f %-10.3f %-10.3f\n", nebula->name, unparse_range(nebula->radius), 
			nebula->x / PARSEC, nebula->y / PARSEC, nebula->z / PARSEC);
	
    }
}
/* space_output.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *output_shields_raw ()
{
	int i;
	static char buffer[BUFFER_LEN];
	
	/* Allocate 6, 7, 8, 9 and 10 */
	/* Shield 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 and 12 */
	
	/* Master Shield Information */
	snprintf(buffer, sizeof(buffer), "%f|%f|%f|%d|%3.3f|%d", sdb[n].power.total, sdb[n].alloc.shields, sdb[n].shield.ratio, sdb[n].shield.maximum, sdb[n].shield.freq, sdb[n].shield.exist);
	
	for (i = 0; i < MAX_SHIELD_NAME; ++i) {
		strncat(
			buffer, 
			tprintf("~%s|%f|%f|%d|%f", unparse_shield(i), sdb[n].alloc.shield[i] * sdb[n].power.total, sdb[n].alloc.shield[i], sdb[n].shield.active[i], sdb[n].shield.damage[i]), 
			sizeof(buffer) - 1
		);
	}
		
	return buffer;
}

char *output_beams_raw ()
{	
	/* Allocate 12 */
	/* Beam 1, 2, 3, 4, 5 */
	/* Beam_Active, Beam_Name, Beame_Damage, Beam_Bonus, Beam_Cost, Beam_Range, Beam_Arcs, Beam_Lock, Beam_Load
	
	/* Master Beam Information */
	return "";
}
/* ------------------------------------------------------------------------ */

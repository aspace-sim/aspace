/* space_output.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *output_shields_raw ()
{
	int x;
	static char buffer[BUFFER_LEN];
	
	x = n;
	
	/* Allocate 6, 7, 8, 9 and 10 */
	/* Shield 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 and 12 */
	
	/* Master Shield Information */
	snprintf(buffer, sizeof(buffer), "%d|%f|%f|%d|%3.3f|%d", x, sdb[x].alloc.shields, sdb[x].shield.ratio, sdb[x].shield.maximum, sdb[x].shield.freq, sdb[x].shield.exist);
	strncat(buffer, "~", sizeof(buffer) - 1);
	
	/* Fore Shield */
	
	strncat(buffer, tprintf("%s|%f|%d|%f", "Fore Shield", sdb[x].alloc.shield[0], sdb[x].shield.active[0], sdb[x].shield.damage[0]), sizeof(buffer) - 1);
	strncat(buffer, "~", sizeof(buffer) - 1);

	/* Starboard Shield */
	
	strncat(buffer, tprintf("%s|%f|%d|%f", "Starboard Shield", sdb[x].alloc.shield[1], sdb[x].shield.active[1], sdb[x].shield.damage[1]), sizeof(buffer) - 1);
	strncat(buffer, "~", sizeof(buffer) - 1);
	
	/* Aft Shield */
	
	strncat(buffer, tprintf("%s|%f|%d|%f", "Aft Shield", sdb[x].alloc.shield[2], sdb[x].shield.active[2], sdb[x].shield.damage[2]), sizeof(buffer) - 1);
	strncat(buffer, "~", sizeof(buffer) - 1);
	
	/* Port Shield */
	
	strncat(buffer, tprintf("%s|%f|%d|%f", "Port Shield", sdb[x].alloc.shield[3], sdb[x].shield.active[3], sdb[x].shield.damage[3]), sizeof(buffer) - 1);
	
	return buffer;
}

/* ------------------------------------------------------------------------ */

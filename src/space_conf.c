/* space_conf.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

SPACETAB configstruct;

struct spaceconfig get_space_config(char *filename) {
	struct spaceconfig configstruct;
	FILE *file = fopen(filename, "r");
	
	if ( file != NULL ) {
		char line[MAXBUF];
		int i = 0;

		while(fgets(line, sizeof(line), file) != NULL) {
			char *cfline;
			cfline = strstr((char *) line, DELIM);
			cfline = cfline + strlen(DELIM);

			if ( i == 0 ) {
				configstruct.cochrane_rate = parse_number(cfline);
			}

			i++;
		}

		fclose(file);
	}

	return configstruct;
}

void loadDefaultConfig() {
	configstruct.cochrane_rate = 12927.238000;
}

void loadSpaceConfig() {
	//loadDefaultConfig();
	configstruct = get_space_config(FILENAME);
}

/* ------------------------------------------------------------------------ */
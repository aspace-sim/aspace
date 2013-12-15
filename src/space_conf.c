/* space_conf.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

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
				memcpy(configstruct.mainReactor, cfline, strlen(cfline));
			}

			i++;
		}

		fclose(file);
	}

	return configstruct;
}

void loadSpaceConfig() {
	configstruct = get_space_config(FILENAME);
}

/* ------------------------------------------------------------------------ */
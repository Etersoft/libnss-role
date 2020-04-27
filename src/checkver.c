#include <stdio.h>
#include <string.h>

#include "role/version.h"

int main(int argc, char** argv) {
	printf("%s\n", LIBNSS_ROLE_VERSION);

	if (argc == 2) {
		if (0 != strcmp(LIBNSS_ROLE_VERSION, argv[1])) {
			return 1;
		}
	}

	return 0;
}


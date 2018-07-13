#include <grp.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "role/parser.h"
#include "role/version.h"

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'v'}
};

static int parse_options(int argc, char **argv)
{
	int c, opt_ind;
	while((c = getopt_long(argc, argv, "hv", rolelst_opt, &opt_ind)) != -1) {
		switch(c) {
			case 'h':
				fprintf(stderr, "Usage: rolelst\n  -h [ --help ]\t\t produce help message\n\n");
				return 0;
			case 'v':
				printf("rolelst is the utility for libnss_role version %s\n",
				       LIBNSS_ROLE_VERSION);
				return 0;
			case '?':
				return 0;
			default:
				return 0;
		}
	}
	return 1;
}

static int get_group_name(gid_t g, char *ans)
{
	struct group grp, *grp_ptr;
	char buffer[1000];
	if (getgrgid_r(g, &grp, buffer, 1000, &grp_ptr) == 0) {
		if (errno == ERANGE)
			return LIBROLE_OUT_OF_RANGE;
		if (errno != 0)
			return LIBROLE_UNKNOWN_ERROR;
	}
	if (!grp_ptr)
		return LIBROLE_NO_SUCH_GROUP;
	strncpy(ans, grp.gr_name, 999);
	ans[999] = '\0';
	return LIBROLE_OK;
}

int main(int argc, char **argv) {
	struct librole_graph G = {0,0,0,10};
	int result, i;
	char gr_name[1000];
	if (!parse_options(argc, argv))
		goto exit;
	result = librole_graph_init(&G);
	if (result != LIBROLE_OK) {
		fprintf(stderr, "Memory error\n");
		goto exit;
	}
	result = librole_reading("/etc/role", &G);
	if (result != LIBROLE_OK) {
		if (result == LIBROLE_IO_ERROR)
			fprintf(stderr, "IO error\n");
		else if (result == LIBROLE_MEMORY_ERROR ||
			result == LIBROLE_OUT_OF_RANGE)
			fprintf(stderr, "Memory error\n");
		goto exit;
	}
	for(i = 0; i < G.size; i++) {
		int j;
		if (get_group_name(G.gr[i].gid, gr_name) == LIBROLE_OK)
			printf("%s:", gr_name);
		else
			goto exit;
		for(j = 0; j < G.gr[i].size; j++)
			if (get_group_name(G.gr[i].list[j], gr_name)
							== LIBROLE_OK)
				printf(" %s", gr_name);
			else
				goto exit;
		printf("\n");
	}

exit:
	librole_free_all(&G);
	return result;
}



#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "role/parser.h"
#include "role/version.h"

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'},
	{"number", no_argument, 0, 'n'},
	{"version", no_argument, 0, 'v'}
};

static int parse_options(int argc, char **argv, int *numeric_flag)
{
	int c, opt_ind;
	*numeric_flag = 0;
	while((c = getopt_long(argc, argv, "hnv", rolelst_opt, &opt_ind)) != -1) {
		switch(c) {
			case 'h':
				fprintf(stderr, "Usage: rolelst\n  -h [ --help ]\t\t produce help message\n\n");
				return 0;
			case 'n':
				*numeric_flag = 1;
				break;
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

int print_group(int gid)
{
	int result;
	char gr_name[LIBROLE_MAX_NAME];

	result = librole_get_group_name(gid, gr_name, LIBROLE_MAX_NAME);
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		return result;
	}
	printf("%s", gr_name);
	return LIBROLE_OK;
}

int main(int argc, char **argv) {
	struct librole_graph G = {0,0,0,10};
	int numeric_flag;
	int result = LIBROLE_OK;
	int i;
	
	if (!parse_options(argc, argv, &numeric_flag))
		goto exit;
	
	result = librole_graph_init(&G);
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		goto exit;
	}
	result = librole_reading("/etc/role", &G);
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		goto exit;
	}
	
	for(i = 0; i < G.size; i++) {
		int j;
		if (numeric_flag) {
			printf("%u:", G.gr[i].gid);
		} else {
			if (print_group(G.gr[i].gid) != LIBROLE_OK)
				goto exit;
			putchar(':');
		}
		
		for(j = 0; j < G.gr[i].size; j++) {
			if (numeric_flag) {
				printf(" %u", G.gr[i].list[j]);
			} else {
				putchar(' ');
				if (print_group(G.gr[i].list[j]) != LIBROLE_OK)
					goto exit;
			}
		}
		printf("\n");
	}

exit:
	librole_free_all(&G);
	return result;
}


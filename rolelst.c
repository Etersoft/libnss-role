#include <Role/parser.h>
#include <grp.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'}
};

static int parse_options(int argc, char **argv)
{
	int c, opt_ind;
	while((c = getopt_long(argc, argv, "h", rolelst_opt, &opt_ind)) != -1) {
		switch(c) {
			case 'h':
				fprintf(stderr, "Usage: rolelst\n  -h [ --help ]\t\t produce help message\n\n");
				return 0;
			case '?':
				return 0;
			default:
				return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv) {
	struct librole_graph G = {0,0,0,10};
	int result, i;
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
		printf("%u:", G.gr[i].gid);
		for(j = 0; j < G.gr[i].size; j++)
			printf(" %u", G.gr[i].list[j]);
		printf("\n");
	}

exit:
	librole_free_all(&G);
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "role/parser.h"
#include "role/version.h"

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'},
	{"remove", no_argument, 0, 'r'},
	{"skip_errors", no_argument, 0, 'm'},
	{"version", no_argument, 0, 'v'}
};

static void print_help(void)
{
	fprintf(stdout, "Usage: roledel [-hrmv] ROLE [*PRIVS]\n");
	fprintf(stdout,
		"\t-h [ --help ]\t\tproduce help message\n");
	fprintf(stdout,
		"\t-r [ --remove ]\t\tremove all privileges from role\n");
	fprintf(stdout,
		"\t-m [ --skip-errors ]\tskip missed privileges\n");
	fprintf(stdout,
		"\t-v [ --version]\t\tprint roledel version being used\n");
	fprintf(stdout, "\n");
}

static int parse_options(int argc, char **argv, int *remove_flag, int *skip_flag)
{
	int c, opt_ind;
	*remove_flag = 0;
	*skip_flag = 0;
	while((c = getopt_long(argc, argv, "hmrv", rolelst_opt, &opt_ind)) != -1) {
		switch(c) {
			case 'h':
				print_help();
				return 0;
			case 'm':
				*skip_flag = 1;
				break;
			case 'r':
				*remove_flag = 1;
				break;
			case 'v':
				printf("roledel is the utility for libnss_role version %s\n",
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

int main(int argc, char **argv) {
	struct librole_graph G;
	int result, i, remove_flag, skip_flag;
	struct librole_ver del_role;

	if (!parse_options(argc, argv, &remove_flag, &skip_flag))
		return 0;

	if (optind >= argc) {
		print_help();
		return 1;
	}

	result = librole_graph_init(&G);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_reading("/etc/role", &G);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_ver_init(&del_role);
	if (result != LIBROLE_OK)
		goto exit;

	/* get role */
	result = librole_get_gid(argv[optind++], &del_role.gid);
	if (result != LIBROLE_OK)
		goto exit;

	/* get privs from args */
	while(optind < argc) {
		gid_t tmp_gr;
		result = librole_get_gid(argv[optind++], &tmp_gr);
		if (result != LIBROLE_OK && !skip_flag) {
			librole_ver_free(&del_role);
			fprintf(stderr,"No such group: %s!\n", argv[optind-1]);
			goto exit;
		} else if (result != LIBROLE_OK)
			continue;
		result = librole_ver_add(&del_role, tmp_gr);
		if (result != LIBROLE_OK) {
			librole_ver_free(&del_role);
			goto exit;
		}
	}

	result = librole_find_gid(&G, del_role.gid, &i);
	if (result == LIBROLE_OK) {
		if (!remove_flag) {
			struct librole_ver new_role;
			result = librole_ver_init(&new_role);
			if (result != LIBROLE_OK) {
				librole_ver_free(&new_role);
				librole_ver_free(&del_role);
				goto exit;
			}
			new_role.gid = G.gr[i].gid;

			int j;
			for(j = 0; j < G.gr[i].size; j++) {
				result = librole_ver_find_gid(&del_role, G.gr[i].list[j], NULL);
				if (result == LIBROLE_OK)
					continue; 
				result = librole_ver_add(&new_role, G.gr[i].list[j]);
				if (result != LIBROLE_OK) {
					librole_ver_free(&new_role);
					librole_ver_free(&del_role);
					goto exit;
				}
			}
			librole_ver_free(&G.gr[i]);
			G.gr[i] = new_role;
		} else
			librole_ver_free(&G.gr[i]);
	}
	librole_ver_free(&del_role);

	result = librole_write("roledel", &G);

exit:
	librole_print_error(result);
	librole_graph_free(&G);
	return result;
}

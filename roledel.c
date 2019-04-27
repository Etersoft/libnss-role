#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <grp.h>

#include "role/parser.h"
#include "role/version.h"
#include "role/pam_check.h"
#include "role/lock_file.h"

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
	int result, i, remove_flag, skip_flag, pam_status;
	pam_handle_t *pamh;

	if (!parse_options(argc, argv, &remove_flag, &skip_flag))
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

	result = librole_ver_init(&del_role);
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		goto exit;
	}

	if (optind < argc) {
		result = librole_get_gid(argv[optind++], &del_role.gid);
		if (result != LIBROLE_OK) {
			librole_print_error(result);
			goto exit;
		}
		while(optind < argc) {
			gid_t tmp_gr;
			result = librole_get_gid(argv[optind++], &tmp_gr);
			if (result != LIBROLE_OK && !skip_flag) {
				free(del_role.list);
				librole_print_error(result);
				fprintf(stdout,"No such group: %s!\n", argv[optind-1]);
				goto exit;
			} else if (result != LIBROLE_OK)
				continue;
			result = librole_ver_add(&del_role, tmp_gr);
			if (result != LIBROLE_OK) {
				free(del_role.list);
				librole_print_error(result);
				goto exit;
			}
		}
	} else {
		print_help();
		free(del_role.list);
		goto exit;
	}

	result = librole_find_id(&G, del_role.gid, &i);
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
				int todel = 0, k;
				for(k = 0; k < del_role.size; k++)
					if (G.gr[i].list[j] == del_role.list[k]) {
						todel = 1;
						break;
					}
				if (todel)
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
			G.gr[i].size = 0;
	}
	free(del_role.list);

	result = librole_pam_check(pamh, "roledel", &pam_status);
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		fprintf(stdout,"Only root can do it\n");
		goto exit;
	}

	result = librole_lock("/etc/role");
	if (result != LIBROLE_OK) {
		librole_print_error(result);
		librole_pam_release(pamh, pam_status);
		goto exit;
	}

	result = librole_writing("/etc/role", &G);
	librole_print_error(result);

	librole_unlock("/etc/role");

	librole_pam_release(pamh, pam_status);

exit:
	librole_graph_free(&G);
	return result;
}


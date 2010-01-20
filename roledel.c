#include <Role/parser.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <Role/pam_check.h>

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'},
	{"remove", no_argument, 0, 'r'},
	{"skip_errors", no_argument, 0, 'm'}
};

static void print_help(void)
{
	fprintf(stderr, "Usage: roledel [-hrm] ROLE [*PRIVS]\n");
	fprintf(stderr,
		"\t-h [ --help ]\t\tproduce help message\n");
	fprintf(stderr,
		"\t-r [ --remove ]\t\tremove role with privilegies\n");
	fprintf(stderr,
		"\t-m [ --skip-errors ]\tignore errors\n");
	fprintf(stderr, "\n");
}

static int parse_options(int argc, char **argv, int *remove_flag, int *skip_flag)
{
	int c, opt_ind;
	*remove_flag = 0;
	*skip_flag = 0;
	while((c = getopt_long(argc, argv, "rhm", rolelst_opt, &opt_ind)) != -1) {
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
	int result, i, remove_flag, skip_flag, pam_status;
	struct librole_ver del_role = {0,0,0,10};
	pam_handle_t *pamh;

	if (!parse_options(argc, argv, &remove_flag, &skip_flag))
		goto exit;

	result = librole_graph_init(&G);
	if (result != LIBROLE_OK) {
		fprintf(stderr,"Memory error!\n");
		goto exit;
	}

	result = librole_reading("/etc/role", &G);
	if (result != LIBROLE_OK) {
		if (result = LIBROLE_IO_ERROR)
			fprintf(stderr,"IO error!\n");
		else if (result == LIBROLE_MEMORY_ERROR ||
				result == LIBROLE_OUT_OF_RANGE)
			fprintf(stderr,"Memory error!\n");
		goto exit;
	}

	result = librole_ver_init(&del_role);
	if (result != LIBROLE_OK) {
		fprintf(stderr,"Memory error!\n");
		goto exit;
	}

	if (optind < argc) {
		result = librole_get_gid(argv[optind++], &del_role.gid);
		if (result != LIBROLE_OK) {
			if (result != LIBROLE_NO_SUCH_GROUP)
				fprintf(stderr,"Memory error!\n");
			goto exit;
		}
		while(optind < argc) {
			gid_t tmp_gr;
			result = librole_get_gid(argv[optind++], &tmp_gr);
			if (result != LIBROLE_OK && !skip_flag) {
				free(del_role.list);
				fprintf(stderr,"No such group: %s!\n", argv[optind-1]);
				goto exit;
			} else if (result != LIBROLE_OK)
				continue;
			result = librole_ver_add(&del_role, tmp_gr);
			if (result != LIBROLE_OK) {
				free(del_role.list);
				fprintf(stderr,"Memory error!\n");
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
			struct librole_ver new_role = {0, 0, 0, 10};
			result = librole_ver_init(&new_role);
			if (result != LIBROLE_OK) {
				free(new_role.list);
				free(del_role.list);
				fprintf(stderr,"Memory error!\n");
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
					free(new_role.list);
					free(del_role.list);
					fprintf(stderr,"Memory error!\n");
					goto exit;
				}
			}
			free(G.gr[i].list);
			G.gr[i] = new_role;
		} else
			G.gr[i].size = 0;
	}
	free(del_role.list);

	result = librole_pam_check(pamh, "roledel", &pam_status);
	if (result != LIBROLE_OK) {
		fprintf(stderr,"Only root can do it\n");
		goto exit;
	}

	result = librole_lock("/etc/role");
	if (result != LIBROLE_OK) {
		librole_pam_release(pamh, pam_status);
		goto exit;
	}

	result = librole_writing("/etc/role", &G);

	librole_unlock("/etc/role");

	librole_pam_release(pamh, pam_status);

exit:
	librole_free_all(&G);
	return 0;
}


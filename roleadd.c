#include <Role/parser.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <security/pam_appl.h>

struct option rolelst_opt[] = {
	{"help", no_argument, 0, 'h'},
	{"set", no_argument, 0, 's'},
	{"skip_errors", no_argument, 0, 'm'}
};

static void print_help(void)
{
	fprintf(stderr, "Usage: roledel [-hrm] ROLE [*PRIVS]\n");
	fprintf(stderr,
		"\t-h [ --help ]\t\tproduce help message\n");
	fprintf(stderr,
		"\t-s [ --set ]\t\tset role with privilegies\n");
	fprintf(stderr,
		"\t-m [ --skip-errors ]\tignore errors\n");
	fprintf(stderr, "\n");
}

static int parse_options(int argc, char **argv, int *set_flag, int *skip_flag)
{
	int c, opt_ind;
	*set_flag = 0;
	*skip_flag = 0;
	while((c = getopt_long(argc, argv, "shm", rolelst_opt, &opt_ind)) != -1) {
		switch(c) {
			case 'h':
				print_help();
				return 0;
			case 'm':
				*skip_flag = 1;
				break;
			case 's':
				*set_flag = 1;
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
	struct librole_graph G = {0, 0, 0, 10};
	int result, i, set_flag, skip_flag, pam_status;
	struct librole_ver new_role = {0, 0, 0, 10};
	pam_handle_t *pamh;

	if (!parse_options(argc, argv, &set_flag, &skip_flag))
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

	result = librole_ver_init(&new_role);
	if (result != LIBROLE_OK) {
		fprintf(stderr,"Memory error!\n");
		goto exit;
	}

	if (optind < argc) {
		result = librole_get_gid(argv[optind++], &new_role.gid);
		if (result != LIBROLE_OK) {
			if (result != LIBROLE_NO_SUCH_GROUP)
				fprintf(stderr,"Memory error!\n");
			goto exit;
		}
		while(optind < argc) {
			gid_t tmp_gr;
			result = librole_get_gid(argv[optind++], &tmp_gr);
			if (result != LIBROLE_OK && !skip_flag) {
				free(new_role.list);
				fprintf(stderr,"No such group: %s!\n", argv[optind-1]);
				goto exit;
			} else if (result != LIBROLE_OK)
				continue;
			result = librole_ver_add(&new_role, tmp_gr);
			if (result != LIBROLE_OK) {
				free(new_role.list);
				fprintf(stderr,"Memory error!\n");
				goto exit;
			}
		}
	} else {
		print_help();
		free(new_role.list);
		goto exit;
	}

	result = librole_find_id(&G, new_role.gid, &i);
	if (result == LIBROLE_OK) {
		if (!set_flag) {
			int j;
			for(j = 0; j < new_role.size; j++) {
				int k, exist_in_role = 0;
				for(k = 0; k < G.gr[i].size; k++)
					if (G.gr[i].list[k] == new_role.list[j]) {
						exist_in_role = 1;
						break;
					}
				if (exist_in_role)
					continue;
				result = librole_ver_add(&G.gr[i], new_role.list[j]);
				if (result != LIBROLE_OK) {
					free(new_role.list);
					fprintf(stderr,"Memory error!\n");
					goto exit;
				}
			}
			free(new_role.list);
		} else {
			free(G.gr[i].list);
			G.gr[i] = new_role;
		}
	} else {
		result = librole_graph_add(&G, new_role);
		if (result != LIBROLE_OK) {
			free(new_role.list);
			fprintf(stderr,"Memory error!\n");
			goto exit;
		}
	}
	result = librole_pam_check(pamh, "roleadd", &pam_status);
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


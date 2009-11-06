#include <Role/parser.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

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
	struct graph G = {0, 0, 0, 10};
	int result, i, set_flag, skip_flag;
	struct ver new_role = {0, 0, 0, 10};

	if (!parse_options(argc, argv, &set_flag, &skip_flag))
		goto exit;

	result = graph_init(&G);
	if (result != OK)
		goto exit;

	result = reading("/etc/role", &G);
	if (result != OK)
		goto exit;

	result = ver_init(&new_role);
	if (result != OK)
		goto exit;

	if (optind < argc) {
		result = get_gid(argv[optind++], &new_role.gid);
		if (result != OK)
			goto exit;
		while(optind < argc) {
			gid_t tmp_gr;
			result = get_gid(argv[optind++], &tmp_gr);
			if (result != OK && !skip_flag) {
				free(new_role.list);
				goto exit;
			} else if (result != OK)
				continue;
			result = ver_add(&new_role, tmp_gr);
			if (result != OK) {
				free(new_role.list);
				goto exit;
			}
		}
	} else {
		print_help();
		goto exit;
	}

	result = find_id(&G, new_role.gid, &i);
	if (result == OK) {
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
				result = ver_add(&G.gr[i], new_role.list[j]);
				if (result != OK) {
					free(new_role.list);
					goto exit;
				}
			}
		} else {
			free(G.gr[i].list);
			G.gr[i] = new_role;
		}
	} else {
		result = graph_add(&G, new_role);
		if (result != OK) {
			free(new_role.list);
			goto exit;
		}
	}

	result = writing("/etc/role", &G);

exit:
	free_all(&G);
	return 0;
}


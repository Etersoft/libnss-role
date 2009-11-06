#include <Role/parser.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

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
	struct graph G = {0,0,0,10};
	int result, i, remove_flag, skip_flag;
	struct ver del_role = {0,0,0,10};

	if (!parse_options(argc, argv, &remove_flag, &skip_flag))
		goto exit;

	result = graph_init(&G);
	if (result != OK)
		goto exit;

	result = reading("/etc/role", &G);
	if (result != OK)
		goto exit;

	result = ver_init(&del_role);
	if (result != OK)
		goto exit;

	if (optind < argc) {
		result = get_gid(argv[optind++], &del_role.gid);
		if (result != OK)
			goto exit;
		while(optind < argc) {
			gid_t tmp_gr;
			result = get_gid(argv[optind++], &tmp_gr);
			if (result != OK && !skip_flag) {
				free(del_role.list);
				goto exit;
			} else if (result != OK)
				continue;
			result = ver_add(&del_role, tmp_gr);
			if (result != OK) {
				free(del_role.list);
				goto exit;
			}
		}
	} else {
		print_help();
		free(del_role.list);
		goto exit;
	}

	result = find_id(&G, del_role.gid, &i);
	if (result == OK) {
		if (!remove_flag) {
			struct ver new_role = {0, 0, 0, 10};
			result = ver_init(&new_role);
			if (result != OK) {
				free(new_role.list);
				free(del_role.list);
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
				result = ver_add(&new_role, G.gr[i].list[j]);
				if (result != OK) {
					free(new_role.list);
					free(del_role.list);
					goto exit;
				}
			}
			free(G.gr[i].list);
			G.gr[i] = new_role;
		} else
			G.gr[i].size = 0;
	}
	free(del_role.list);

	result = librole_lock("/etc/role");
	if (result != OK)
		goto exit;
	result = writing("/etc/role", &G);
	librole_unlock("/etc/role");

exit:
	free_all(&G);
	return 0;
}


#include <nss.h>
#include <grp.h>

#include <pthread.h>

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

static int STR_max_size = 10000;
static int STR_min_size = 100;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ver {
	gid_t v;
	gid_t *list;
	int size;
	int capability;
};

struct graph {
	struct ver *gr;
	int size;
	int capability;
};

struct string {
	char s[20];
	int id;
};

static void free_all(struct graph *G)
{
	int i;
	for(i = 0; i < G->size; i++) {
		int j;
		for(j = 0; j < G->gr[i].size; j++)
			free(G->gr[i].list);
		free(&G->gr[i]);
	}
}

static int realloc_groups (long int **size, gid_t ***groups, long int limit)
{
	long int new_size;
	gid_t *new_groups;

	new_size = 2 * (**size);
	if (limit > 0) {
		if (**size == limit)
			return 0;

		if (new_size > limit)
			new_size = limit;
	}

	new_groups = (gid_t *)
		realloc((**groups),
			new_size * sizeof(***groups));
	if (!new_groups)
		return 0;

	**groups = new_groups;
	**size = new_size;

	return 1;
}

static void parse_line(char *s, struct graph *G)
{
//	printf("%s\n", s);
}

static void reading(char *s, struct graph *G)
{
	FILE *f = fopen(s, "r");
	unsigned long len = STR_min_size;
	char *str = malloc(len * sizeof(char));
	unsigned long id = 0;
	char c;
	printf("in reading\n");
	while(1) {
		c = fgetc(f);
		if (c == EOF)
			break;
		if (c == '\n') {
			str[id] = '\0';
			parse_line(str, G);
			id = 0;
			continue;
		}
		str[id++] = c;
		if (id == len) {
			len *= 2;
			str = realloc(str, len * sizeof(char));
		}
	}
	if (id) {
		str[id] = '\0';
		parse_line(str, G);
	}
}

static void dfs(gid_t v)
{

}

static gid_t get_gid(char *role_name)
{
	if (isalpha(role_name[0])) {
		struct group grp, *grp_ptr;
		char buffer[1000];
		if (getgrnam_r(role_name, &grp, buffer, 1000, &grp_ptr) == 0) {
			if (errno == ERANGE)
				return -abs(ERANGE);
			if (errno != 0)
				return -1;
		}
		if (!grp_ptr)
			return -2;
		return grp.gr_gid;
	}
	return atoi(role_name);
}


enum nss_status _nss_role_initgroups_dyn (char *user, gid_t main_group, long int *start, long int *size, gid_t **groups, long int limit, int *errnop)
{
	enum nss_status ret = NSS_STATUS_SUCCESS;
	pthread_mutex_lock(&mutex);

	struct graph G = {0, 0, 0};
	int i;
	reading("/etc/role", &G);

	dfs(main_group);
	for(i = 0; i < *start; i++)
		dfs((*groups)[i]);

out:
	free_all(&G);
	pthread_mutex_unlock(&mutex);
	return ret;
}


int main(void)
{
	struct graph G;
	reading("/etc/role", &G);
	return 0;
}

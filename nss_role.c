#include <nss.h>
#include <grp.h>

#include <pthread.h>

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>



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

void free_all(struct graph *G)
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

void reading(char *s, struct graph *G) {

}

void dfs(gid_t v) {

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

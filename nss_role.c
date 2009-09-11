#include <nss.h>
#include <grp.h>

#include <pthread.h>

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const int STR_min_size  = 100;

static const int NO_SUCH_GROUP = -100;
static const int OUT_OF_RANGE  = -101;
static const int MEMORY_ERROR  = -102;
static const int UNKNOWN_ERROR = -103;
static const int OK            = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ver {
	gid_t gid;
	gid_t *list;
	int size;
	int capacity;
};

typedef struct ver group_collector;

struct graph {
	struct ver *gr;
	int *used;
	int size;
	int capacity;
};

static gid_t get_gid(char *, gid_t *);

static int graph_add(struct graph *G, struct ver v)
{
	G->gr[G->size++] = v;
	if (G->size == G->capacity) {
		G->capacity *= 2;
		G->gr = (struct ver *) realloc(G->gr, sizeof(struct ver) * G->capacity);
		if (!G->gr)
			return MEMORY_ERROR;
	}
	return OK;
}

static int ver_add(struct ver *v, gid_t g)
{
	v->list[v->size++] = g;
	if (v->size == v->capacity) {
		v->capacity *= 2;
		v->list = (gid_t *) realloc(v->list, sizeof(gid_t) * v->capacity);
		if (!v->list)
			return MEMORY_ERROR;
	}
	return OK;
}

struct group_name {
	char name[32];
	int id;
};

static int find_id(struct graph *G, gid_t g, int *id)
{
	int i;
	for(i = 0; i < G->size; i++)
		if (G->gr[i].gid == g) {
			*id = i;
			return OK;
		}

	return NO_SUCH_GROUP;
}

static void free_all(struct graph *G)
{
	int i;
	for(i = 0; i < G->size; i++) {
		free(G->gr[i].list);
	}
	free(G->gr);
	free(G->used);
}

static int realloc_groups(long int **size, gid_t ***groups, long int limit)
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
		return MEMORY_ERROR;

	**groups = new_groups;
	**size = new_size;

	return OK;
}

static int parse_line(char *s, struct graph *G)
{
	int result;
	unsigned long len = strlen(s);
	int i;
	struct group_name role_name = {{0}, 0};
	struct ver role = {0, 0, 0, 10};
	role.list = (gid_t *) malloc(sizeof(gid_t) * role.capacity);

	if (!role.list)
		return MEMORY_ERROR;

	for(i = 0; i < len; i++) {
		if (s[i] == ':') {
			i++;
			break;
		}
		role_name.name[role_name.id++] = s[i];
	}

	result = get_gid(role_name.name, &role.gid);
	if (result != OK)
		return result;

	while(1) {
		struct group_name gr_name = {{0}, 0};
		if (i >= len)
			break;
		gid_t gr;
		for(; i < len; i++) {
			if (s[i] == ',') {
				i++;
				break;
			}
			gr_name.name[gr_name.id++] = s[i];
		}

		result = get_gid(gr_name.name, &gr);
		if (result != OK && result != NO_SUCH_GROUP)
			return result;
		else if (result == NO_SUCH_GROUP)
			continue;

		result = ver_add(&role, gr);
		if (result != OK)
			return result;
	}

	result = graph_add(G, role);
	return result;
}

static int reading(char *s, struct graph *G)
{
	int result;
	FILE *f = fopen(s, "r");
	unsigned long len = STR_min_size;
	char *str = malloc(len * sizeof(char));
	unsigned long id = 0;
	char c;

	while(1) {
		c = fgetc(f);
		if (c == EOF)
			break;
		if (c == '\n') {
			str[id] = '\0';
			result = parse_line(str, G);
			if (result != OK)
				return result;
			id = 0;
			continue;
		}
		str[id++] = c;
		if (id == len) {
			len *= 2;
			str = realloc(str, len * sizeof(char));
			if (!str)
				return MEMORY_ERROR;
		}
	}
	if (id) {
		str[id] = '\0';
		result = parse_line(str, G);
		if (result != OK)
			return result;
	}
	return OK;
}

static int dfs(struct graph *G, gid_t v, group_collector *col)
{
	int i, j, result;

	result = find_id(G, v, &i);
	if (result != OK) {
		result = ver_add(col, v);
		return result;
	}

	//printf("after find %llu\n", v);
	if (G->used[i])
		return OK;
	result = ver_add(col, v);
	if (result != OK)
		return result;
	G->used[i] = 1;

	for(j = 0; j < G->gr[i].size; j++) {
	//	printf("    dfs: %d %llu \n", j, v);
		result = dfs(G, G->gr[i].list[j], col);
	//	printf("    result: %d %llu\n", result, v);
		if (result != OK && result != NO_SUCH_GROUP)
			return result;
	}
	return OK;
}

static gid_t get_gid(char *gr_name, gid_t *ans)
{
	if (isalpha(gr_name[0])) {
		struct group grp, *grp_ptr;
		char buffer[1000];
		if (getgrnam_r(gr_name, &grp, buffer, 1000, &grp_ptr) == 0) {
			if (errno == ERANGE)
				return OUT_OF_RANGE;
			if (errno != 0)
				return UNKNOWN_ERROR;
		}
		if (!grp_ptr)
			return NO_SUCH_GROUP;
		*ans = grp.gr_gid;
		return OK;
	}
	*ans = (gid_t) atoi(gr_name);
	return OK;
}

enum nss_status _nss_role_initgroups_dyn (char *user, gid_t main_group, long int *start, long int *size, gid_t **groups, long int limit, int *errnop)
{
	enum nss_status ret = NSS_STATUS_SUCCESS;
	pthread_mutex_lock(&mutex);

	struct graph G = {0, 0, 0, 10};
	int i;
	group_collector col = {0, 0, 0, 10};
	col.list = (int *) malloc(sizeof(int) * col.capacity);

	G.gr = (struct ver *) malloc(sizeof(struct ver) * G.capacity);
	reading("/etc/role", &G);
	G.used = (int *) malloc(sizeof(int) * G.capacity);
	memset(G.used, 0, sizeof(int) * G.capacity);

	dfs(&G, main_group, &col);
	for(i = 0; i < *start; i++)
		dfs(&G, (*groups)[i], &col);

out:
	free(col.list);
	free_all(&G);
	pthread_mutex_unlock(&mutex);
	return ret;
}


int main(void)
{
	struct graph G = {0, 0, 0, 10};
	group_collector col = {0, 0, 0, 10};
	col.list = (int *) malloc(sizeof(int) * col.capacity);
	int i;
	G.gr = malloc(sizeof(struct ver) * G.capacity);
	reading("/etc/role", &G);
	G.used = (int *) malloc(sizeof(int) * G.capacity);
	memset(G.used, 0, sizeof(int) * G.capacity);

	for(i = 0; i < G.size; i++) {
		int j;
		printf("%llu: ", G.gr[i].gid);
		for(j = 0; j < G.gr[i].size; j++)
			printf("%llu, ", G.gr[i].list[j]);
		printf("\n");
	}
	dfs(&G, 514, &col);
	for(i = 0; i < col.size; i++)
		printf("%llu ", col.list[i]);
	printf("\n");
	free(col.list);
	free_all(&G);

	return 0;
}

#include <grp.h>
#include <sys/types.h>

#define STR_min_size		100

#define NO_SUCH_GROUP		-100
#define OUT_OF_RANGE		-101
#define MEMORY_ERROR		-102
#define UNKNOWN_ERROR		-103
#define OK			0

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

struct group_name {
	char name[32];
	int id;
};

int get_gid(char *, gid_t *);

int graph_add(struct graph *, struct ver);

int ver_add(struct ver *, gid_t);

int graph_init(struct graph *);

int ver_init(struct ver *);

void free_all(struct graph *);

int realloc_groups(long int **, gid_t ***, long int);

int reading(char *, struct graph *);

int dfs(struct graph *, gid_t, group_collector *);

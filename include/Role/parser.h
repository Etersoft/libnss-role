#ifndef LIBROLE_PARSER_H
#define LIBROLE_PARSER_H

#include <grp.h>
#include <sys/types.h>
#include <Role/glob.h>

struct librole_ver {
	gid_t gid;
	gid_t *list;
	int size;
	int capacity;
};

typedef struct librole_ver librole_group_collector;

struct librole_graph {
	struct librole_ver *gr;
	int *used;
	int size;
	int capacity;
};

int librole_get_gid(char *, gid_t *);

int librole_graph_add(struct librole_graph *, struct librole_ver);

int librole_ver_add(struct librole_ver *, gid_t);

int librole_graph_init(struct librole_graph *);

int librole_ver_init(struct librole_ver *);

void librole_free_all(struct librole_graph *);

int librole_realloc_groups(long int **, gid_t ***, long int);

int librole_reading(const char *, struct librole_graph *);

int librole_writing(const char *, struct librole_graph *);

int librole_dfs(struct librole_graph *, gid_t, librole_group_collector *);

int librole_find_id(struct librole_graph *, gid_t, int *);

#endif

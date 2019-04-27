#ifndef LIBROLE_PARSER_H
#define LIBROLE_PARSER_H

#include <grp.h>
#include <sys/types.h>
#include <role/glob.h>

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

int librole_graph_add(struct librole_graph *, struct librole_ver);

int librole_ver_add(struct librole_ver *, gid_t);

int librole_graph_init(struct librole_graph *);

int librole_ver_init(struct librole_ver *);

void librole_graph_free(struct librole_graph *);
void librole_ver_free(struct librole_ver *);

int librole_realloc_groups(long int **, gid_t ***, long int);

int librole_reading(const char *, struct librole_graph *);

int librole_writing(const char *, struct librole_graph *);

int librole_dfs(struct librole_graph *, gid_t, librole_group_collector *);

int librole_find_id(struct librole_graph *, gid_t, int *);

/* get gid by group name */
int librole_get_gid(const char *gr_name, gid_t *ans);

/* get user name by uid */
int librole_get_user_name(uid_t uid, char *user_name, size_t user_name_size);

/* get group name by uid */
int librole_get_group_name(gid_t g, char *ans, size_t ans_size);

void librole_print_error(int result);

/* internal */
int librole_realloc_buffer(void **buffer, size_t *size);
#endif

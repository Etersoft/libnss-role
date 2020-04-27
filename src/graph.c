#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <grp.h>
#include <pwd.h>

#include <sys/types.h>

#include "role/parser.h"


int librole_role_add(struct librole_graph *G, struct librole_ver new_role)
{
    int result;
    int idx, j;

    result = librole_find_gid(G, new_role.gid, &idx);
    if (result == LIBROLE_NO_SUCH_GROUP) {
        result = librole_graph_add(G, new_role);
        return result;
    }
    if (result != LIBROLE_OK)
        return result;

    for (j = 0; j < new_role.size; j++) {
        result = librole_ver_find_gid(&G->gr[idx], new_role.list[j], NULL);
        /* skip already exists */
        if (result == LIBROLE_OK)
            continue;
        result = librole_ver_add(&G->gr[idx], new_role.list[j]);
        if (result != LIBROLE_OK) {
            return result;
        }
    }
    return result;
}

/* Note: it USES new_role, not just copy */
int librole_role_set(struct librole_graph *G, struct librole_ver new_role)
{
    int result;
    int idx;

    result = librole_find_gid(G, new_role.gid, &idx);
    if (result == LIBROLE_NO_SUCH_GROUP) {
        result = librole_graph_add(G, new_role);
        return result;
    }

    if (result != LIBROLE_OK)
        return result;

    librole_ver_free(&G->gr[idx]);
    G->gr[idx] = new_role;

    return LIBROLE_OK;
}


int librole_role_del(struct librole_graph *G, struct librole_ver del_role)
{
    int result;
    int idx, j;
    struct librole_ver new_role;

    result = librole_find_gid(G, del_role.gid, &idx);
    if (result == LIBROLE_NO_SUCH_GROUP)
        return result;

    if (result != LIBROLE_OK)
        return result;

    /* create new ver instead old */
    result = librole_ver_init(&new_role);
    if (result != LIBROLE_OK) {
        librole_ver_free(&new_role);
        return result;
    }
    new_role.gid = G->gr[idx].gid;

    for(j = 0; j < G->gr[idx].size; j++) {
        /* skip role to del */
        result = librole_ver_find_gid(&del_role, G->gr[idx].list[j], NULL);
        if (result == LIBROLE_OK)
            continue;

        result = librole_ver_add(&new_role, G->gr[idx].list[j]);
        if (result != LIBROLE_OK) {
            librole_ver_free(&new_role);
            return result;
        }
    }

    return librole_role_set(G, new_role);
}



int librole_role_drop(struct librole_graph *G, struct librole_ver del_role)
{
    int result;
    int idx;

    result = librole_find_gid(G, del_role.gid, &idx);
    if (result == LIBROLE_NO_SUCH_GROUP)
        return result;

    if (result != LIBROLE_OK)
        return result;

    librole_ver_free(&G->gr[idx]);
    return LIBROLE_OK;
}

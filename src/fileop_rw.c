/*
 * Copyright (c) 2021 BaseALT
 *
 * NSS library for roles and privileges.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA.
 */
#include <stddef.h>
#include <stdlib.h>
/* For dirent and scandir */
#include <dirent.h>
#include <stdio.h>
/* For errno */
#include <errno.h>
/* For pathconf */
#include <unistd.h>
/* For strlen */
#include <string.h>
/* For PATH_MAX */
#include <linux/limits.h>

#include "role/glob.h"
#include "role/fileop_rw.h"
#include "role/pam_check.h"
#include "role/lock_file.h"

/* delim: 0 - '', > 0 - ',' before, < 0 - ':' after */
static int write_group(FILE *f, int gid, int delim, int numeric_flag)
{
    /* print ',' before if needed */
    if (delim > 0 && fputc(',', f) < 0)
        return LIBROLE_IO_ERROR;

    if (numeric_flag) {
        if (fprintf(f, "%u", gid) < 0)
            return LIBROLE_IO_ERROR;
    } else {
        int result;
        char gr_name[LIBROLE_MAX_NAME];
        result = librole_get_group_name(gid, gr_name, LIBROLE_MAX_NAME);
        if (result != LIBROLE_OK)
            return result;
        if (fprintf(f, "%s", gr_name) < 0)
            return LIBROLE_IO_ERROR;
    }

    /* print ':' after if needed */
    if (delim < 0 && fputc(':', f) < 0)
        return LIBROLE_IO_ERROR;

    return LIBROLE_OK;
}


/* TODO: the same like in rolelst
 TODO: write in a new file and atomically rename */
int librole_writing(const char *file, struct librole_graph *G, int numeric_flag, int empty_flag, librole_roles_filter filter)
{
    int i, j, result;
    FILE *f = fopen(file, "w");
    if (!f)
        return LIBROLE_IO_ERROR;

    for(i = 0; i < G->size; i++) {
        if (filter) {
            char gr_name[LIBROLE_MAX_NAME];
            if (librole_get_group_name(G->gr[i].gid, gr_name, LIBROLE_MAX_NAME) != LIBROLE_OK)
                continue;
            if (!filter(gr_name))
                continue;
        } else if (!G->gr[i].size && !empty_flag)
            continue;

        result = write_group(f, G->gr[i].gid, -1, numeric_flag);
        if (result != LIBROLE_OK)
            goto libnss_role_writing_exit;

        for(j = 0; j < G->gr[i].size; j++) {
            result = write_group(f, G->gr[i].list[j], j, numeric_flag);
            if (result != LIBROLE_OK)
                goto libnss_role_writing_exit;
        }
        if (fputc('\n', f) < 0)
            goto libnss_role_writing_exit;
    }

    result = LIBROLE_OK;

libnss_role_writing_exit:
    fclose(f);
    return result;
}

int librole_write(const char* pam_role, struct librole_graph *G, int empty_flag)
{
    int result;
    int pam_status;
    pam_handle_t *pamh;

    result = librole_pam_check(pamh, pam_role, &pam_status);
    if (result != LIBROLE_OK) {
        goto exit;
    }

    result = librole_lock(LIBROLE_CONFIG);
    if (result != LIBROLE_OK) {
        goto exit;
    }

    result = librole_writing(LIBROLE_CONFIG, G, 0, empty_flag, NULL);

    librole_unlock(LIBROLE_CONFIG);

/* TODO: can we release immediately? */
exit:
    librole_pam_release(pamh, pam_status);
    return result;
}

int librole_write_dir(const char* filename, const char* pam_role, struct librole_graph *G, int empty_flag)
{
    int result = 0;
    int pam_status;
    size_t dirlen = strlen(LIBROLE_CONFIG_DIR);
    size_t namelen = strlen(filename);
    size_t fullpathlen = dirlen + namelen + 1 + 1;
    pam_handle_t *pamh = NULL;
    char *fullpath = NULL;

    result = librole_pam_check(pamh, pam_role, &pam_status);
    if (result != LIBROLE_OK)
        return result;


    if (fullpathlen > PATH_MAX)
    {
        result = ENAMETOOLONG;
        goto librole_write_dir_done;
    }
    fullpath = calloc(fullpathlen, sizeof(char));

    /* Build full path to the file being read for roles */
    strcpy(fullpath, LIBROLE_CONFIG_DIR);
    strcat(fullpath, "/");
    strcat(fullpath, filename);

    result = librole_lock(fullpath);
    if (result != LIBROLE_OK) {
        goto librole_write_dir_done;
    }

    result = librole_writing(fullpath, G, 0, empty_flag, NULL);

    librole_unlock(fullpath);

/* TODO: can we release immediately? */
librole_write_dir_done:
    librole_pam_release(pamh, pam_status);
    free(fullpath);
    fullpath = NULL;

    return result;
}

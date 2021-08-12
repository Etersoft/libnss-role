/*
 * Copyright (c) 2008-2020 Etersoft
 * Copyright (c) 2020 BaseALT
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
#include "role/parser.h"
#include "role/fileop.h"
#include "role/pam_check.h"
#include "role/lock_file.h"

/*
 * \brief Selector function for scandir which gets all regular files.
 *
 * \param[in] entry Entry to test.
 * \return
 *  - 0: Entry is not a regular file
 *  - 1: Entry is a regular file
 */
static int librole_is_role_file(const struct dirent *entry)
{
    const char *extension_pattern = LIBROLE_ROLE_EXTENSION;
    size_t extensionlen = strlen(extension_pattern);
    size_t namelen = strlen(entry->d_name);

    if (!entry)
    {
        goto librole_is_file_end;
    }

    if (DT_REG == entry->d_type)
    {
        if (!strcmp(extension_pattern, entry->d_name + namelen - extensionlen)) {
            return 1;
        }
    }

librole_is_file_end:
    return 0;
}

int librole_validate_filename_from_dir(const char *filename)
{
    int retcode = LIBROLE_INVALID_ROLE_FILENAME;
    const char *extension_pattern = LIBROLE_ROLE_EXTENSION;
    size_t extensionlen = strlen(extension_pattern);
    size_t namelen = strlen(filename);

    if (!filename)
    {
        return LIBROLE_INTERNAL_ERROR;
    }

    if (strcmp(extension_pattern, filename + namelen - extensionlen) == 0)
    {
        retcode = LIBROLE_OK;
    }

    return retcode;
}

/*
 * \brief Validate single system role file name from roles directory.
 *
 * \param[in] filename Validate File name in a roles directory.
 * \return
 *  - LIBROLE_OK: if filename is valid.
 *  - LIBROLE_INVALID_ROLE_FILENAME: if filename is invalid.
 */
int librole_validate_system_role_filename(const char *filename, char **rolename)
{
    int retcode = LIBROLE_INVALID_ROLE_FILENAME;
    const char *extension_pattern = LIBROLE_ROLE_EXTENSION;
    size_t extensionlen = strlen(extension_pattern);
    size_t namelen = strlen(filename);
    char *system_role = NULL;
    struct group *gr = NULL;
    struct gid_t *gid;
    struct librole_graph G;

    retcode = librole_validate_filename_from_dir(filename);
    if (retcode != LIBROLE_OK)
        return retcode;

    system_role = strdup(filename);
    if (system_role == NULL)
        return LIBROLE_MEMORY_ERROR;
    system_role[namelen - extensionlen] = 0;

    gr = getgrnam(system_role);
    if (!gr) {
        retcode = LIBROLE_NO_SUCH_GROUP;
        goto librole_validate_system_role_filename_from_dir_done;
    }
    gid = gr->gr_gid;

    retcode = librole_graph_init(&G);
    if (retcode != LIBROLE_OK)
        goto librole_validate_system_role_filename_from_dir_done;

    retcode = librole_read_file_from_dir(LIBROLE_CONFIG_DIR, filename, &G);
    if (retcode != LIBROLE_OK)
        goto librole_validate_system_role_filename_from_dir_done;

    retcode = librole_find_gid(&G, gid, NULL);
    librole_graph_free(&G);

librole_validate_system_role_filename_from_dir_done:
    if (retcode == LIBROLE_OK && rolename != NULL) {
        *rolename = system_role;
    } else {
        free(system_role);
    }

    return retcode;
}

int librole_read_file_from_dir(const char * const directory,
    const char * const filename,
    struct librole_graph *role_graph)
{
    int retcode = LIBROLE_OK;
    size_t dirlen = strlen(directory);
    size_t namelen = strlen(filename);
    size_t fullpathlen = dirlen + namelen + 1 + 1;
    char *fullpath = NULL;

    errno = 0;

    if (!directory || !filename || !role_graph)
    {
        retcode = LIBROLE_INCORRECT_VALUE;
        goto librole_read_file_from_dir_done;
    }

    if (fullpathlen > PATH_MAX)
    {
        retcode = ENAMETOOLONG;
        goto librole_read_file_from_dir_done;
    }
    fullpath = calloc(fullpathlen, sizeof(char));

    /* Build full path to the file being read for roles */
    strcpy(fullpath, directory);
    strcat(fullpath, "/");
    strcat(fullpath, filename);

    /* Ignore return code from librole_reading to retain previous
     * (without /etc/role.d) behavior. */
    retcode = librole_reading(fullpath, role_graph);

librole_read_file_from_dir_done:
    free(fullpath);
    fullpath = NULL;

    return retcode;
}

int librole_get_directory_files(const char * const directory,
    struct librole_graph *role_graph)
{
    int retcode = LIBROLE_OK;
    struct dirent **files;
    int file_count = 0;
    int i = 0;

    errno = 0;

    if (!directory || !role_graph)
    {
        retcode = LIBROLE_INCORRECT_VALUE;
        goto librole_get_directory_files_end;
    }


    /* Get all regular files in directory */
    file_count = scandir(directory, &files, librole_is_role_file, alphasort);
    if (0 != errno)
    {
        retcode = errno;
        goto librole_get_directory_files_end;
    }

    for (i = 0; i < file_count; ++i)
    {
        /* Validate reading filename and skip if name is not valid */
        if (librole_validate_filename_from_dir(files[i]->d_name) == LIBROLE_OK) {
            /* Don't do anything on errors and try to continue reading */
            retcode = librole_read_file_from_dir(
                directory, files[i]->d_name, role_graph);
        }
        free(files[i]);
    }
    free(files);

librole_get_directory_files_end:
    return retcode;
}

int librole_get_system_roles(const char * const directory,
    char *system_roles[])
{
    int retcode = LIBROLE_OK;
    struct dirent **files;
    int file_count = 0;
    int i, r;

    errno = 0;

    if (!directory || !system_roles)
    {
        retcode = LIBROLE_INCORRECT_VALUE;
        goto librole_get_directory_files_end;
    }


    /* Get all regular files in directory */
    file_count = scandir(directory, &files, librole_is_role_file, alphasort);
    if (0 != errno)
    {
        retcode = errno;
        goto librole_get_directory_files_end;
    }

    for (i = 0, r = 0; i < file_count; ++i)
    {
        char* rolename;
        /* Validate reading filename and skip if name is not valid */
        if (librole_validate_system_role_filename(files[i]->d_name, &rolename) == LIBROLE_OK) {
            if (r < LIBROLE_MAX_SYSTEM_ROLES) {
                system_roles[r++] = rolename;
            }
        }
        free(files[i]);
    }
    free(files);
    system_roles[r] = NULL;

librole_get_directory_files_end:
    return retcode;
}

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

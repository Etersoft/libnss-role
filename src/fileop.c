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

/*
 * \brief Selector function for scandir which gets all regular files.
 *
 * \param[in] entry Entry to test.
 * \return
 *  - 0: Entry is not a regular file
 *  - 1: Entry is a regular file
 */
static int librole_is_file(const struct dirent *entry)
{
    if (NULL == entry)
    {
        goto librole_is_file_end;
    }

    if (DT_REG == entry->d_type)
    {
        return 1;
    }

librole_is_file_end:
    return 0;
}

/*
 * \brief Validate single role file name from roles directory.
 *
 * \param[in] filename Validate File name in a roles directory.
 * \return
 *  - LIBROLE_OK: if filename is valid.
 *  - LIBROLE_INVALID_ROLE_FILENAME: if filename is invalid.
 */
int librole_validate_filename_from_dir(const char *filename)
{
    int retcode = LIBROLE_INVALID_ROLE_FILENAME;
    const char *extension_pattern = ".role";
    size_t extensionlen = strlen(extension_pattern);
    size_t namelen = strlen(filename);

    if (NULL == filename)
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
 * \brief Read single role file from specified directory right into
 * role graph node.
 *
 * \param[in] directory Directory to read file from.
 * \param[in] filename File name in a directory to read roles from.
 * \param[in,out] role_graph Role graph to expand with roles.
 * \return
 *  - LIBROLE_OK: Don't try to fail.
 */
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

    if (NULL == directory || NULL == filename || NULL == role_graph)
    {
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

/**
 * \brief Read role entries from /etc/role.d catalog.
 *
 * \param[in] directory directory to open.
 * \return
 *  - LIBROLE_INCORRECT_VALUE: Incorrect values passed to function
 *  - LIBROLE_ERROR_OPENING_DIRECTORY: Error occured while trying to
 *    open directory.
 *  - LIBROLE_OK: Operation succeeded.
 *  - errno: Undefined error.
 */
int librole_get_directory_files(const char * const directory,
    struct librole_graph *role_graph)
{
    int retcode = LIBROLE_OK;
    struct dirent **files;
    int file_count = 0;
    DIR *role_catalog = NULL;
    int i = 0;

    errno = 0;

    if (NULL == directory || NULL == role_graph)
    {
        retcode = LIBROLE_INCORRECT_VALUE;
        goto librole_get_directory_files_end;
    }


    /* Get all regular files in directory */
    file_count = scandir(directory, &files, librole_is_file, alphasort);
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

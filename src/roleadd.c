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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "role/parser.h"
#include "role/version.h"
#include "role/fileop.h"
#include "role/fileop_rw.h"
#include "role/paths.h"

struct option rolelst_opt[] = {
    {"help", no_argument, 0, 'h'},
    {"set", no_argument, 0, 's'},
    {"skip-missing-groups", no_argument, 0, 'm'},
    {"version", no_argument, 0, 'v'},
    {"system", no_argument, 0, 'S'},
    {"file", required_argument, 0, 'f'}
};

static void print_help(void)
{
    fprintf(stdout, "Usage: roleadd [-hsmv] ROLE [*PRIVS]\n");
    fprintf(stdout,
        "\t-h [ --help ]\t\tproduce help message\n");
    fprintf(stdout,
        "\t-s [ --set ]\t\tset role with that privileges only (override)\n");
    fprintf(stdout,
        "\t-m [ --skip-missing-groups ]\tskip missed privileges\n");
    fprintf(stdout,
        "\t-v [ --version]\t\tprint roleadd version being used\n");
    fprintf(stdout,
        "\t-f [ --file=file.role ]\tchange role in additional file /etc/role.d/file.role\n");
    fprintf(stdout,
        "\t-S [ --system ]\t\tchange system ROLE in additional file /etc/role.d/ROLE.role");
    fprintf(stdout, "\n");
}

static int parse_options(int argc, char **argv, int *set_flag, int *skip_flag, int *roled_flag, char **roled_file, int *system_role_flag)
{
    int c, opt_ind;
    *set_flag = 0;
    *skip_flag = 0;
    *roled_flag = 0;
    *system_role_flag = 0;
    while((c = getopt_long(argc, argv, "hsmvSf:", rolelst_opt, &opt_ind)) != -1) {
        switch(c) {
            case 'h':
                print_help();
                return 0;
            case 'm':
                *skip_flag = 1;
                break;
            case 's':
                *set_flag = 1;
                break;
            case 'f':
                *roled_flag = 1;
                *roled_file = strdup(optarg);
                break;
            case 'S':
                *system_role_flag = 1;
                break;
            case 'v':
                printf("roleadd is the utility for libnss_role version %s\n",
                       LIBNSS_ROLE_VERSION);
                return 0;
            case '?':
                return 0;
            default:
                return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    struct librole_graph G;
    int result, set_flag, skip_flag, roled_flag, system_role_flag;
    struct librole_ver new_role;
    char *filename = NULL;

    if (!parse_options(argc, argv, &set_flag, &skip_flag, &roled_flag, &filename, &system_role_flag))
        return 0;

    if (optind >= argc) {
        print_help();
        return 1;
    }

    if (roled_flag && system_role_flag) {
        print_help();
        return 1;
    }

    result = librole_graph_init(&G);
    if (result != LIBROLE_OK)
        goto exit;

    if (roled_flag) {
        result = librole_validate_filename_from_dir(filename);
        if (result != LIBROLE_OK)
            goto exit;

        librole_read_file_from_dir(librole_config_dir(), filename, &G);
    } else if (system_role_flag) {
        int filename_sz = strlen(argv[optind]) + strlen(LIBROLE_ROLE_EXTENSION) + 1;

        filename = malloc(filename_sz);
        if (filename == NULL)
            goto exit;

        strncpy(filename, argv[optind], filename_sz);
        strncat(filename, LIBROLE_ROLE_EXTENSION, strlen(LIBROLE_ROLE_EXTENSION));

        result = librole_validate_filename_from_dir(filename);
        if (result != LIBROLE_OK)
            goto exit;

        librole_read_file_from_dir(librole_config_dir(), filename, &G);
    } else {
        result = librole_reading(librole_config_file(), &G);
        if (result != LIBROLE_OK)
            goto exit;
    }

    result = librole_create_ver_from_args(argc, argv, optind, &new_role, skip_flag);
    if (result != LIBROLE_OK)
        goto exit;

    if (set_flag)
        result = librole_role_set(&G, new_role);
    else
        result = librole_role_add(&G, new_role);

    if (result == LIBROLE_OK && (roled_flag || system_role_flag))
        result = librole_write_dir(filename, "roleadd", &G, 1);
    else if (result == LIBROLE_OK)
        result = librole_write("roleadd", &G, 1);

exit:
    if (filename != NULL)
        free(filename);
    librole_print_error(result);
    librole_graph_free(&G);
    return result;
}


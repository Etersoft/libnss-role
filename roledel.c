#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "role/parser.h"
#include "role/version.h"

struct option rolelst_opt[] = {
    {"help", no_argument, 0, 'h'},
    {"remove", no_argument, 0, 'r'},
    {"skip_errors", no_argument, 0, 'm'},
    {"version", no_argument, 0, 'v'}
};

static void print_help(void)
{
    fprintf(stdout, "Usage: roledel [-hrmv] ROLE [*PRIVS]\n");
    fprintf(stdout,
        "\t-h [ --help ]\t\tproduce help message\n");
    fprintf(stdout,
        "\t-r [ --remove ]\t\tremove all privileges from role\n");
    fprintf(stdout,
        "\t-m [ --skip-errors ]\tskip missed privileges\n");
    fprintf(stdout,
        "\t-v [ --version]\t\tprint roledel version being used\n");
    fprintf(stdout, "\n");
}

static int parse_options(int argc, char **argv, int *remove_flag, int *skip_flag)
{
    int c, opt_ind;
    *remove_flag = 0;
    *skip_flag = 0;
    while((c = getopt_long(argc, argv, "hmrv", rolelst_opt, &opt_ind)) != -1) {
        switch(c) {
            case 'h':
                print_help();
                return 0;
            case 'm':
                *skip_flag = 1;
                break;
            case 'r':
                *remove_flag = 1;
                break;
            case 'v':
                printf("roledel is the utility for libnss_role version %s\n",
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
    int result, i, remove_flag, skip_flag;
    struct librole_ver del_role;

    if (!parse_options(argc, argv, &remove_flag, &skip_flag))
        return 0;

    if (optind >= argc) {
        print_help();
        return 1;
    }

    result = librole_graph_init(&G);
    if (result != LIBROLE_OK)
        goto exit;

    result = librole_reading(LIBROLE_CONFIG, &G);
    if (result != LIBROLE_OK)
        goto exit;

    result = librole_create_ver_from_args(argc, argv, optind, &del_role, skip_flag);
    if (result != LIBROLE_OK)
        goto exit;

    if (remove_flag)
        result = librole_role_drop(&G, del_role);
    else
        result = librole_role_del(&G, del_role);

    if (result == LIBROLE_OK)
        result = librole_write("roledel", &G);

exit:
    librole_print_error(result);
    librole_graph_free(&G);
    return result;
}


#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "role/parser.h"
#include "role/version.h"

struct option rolelst_opt[] = {
    {"help", no_argument, 0, 'h'},
    {"numeric", no_argument, 0, 'n'},
    {"version", no_argument, 0, 'v'}
};

static int parse_options(int argc, char **argv, int *numeric_flag)
{
    int c, opt_ind;
    *numeric_flag = 0;
    while((c = getopt_long(argc, argv, "hnv", rolelst_opt, &opt_ind)) != -1) {
        switch(c) {
            case 'h':
                fprintf(stdout, "Usage: rolelst [-hnv]\n");
                fprintf(stdout,
                    "\t-h [ --help   ]\t\tproduce help message\n");
                fprintf(stdout,
                    "\t-n [ --numeric]\t\tprint gid instead of group names\n");
                fprintf(stdout,
                    "\t-v [ --version]\t\tprint roledel version being used\n");
                fprintf(stdout, "\n");
                return 0;
            case 'n':
                *numeric_flag = 1;
                break;
            case 'v':
                printf("rolelst is the utility for libnss_role version %s\n",
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
    int numeric_flag;
    int result;

    if (!parse_options(argc, argv, &numeric_flag))
        return 0;

    result = librole_graph_init(&G);
    if (result != LIBROLE_OK)
        goto exit;

    result = librole_reading("/etc/role", &G);
    if (result != LIBROLE_OK)
        goto exit;

    result = librole_writing("/dev/stdout", &G, numeric_flag);
    if (result != LIBROLE_OK)
        goto exit;

exit:
    librole_print_error(result);
    librole_graph_free(&G);
    return result;
}


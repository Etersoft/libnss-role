
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "role/parser.h"
#include "role/version.h"


int main() {
	int result;
	struct librole_graph G;

	result = librole_graph_init(&G);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_reading("test/role.source", &G);
	if (result != LIBROLE_OK)
		goto exit;


	result = librole_writing("/dev/stdout", &G, 0);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_writing("/dev/stdout", &G, 1);
	if (result != LIBROLE_OK)
		goto exit;

/* TODO
{
	int k;
	struct librole_ver col;
	librole_ver_init(&col);
	librole_dfs(&G,4,&col);
	for (k = 0; 
}
*/

	result = librole_writing("/dev/stdout", &G, 0);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_writing("/dev/stdout", &G, 1);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_writing("test/role.test.new", &G, 0);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_writing("test/role.test.add", &G, 0);
	if (result != LIBROLE_OK)
		goto exit;

	result = librole_writing("test/role.test.del", &G, 0);
	if (result != LIBROLE_OK)
		goto exit;

exit:
	librole_print_error(result);
	librole_graph_free(&G);
	return result;
}


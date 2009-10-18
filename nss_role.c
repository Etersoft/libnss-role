#include <nss.h>
#include <grp.h>

#include <pthread.h>

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <Role/parser.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

enum nss_status _nss_role_initgroups_dyn(char *user, gid_t main_group,
		long int *start, long int *size, gid_t **groups,
		long int limit, int *errnop)
{
	enum nss_status ret = NSS_STATUS_SUCCESS;
	pthread_mutex_lock(&mutex);

	struct graph G = {0, 0, 0, 10};
	int i, result;
	group_collector col = {0, 0, 0, 10}, ans = {0, 0, 0, 10};

	result = graph_init(&G);
	if (result != OK) {
		*errnop = ENOMEM;
		ret = NSS_STATUS_NOTFOUND;
		goto libnss_role_out;
	}

	result = reading("/etc/role", &G);
	if (result != OK) {
		if (result == MEMORY_ERROR) {
			*errnop = ENOMEM;
			ret =  NSS_STATUS_NOTFOUND;
		} else
			ret = NSS_STATUS_UNAVAIL;
		goto libnss_role_out;
	}

	result = ver_init(&col);
	if (result != OK) {
		*errnop = ENOMEM;
		ret = NSS_STATUS_NOTFOUND;
		goto libnss_role_out;
	}

	result = dfs(&G, main_group, &col);
	if (result == MEMORY_ERROR) {
		*errnop = ENOMEM;
		ret = NSS_STATUS_NOTFOUND;
		goto libnss_role_out;
	}

	for(i = 0; i < *start; i++) {
		result = dfs(&G, (*groups)[i], &col);
		if (result == MEMORY_ERROR) {
			*errnop = ENOMEM;
			ret = NSS_STATUS_NOTFOUND;
			goto libnss_role_out;
		}
	}

	result = ver_init(&ans);
	if (result != OK) {
		*errnop = ENOMEM;
		ret = NSS_STATUS_NOTFOUND;
		goto libnss_role_out;
	}

	for(i = 0; i < col.size; i++) {
		int exist = 0, j;
		for(j = 0; j < *start; j++) {
			if ((*groups)[j] == col.list[i]) {
				exist = 1;
				break;
			}
		}
		if (main_group == col.list[i])
			exist = 1;
		for(j = 0; j < ans.size; j++) {
			if (ans.list[j] == col.list[i]) {
				exist = 1;
				break;
			}
		}

		if (exist)
			continue;

		result = ver_add(&ans, col.list[i]);
		if (result != OK) {
			*errnop = ENOMEM;
			ret = NSS_STATUS_NOTFOUND;
			goto libnss_role_out;
		}
	}

	if (*start + ans.size > *size) {
		if ((limit >= 0 && *start + ans.size > limit) ||
			realloc_groups(&size, &groups,
				*start + ans.size) != OK) {
			*errnop = ENOMEM;
			ret = NSS_STATUS_NOTFOUND;
			goto libnss_role_out;
		}
	}

	for(i = 0; i < ans.size; i++)
		(*groups)[(*start)++] = ans.list[i];

libnss_role_out:
	free(ans.list);
	free(col.list);
	free_all(&G);
	pthread_mutex_unlock(&mutex);
	return ret;
}

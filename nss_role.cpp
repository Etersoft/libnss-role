#include <nss.h>
#include <grp.h>

#include <pthread.h>

#include <sys/types.h>
#include <errno.h>

#include <stdexcept>
//#include <iostream>

#include "lock.h"
#include "roleParserSimple.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static bool realloc_groups (long int **size, gid_t ***groups, long int limit)
{
	long int new_size;
	gid_t *new_groups;

	new_size = 2 * (**size);
	if (limit > 0) {
		if (**size == limit)
			return false;

		if (new_size > limit)
			new_size = limit;
	}

	new_groups = (gid_t *)
		realloc((**groups),
			new_size * sizeof(***groups));
	if (!new_groups)
		return false;

	**groups = new_groups;
	**size = new_size;

	return true;
}

static void add_groups_insert (gid_t gid, const Roles &roles, Groups &groups, Groups &rolesAsGroups)
{
	Roles::const_iterator iter = roles.find(gid);
	if (iter != roles.end()) {
		const Privs &privs = iter->second;
		gid_t rid = iter->first;

		rolesAsGroups.insert(rid);

		for (Privs::const_iterator i = privs.begin(); i != privs.end(); i++) {
			groups.insert(privs.begin(), privs.end());
			if (roles.find(rid) != roles.end() && rolesAsGroups.find(rid) == rolesAsGroups.end())
				add_groups_insert(rid, roles, groups, rolesAsGroups);
		}
	}
}

extern "C" {

enum nss_status _nss_role_initgroups_dyn (char *user, gid_t main_group, long int *start, long int *size, gid_t **groups, long int limit, int *errnop)
{
	nss_status ret = NSS_STATUS_SUCCESS;
	
	Lock lock(&mutex);

//	std::cerr << "_nss_role_initgroups start" << std::endl;

	try {
		Roles roles;
		Groups add_groups, role_groups;
		long int last_start = *start;
		if (!RoleParserSimple().Update(roles))
			return ret;

		add_groups_insert(main_group, roles, add_groups, role_groups);
		for (long int i = 0; i < last_start; i++) {
			role_groups.clear();
			add_groups_insert((*groups)[i], roles, add_groups, role_groups);
		}

		add_groups.erase(main_group);
		for (long int i = 0; i < last_start; i++)
			add_groups.erase((*groups)[i]);

		for (Groups::iterator i = add_groups.begin(); i != add_groups.end(); i++) {
			if (*start == *size) {
				if (!realloc_groups (&size, &groups, limit))
					break;
			}
			(*groups)[(*start)++] = *i;
		}
	} catch (std::bad_alloc error) {
//		std::cerr << "_nss_role_initgroups memory_allocate_error: " << error.what() << std::endl;
		*errnop = ENOMEM;
		ret = NSS_STATUS_NOTFOUND;
	} catch (std::runtime_error error) {
//		std::cerr << "_nss_role_initgroups runtime_error: " << error.what() << std::endl;
		ret = NSS_STATUS_UNAVAIL;
	} catch (std::logic_error error) {
//		std::cerr << "_nss_role_initgroups logic_error:" << error.what() << std::endl;; 
		ret = NSS_STATUS_UNAVAIL;
	} catch (...) {
//		std::cerr << "_nss_role_initgroups!!!\n"; 
		ret = NSS_STATUS_UNAVAIL;
	}

//	std::cerr << "_nss_role_initgroups end" << std::endl;

	return ret;
}

} // extern "C"

#ifndef LIBNSS_ROLE_COMMON_H_
#define LIBNSS_ROLE_COMMON_H_

#include <map>
#include <set>
#include <vector>

typedef std::set<gid_t> Groups;
typedef std::vector<gid_t> Privs;
typedef std::map<gid_t, Privs> Roles;
typedef std::pair<gid_t, Privs> Role;

#endif /*LIBNSS_ROLE_COMMON_H_*/

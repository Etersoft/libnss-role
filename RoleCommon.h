#ifndef LIBNSS_ROLE_COMMON_H_
#define LIBNSS_ROLE_COMMON_H_

#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <stdexcept>

typedef std::set<gid_t> Groups;
typedef std::vector<gid_t> Privs;
typedef std::map<gid_t, Privs> Roles;
typedef std::pair<gid_t, Privs> Role;

std::ostream& operator <<(std::ostream &os, const Role &role);
std::ostream& operator <<(std::ostream &os, const Roles &roles);

class GroupMap
{
	typedef std::map<std::string, gid_t> NameMap;
	typedef std::map<gid_t, std::string> GidMap;
	NameMap name2gid;
	GidMap gid2name;
public:
	GroupMap() {}
	gid_t operator[] (const std::string&);
	std::string operator[] (gid_t);
};

std::ostream& output_gid(std::ostream &os, gid_t gid, GroupMap &groupmap);
std::ostream& output_role(std::ostream &os, const Role &role, GroupMap &groupmap);

#endif /*LIBNSS_ROLE_COMMON_H_*/

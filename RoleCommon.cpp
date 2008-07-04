#include <Role/RoleCommon.h>
#include <Role/GroupReader.h>

gid_t GroupMap::operator[] (const std::string& name)
{
	NameMap::iterator i = name2gid.find(name);

	if (i != name2gid.end())
		return i->second;

	gid_t gid = Group(name).gid();
	name2gid.insert(NameMap::value_type(name, gid));
	gid2name.insert(GidMap::value_type(gid, name));

	return gid;
}

std::string GroupMap::operator[] (gid_t gid)
{
	GidMap::iterator i = gid2name.find(gid);

	if (i != gid2name.end())
		return i->second;

	std::string name = Group(gid).name();
	gid2name.insert(GidMap::value_type(gid, name));
	name2gid.insert(NameMap::value_type(name, gid));

	return name;
}


std::ostream& output_gid(std::ostream &os, gid_t gid, GroupMap &groupmap)
{
	try {
		const std::string &name = groupmap[gid];
		os << name;
	} catch(...) {
		os << "(gid=" << gid << ")";
	}
	
	return os;
}

std::ostream& output_role(std::ostream &os, const Role &role, GroupMap &groupmap)
{
	const Privs &privs = role.second;
	output_gid(os, role.first, groupmap) << ':';
	for (Privs::const_iterator j = privs.begin(); j != privs.end(); j++) {
		output_gid(os << ' ', *j, groupmap);
		if ((j+1) != privs.end())
			os << ',';
	}

	return os << std::endl;
}

std::ostream& operator <<(std::ostream &os, const Role &role)
{
	GroupMap groupmap;
	return output_role(os, role, groupmap);
}

std::ostream& operator <<(std::ostream &os, const Roles &roles)
{
	GroupMap groupmap;

	for (Roles::const_iterator i = roles.begin(); i != roles.end(); i++)
		output_role(os, *i, groupmap);

	return os;
}

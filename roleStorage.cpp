#include "roleStorage.h"

bool RoleStorage::Store(const Roles &roles)
{
	out.open(filename.c_str());
	if (!out)
		return false;

	for (Roles::const_iterator i = roles.begin(); i != roles.end(); ++i)
	{
		gid_t id = i->first;
		const Privs &privs = i->second;
		out << id << ':';

		Privs::const_iterator j = privs.begin();
		if (j != privs.end())
			out << *j++;
		while (j != privs.end())
			out << ',' << *j++;
		out << std::endl;
	}

	return true;
}

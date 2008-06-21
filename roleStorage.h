#ifndef LIBNSS_ROLE_STORAGE_H_
#define LIBNSS_ROLE_STORAGE_H_

#include <string>
#include <ostream>

#include "roleCommon.h"

class RoleStorage
{
protected:
	std::string filename;
	bool StoreCycle(const Roles &roles, std::ostream &out);
public:
	RoleStorage (const std::string &filename = "/etc/role"):
		filename(filename) {}
	bool Store(const Roles &roles, int fd = -1);
};

#endif /*LIBNSS_ROLE_STORAGE_H_*/

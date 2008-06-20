#ifndef LIBNSS_ROLE_STORAGE_H_
#define LIBNSS_ROLE_STORAGE_H_

#include <fstream>
#include <string>

#include "roleCommon.h"

class RoleStorage
{
protected:
	std::string filename;
	std::ofstream out;
public:
	RoleStorage (const std::string &filename = "/etc/role"):
		filename(filename) {}
	bool Store(const Roles &roles);
};

#endif /*LIBNSS_ROLE_STORAGE_H_*/

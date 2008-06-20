#ifndef LIBNSS_ROLE_PARSER_H_
#define LIBNSS_ROLE_PARSER_H_

#include <fstream>
#include <string>

#include "roleCommon.h"

class RoleParser
{
protected:
	std::string filename;
	std::ifstream in;
	bool Parse(char const* str, gid_t &role_id, Privs &privs);
public:
	RoleParser (const std::string &filename = "/etc/role"):
		filename(filename) {}
	bool Update(Roles &roles);
};

#endif /*LIBNSS_ROLE_PARSER_H_*/

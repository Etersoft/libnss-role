#ifndef LIBNSS_ROLE_PARSER_SIMPLE_H_
#define LIBNSS_ROLE_PARSER_SIMPLE_H_

#include <string>
#include <istream>

#include "roleCommon.h"

class RoleParserSimple
{
protected:
	std::string filename;
	bool Parse(char const* str, gid_t &role_id, Privs &privs);
	bool ParseCycle(Roles &roles, std::istream &in);
public:
	RoleParserSimple (const std::string &filename = "/etc/role"):
		filename(filename) {}
	bool Update(Roles &roles);
};

#endif /*LIBNSS_ROLE_PARSER_SIMPLE_H_*/

#ifndef LIBNSS_ROLE_PARSER_H_
#define LIBNSS_ROLE_PARSER_H_

#include <string>
#include <istream>

#include <RoleParserSimple.h>

class RoleParser: public RoleParserSimple
{
public:
	RoleParser (const std::string &filename = "/etc/role"):
		RoleParserSimple(filename) {}
	bool Update(Roles &roles, int fd = -1);
};

#endif /*LIBNSS_ROLE_PARSER_H_*/

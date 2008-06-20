#include "roleParser.h"

#include <boost/spirit/core.hpp>

using boost::spirit::parse;
using boost::spirit::uint_p;
using boost::spirit::space_p;
using boost::spirit::assign;
using boost::spirit::push_back_a;

bool RoleParser::Parse(char const* str, gid_t &role_id, Privs &privs)
{
	return parse(str,
	(
	 uint_p[assign(role_id)] >>
	 ':' >>
	 uint_p[push_back_a(privs)] >>
	 *(',' >> uint_p[push_back_a(privs)])),
	 space_p).full;
}

bool RoleParser::Update(Roles &roles)
{
	in.open(filename.c_str());
	if (!in)
		return false;

	roles.clear();
	while (in && !in.eof())
	{
		Privs privs;
		gid_t role_id;
		std::string rbuf;

		getline (in, rbuf);
		if (rbuf.length() == 0)
			continue;
		if (!Parse(rbuf.c_str(), role_id, privs))
			continue;

		roles.insert(Role(role_id, privs));
	}

	return true;
}

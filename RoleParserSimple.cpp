#include <Role/RoleParserSimple.h>

#include <boost/spirit/core.hpp>
#include <fstream>

using boost::spirit::parse;
using boost::spirit::uint_p;
using boost::spirit::space_p;
using boost::spirit::assign;
using boost::spirit::push_back_a;

bool RoleParserSimple::Parse(char const* str, gid_t &role_id, Privs &privs)
{
	return parse(str,
	(
	 uint_p[assign(role_id)] >>
	 ':' >>
	 !(uint_p[push_back_a(privs)] >>
	  *(',' >> uint_p[push_back_a(privs)]))),
	 space_p).full;
}

bool RoleParserSimple::ParseCycle(Roles &roles, std::istream &in)
{
	roles.clear();

	while (in && !in.eof())
	{
		Privs privs;
		gid_t role_id;
		std::string rbuf;

		getline (in, rbuf);
		if (!in || rbuf.length() == 0)
			continue;
		if (!Parse(rbuf.c_str(), role_id, privs))
			continue;

		roles.insert(Role(role_id, privs));
	}

	return true;
}

bool RoleParserSimple::Update(Roles &roles)
{
	std::ifstream in(filename.c_str());
	if (!in)
		return false;

	return ParseCycle(roles, in);
}

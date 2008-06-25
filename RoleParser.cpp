#include "roleParser.h"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace io = boost::iostreams;

bool RoleParser::Update(Roles &roles, int fd)
{
	if (fd < 0)
		return RoleParserSimple::Update(roles);

	io::file_descriptor_source fd_source(fd);
	io::stream_buffer<io::file_descriptor_source> buf(fd_source);
	std::istream in(&buf);
	if (!in)
		return false;

	return ParseCycle(roles, in);
}

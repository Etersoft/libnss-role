#include <fstream>
#include <Role/RoleStorage.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace io = boost::iostreams;

bool RoleStorage::StoreCycle(const Roles &roles, std::ostream &out)
{
	for (Roles::const_iterator i = roles.begin(); out && i != roles.end(); ++i)
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

	return out;
}

bool RoleStorage::Store(const Roles &roles, int fd)
{
	if (fd < 0) {
		std::ofstream out(filename.c_str());
		if (out)
			return StoreCycle(roles, out);

		return false;
	}

	io::file_descriptor_source fd_source(fd);
	io::stream_buffer<io::file_descriptor_source> buf(fd_source);
	std::ostream out(&buf);
	if (!out)
		return false;

	return StoreCycle(roles, out);
}

#include <sys/types.h>
#include <unistd.h>

#include "groupReader.h"

class buf_size_error: public errno_error
{
public:
	explicit buf_size_error(std::string what, int err):
		errno_error(what, errno) {}
};

Group::Group(gid_t gid):
	buf(grp_buf_default_size)
{
	try {
		getgrgid (gid);
	} catch (buf_size_error) {
		buf.resize(max_grp_size());
		getgrgid (gid);
	}
}

Group::Group(const std::string &name):
	buf(grp_buf_default_size)
{
	try {
		getgrnam (name);
	} catch (buf_size_error) {
		buf.resize(max_grp_size());
		getgrnam (name);
	}
}

size_t Group::max_grp_size ()
{
	static size_t max_size = 0;
	if (max_size == 0)
		max_size = sysconf(_SC_GETGR_R_SIZE_MAX);
	return max_size;
}

void Group::getgrgid (gid_t gid)
{
	size_t len = buf.capacity();
	char *buffer = &buf[0];
	struct group* grp_ptr;
	if (getgrgid_r(gid, &grp, buffer, len, &grp_ptr) == 0) {
		if (errno == ERANGE)
			throw buf_size_error("getgrgid_r: not enough space in buffer", errno);
		else if (errno != 0)
			throw errno_error ("getgrgid_r: error", errno);
	}

	if (!grp_ptr)
		throw no_such_error ("getgrgid_r: no such group");
}

void Group::getgrnam (const std::string &name)
{
	size_t len = buf.capacity();
	char *buffer = &buf[0];
	struct group* grp_ptr;
	if (getgrnam_r(name.c_str(), &grp, buffer, len, &grp_ptr) == 0) {
		if (errno == ERANGE)
			throw buf_size_error("getgrgid_r: not enough space in buffer", errno);
		else if (errno != 0)
			throw errno_error ("getgrgid_r: error", errno);
	}

	if (!grp_ptr)
		throw no_such_error ("getgrgid_r: no such group");
}

#include <sys/types.h>
#include <unistd.h>

#include <Role/UserReader.h>

User::User(uid_t uid):
	buf(pwd_buf_default_size)
{
	try {
		getpwuid (uid);
	} catch (buf_size_error) {
		buf.resize(max_pwd_size());
		getpwuid (uid);
	}
}

User::User(const std::string &name):
	buf(pwd_buf_default_size)
{
	try {
		getpwnam (name);
	} catch (buf_size_error) {
		buf.resize(max_pwd_size());
		getpwnam (name);
	}
}

size_t User::max_pwd_size ()
{
	static size_t max_size = 0;
	if (max_size == 0)
		max_size = sysconf(_SC_GETPW_R_SIZE_MAX);
	return max_size;
}

void User::getpwuid (uid_t uid)
{
	size_t len = buf.capacity();
	char *buffer = &buf[0];
	struct passwd* pwd_ptr;
	if (getpwuid_r(uid, &pwd, buffer, len, &pwd_ptr) == 0) {
		if (errno == ERANGE)
			throw buf_size_error("getpwuid_r: not enough space in buffer", errno);
		else if (errno != 0)
			throw errno_error ("getpwuid_r: error", errno);
	}

	if (!pwd_ptr)
		throw no_such_error ("getpwuid_r: no such group");
}

void User::getpwnam (const std::string &name)
{
	size_t len = buf.capacity();
	char *buffer = &buf[0];
	struct passwd* pwd_ptr;
	if (getpwnam_r(name.c_str(), &pwd, buffer, len, &pwd_ptr) == 0) {
		if (errno == ERANGE)
			throw buf_size_error("getpwnam_r: not enough space in buffer", errno);
		else if (errno != 0)
			throw errno_error ("getpwnam_r: error", errno);
	}

	if (!pwd_ptr)
		throw no_such_error ("getpwnam_r: no such group");
}

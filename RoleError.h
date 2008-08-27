#ifndef LIBNSS_ROLE_ERROR_H_
#define LIBNSS_ROLE_ERROR_H_

#include <stdexcept>
#include <errno.h>

class system_error: public std::runtime_error
{
public:
	explicit system_error(std::string what):
		std::runtime_error(what) {}
};

class errno_error: public system_error
{
	int code;
public:
	explicit errno_error(std::string what, int err):
		system_error(what), code(err) {}
	int get_errno() { return code; }
};

class buf_size_error: public errno_error
{
public:
	explicit buf_size_error(std::string what, int err):
		errno_error(what, errno) {}
};

class no_such_error: public system_error
{
public:
	explicit no_such_error(std::string what):
		system_error(what) {}
};

class auth_error: public system_error
{
public:
	explicit auth_error(std::string what):
		system_error(what) {}
};

#endif /*LIBNSS_ROLE_COMMON_H_*/

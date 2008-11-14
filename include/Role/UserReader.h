#ifndef LIBNSS_USER_READER_H_
#define LIBNSS_USER_READER_H_

#include <string>
#include <vector>
#include <pwd.h>
#include <errno.h>

#include <Role/RoleError.h>

class User {
	struct passwd pwd;
	std::vector<char> buf;

	static const size_t pwd_buf_default_size = 1024;
	static const size_t pwd_struct_size = sizeof (struct passwd); 

	size_t max_pwd_size();
	void getpwuid (uid_t uid);
	void getpwnam (const std::string&);
public:
	User(const std::string &name);
	User(uid_t uid);
	const char* name() {
		return pwd.pw_name;
	}
	const char* passwd() {
		return pwd.pw_passwd;
	}
	uid_t uid() {
		return pwd.pw_uid;
	}
	gid_t gid() {
		return pwd.pw_gid;
	}
	const char* gecos() {
		return pwd.pw_gecos;
	}
	const char* dir() {
		return pwd.pw_dir;
	}
	const char* home() {
		return pwd.pw_dir;
	}
	const char* shell() {
		return pwd.pw_shell;
	}
};

#endif /*LIBNSS_USER_READER_H_*/

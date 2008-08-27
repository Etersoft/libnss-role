#ifndef LIBNSS_GROUP_READER_H_
#define LIBNSS_GROUP_READER_H_

#include <string>
#include <vector>
#include <grp.h>
#include <errno.h>

#include <Role/RoleError.h>

class Group {
	struct group grp;
	std::vector<char> buf;

	static const size_t grp_buf_default_size = 1024;
	static const size_t grp_struct_size = sizeof (struct group); 

	size_t max_grp_size();
	void getgrgid (gid_t gid);
	void getgrnam (const std::string&);
public:
	Group(const std::string &name);
	Group(gid_t gid);
	const char* name() {
		return grp.gr_name;
	}
	const char* passwd() {
		return grp.gr_passwd;
	}
	gid_t gid() {
		return grp.gr_gid;
	}
	//const char** members() {
	//	return grp.gr_mem;
	//}
};

#endif /*LIBNSS_GROUP_READER_H_*/

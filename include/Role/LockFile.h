#ifndef LIBNSS_ROLE_LOCKFILE_H_
#define LIBNSS_ROLE_LOCKFILE_H_

#include <string>

#include <Role/RoleError.h>

class LockFile
{
	std::string lockfile;
	void DoLock (const std::string &file, const std::string &uniq);
	bool CheckLinkCount(const std::string &file);
public:
	LockFile (const std::string &file);
	~LockFile ();
};

#endif /*LIBNSS_ROLE_LOCKFILE_H_*/

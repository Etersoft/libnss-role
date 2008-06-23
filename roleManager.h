#ifndef LIBNSS_ROLE_MANAGER_H_
#define LIBNSS_ROLE_MANAGER_H_

#include <string>
#include <stdexcept>

#include "roleCommon.h"
#include "lockFile.h"

class system_error: public std::runtime_error
{
public:
	explicit system_error(std::string what):
		std::runtime_error(what) {}
};

class RoleManager
{
	int fd;
	Roles roles;
	std::string config;
	LockFile locker;
	bool initialized;
public:
	typedef std::string Name;
	typedef std::vector<std::string> PrivList;
	RoleManager(const std::string &config);
	~RoleManager();
	bool isInitialized() {
		return initialized;
	}
	void Update();
	void Store();
	void Add(const Name &name, const PrivList &list);
	void Set(const Name &name, const PrivList &list);
	void Delete(const Name &name, const PrivList &list);
	void Remove(const Name &name);
};

#endif /*LIBNSS_ROLE_MANAGER_H_*/

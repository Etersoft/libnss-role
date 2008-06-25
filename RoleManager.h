#ifndef LIBNSS_ROLE_MANAGER_H_
#define LIBNSS_ROLE_MANAGER_H_

#include <string>
#include <stdexcept>

#include <Role/RoleCommon.h>
#include <Role/LockFile.h>

class system_error: public std::runtime_error
{
public:
	explicit system_error(std::string what):
		std::runtime_error(what) {}
};

class GroupMap: std::map<std::string,gid_t>
{
public:
	GroupMap() {}
	gid_t operator[] (const std::string&);
};

class RoleManager
{
public:
	typedef std::vector<std::string> PrivNames;
private:
	int fd;
	Roles roles;
	std::string config;
	LockFile locker;
	bool initialized;
	GroupMap groupmap;

	void fillGroups(Groups &groups, const PrivNames &list);
	Privs getPrivs(const Groups &groups);
public:
	RoleManager(const std::string &config);
	~RoleManager();
	bool isInitialized() {
		return initialized;
	}
	void Update();
	void Store();
	void Add(const std::string &name, const PrivNames &list);
	void Set(const std::string &name, const PrivNames &list);
	void Delete(const std::string &name, const PrivNames &list);
	void Remove(const std::string &name);
};

#endif /*LIBNSS_ROLE_MANAGER_H_*/

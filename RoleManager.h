#ifndef LIBNSS_ROLE_MANAGER_H_
#define LIBNSS_ROLE_MANAGER_H_

#include <string>

#include <Role/RoleCommon.h>
#include <Role/LockFile.h>

class RoleManager;
std::ostream& operator <<(std::ostream &os, const RoleManager &manager);

class RoleManager
{
public:
	typedef std::vector<std::string> PrivNames;
private:
	int fd;
	Roles roles;
	std::string config;
	LockFile locker;
	GroupMap groupmap;

	void fillGroups(Groups &groups, const PrivNames &list);
	Privs getPrivs(const Groups &groups);
public:
	RoleManager(const std::string &config);
	~RoleManager();

	void Update();
	void Store();

	void Add(const std::string &name, const PrivNames &list);
	void Set(const std::string &name, const PrivNames &list);
	void Delete(const std::string &name, const PrivNames &list);
	void Remove(const std::string &name);

	friend std::ostream& operator <<(std::ostream &os, const RoleManager &manager);
};

#endif /*LIBNSS_ROLE_MANAGER_H_*/

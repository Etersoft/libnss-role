#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "roleManager.h"
#include "roleStorage.h"
#include "roleParser.h"
#include "groupReader.h"

gid_t GroupMap::operator[] (const std::string& name)
{
	iterator i = this->find(name);

	if (i != this->end())
		return i->second;

	gid_t gid = Group(name).gid();
	this->insert(value_type(name, gid));

	return gid;
}

RoleManager::RoleManager(const std::string &filename): config(filename), fd(-1), initialized(false), locker(config)
{
	if (!locker.isLocked())
		return;

//	fd = open(filename.c_str(), O_RDWR|O_EXCL);
//	if (fd >= 0)
		initialized = true;
}

RoleManager::~RoleManager()
{
	if (fd < 0)
		return;

	close(fd);
}

void RoleManager::Update()
{
	if (!initialized)
		throw system_error("RoleManager not initialized");

	if (!RoleParser(config).Update(roles), fd)
		system_error("RoleManager Parser error");
}

void RoleManager::Store()
{
	if (!initialized)
		throw system_error("RoleManager not initialized");

	if (!RoleStorage(config).Store(roles), fd)
		system_error("RoleManager Store error");
}

Privs RoleManager::getPrivs(const Groups &groups)
{
	return Privs(groups.begin(), groups.end());
}

void RoleManager::fillGroups(Groups &groups, const PrivNames &list)
{
	for (PrivNames::const_iterator i = list.begin(); i != list.end(); i++)
	{
		gid_t id;
		try {
			id = groupmap[*i];
		} catch(...) {
			continue;
		}
		groups.insert(id);
	}
}

void RoleManager::Add(const std::string &name, const PrivNames &list)
{
	Groups groups;
	gid_t gid = groupmap[name];

	fillGroups(groups, list);

	Roles::iterator curr = roles.find(gid);
	if (curr != roles.end()) {
		const Privs &privs = curr->second;
		for (Privs::const_iterator i = privs.begin(); i != privs.end(); i++)
			groups.insert(*i);
	}

	roles[gid] = getPrivs(groups);
}

void RoleManager::Set(const std::string &name, const PrivNames &list)
{
	Groups groups;
	gid_t gid = groupmap[name];

	fillGroups(groups, list);
	roles[gid] = getPrivs(groups);
}

void RoleManager::Delete(const std::string &name, const PrivNames &list)
{
	Groups groups;
	gid_t gid = groupmap[name];

	fillGroups(groups, list);

	Roles::iterator curr = roles.find(gid);
	if (curr != roles.end()) {
		const Privs &privs = curr->second;
		for (Groups::iterator i = groups.begin(); i != groups.end(); i++)
			groups.erase(*i);
		roles[gid] = getPrivs(groups);
	}
}

void RoleManager::Remove(const std::string &name)
{
	gid_t gid = groupmap[name];

	roles.erase(gid);
}

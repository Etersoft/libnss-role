#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <Role/RoleManager.h>
#include <Role/RoleStorage.h>
#include <Role/RoleParser.h>
#include <Role/GetText.h>

std::ostream& operator <<(std::ostream &os, const RoleManager &manager)
{
	return os << manager.roles;
}

RoleManager::RoleManager(const std::string &filename, const std::string &progname): config(filename), fd(-1), pamcheck(progname), locker(config)
{
//	fd = open(filename.c_str(), O_RDWR|O_EXCL);
//	if (fd >= 0)
}

RoleManager::~RoleManager()
{
	if (fd < 0)
		return;

    	close(fd);
}

void RoleManager::Update()
{
	if (!RoleParser(config).Update(roles, fd))
		throw system_error(_("RoleManager Parser error"));
}

void RoleManager::Store()
{
	if (!RoleStorage(config).Store(roles, fd))
		throw system_error(_("RoleManager Store error"));
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
		Groups newgroups(privs.begin(), privs.end());
		for (Groups::iterator i = groups.begin(); i != groups.end(); i++)
			newgroups.erase(*i);
		roles[gid] = getPrivs(newgroups);
	}
}

void RoleManager::Remove(const std::string &name)
{
	gid_t gid = groupmap[name];

	roles.erase(gid);
}

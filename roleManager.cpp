#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "roleManager.h"
#include "roleStorage.h"
#include "roleParser.h"
#include "groupReader.h"

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

void RoleManager::Add(const Name &name, const PrivList &list)
{
}

void RoleManager::Set(const Name &name, const PrivList &list)
{
}

void RoleManager::Delete(const Name &name, const PrivList &list)
{
}

void RoleManager::Remove(const Name &name)
{
}

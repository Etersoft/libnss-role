#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <Role/glob.h>

static int get_pid(char *buf, pid_t *pid)
{
	if (sscanf(buf, "%u", pid) < 1)
		return LIBROLE_IO_ERROR;

	return LIBROLE_OK;
}

static int check_link_count(const char *file)
{
	struct stat sb;

	if (stat (file, &sb) != 0) {
		return LIBROLE_UNKNOWN_ERROR;
	}

	if (sb.st_nlink != 2) {
		return LIBROLE_UNKNOWN_ERROR;
	}

	return LIBROLE_OK;
}


static int do_lock(const char *file, const char *lock)
{
	int fd;
	pid_t pid;
	ssize_t len;
	int result;
	char buf[32];

	fd = open(file, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (-1 == fd) {
		return 0;
	}

	pid = getpid();
	snprintf(buf, sizeof buf, "%lu", (unsigned long) pid);
	len = (ssize_t) strlen (buf) + 1;
	if (write(fd, buf, len) != len) {
		close (fd);
		unlink(file);
		return ;
	}
	close (fd);

	if (link(file, lock) == 0) {
		result = check_link_count (file);
		unlink(file);
		return result;
	}

	fd = open(lock, O_RDWR);
	if (fd == -1) {
		unlink(file);
		errno = EINVAL;
		return LIBROLE_IO_ERROR;
	}
	len = read(fd, buf, sizeof (buf) - 1);
	close(fd);
	if (len <= 0) {
		unlink(file);
		errno = EINVAL;
		return LIBROLE_IO_ERROR;
	}
	buf[len] = '\0';
	if ((result = get_pid(buf, &pid)) != LIBROLE_OK) {
		unlink(file);
		errno = EINVAL;
		return result;
	}
	if (kill(pid, 0) == 0) {
		unlink(file);
		errno = EEXIST;
		return LIBROLE_UNKNOWN_ERROR;
	}
	if (unlink(lock) != 0) {
		unlink(file);
		return LIBROLE_IO_ERROR;
	}

	result = 0;
	if ((link(file, lock) == 0) && (check_link_count(file) == LIBROLE_OK))
		result = LIBROLE_OK;

	unlink(file);
	return result;
}

int librole_lock(const char *file)
{
	char lock[1024];

	if (snprintf(lock, sizeof lock, "%s.lock", file) < 1)
		return LIBROLE_UNKNOWN_ERROR;

	return do_lock(file, lock);
}

int librole_unlock(const char *file)
{
	char lock[1024];

	if (snprintf(lock, sizeof lock, "%s.lock", file) < 1)
		return LIBROLE_UNKNOWN_ERROR;

	unlink(lock);
	return LIBROLE_OK;
}

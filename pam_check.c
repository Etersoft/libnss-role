#include <pwd.h>
#include <errno.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "role/pam_check.h"
#include "role/glob.h"

static struct pam_conv conv = {
	misc_conv,
	NULL
};

static int get_user_name(uid_t uid, char *user_name)
{
	char buffer[1000];
	struct passwd pwd;
	struct passwd* pwd_ptr;
	if (getpwuid_r(uid, &pwd, buffer, 1000, &pwd_ptr) == 0) {
		if (errno == ERANGE)
			return LIBROLE_OUT_OF_RANGE;
		else if (errno != 0)
			return LIBROLE_UNKNOWN_ERROR;
	}

	if (!pwd_ptr)
		return LIBROLE_NO_SUCH_GROUP;

	strncpy(user_name, pwd_ptr->pw_name, 33);
	user_name[32] = '\0';
	return LIBROLE_OK;
}

int librole_pam_check(pam_handle_t *pamh, const char *prog, int *status)
{
	int result = PAM_SUCCESS;
	char user_name[33];

	pamh = 0;

	result = get_user_name(getuid(), user_name);
	if (result != LIBROLE_OK)
		return result;

	result = pam_start(prog, user_name, &conv, &pamh);
	if (result != PAM_SUCCESS)
		return LIBROLE_UNKNOWN_ERROR;

	result = pam_authenticate(pamh, 0);
	if (result != PAM_SUCCESS) {
		pam_end(pamh, result);
		return LIBROLE_UNKNOWN_ERROR;
	}
	result = pam_acct_mgmt(pamh, 0);
	if (result != PAM_SUCCESS) {
		pam_end(pamh, result);
		return LIBROLE_UNKNOWN_ERROR;
	}

	*status = result;

	return LIBROLE_OK;
}

int librole_pam_release(pam_handle_t *pamh, int status)
{
	int result = PAM_SUCCESS;

	result = pam_end(pamh, status);
	if (result != PAM_SUCCESS)
		return LIBROLE_UNKNOWN_ERROR;

	return LIBROLE_OK;
}

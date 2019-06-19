#include <errno.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "role/pam_check.h"
#include "role/glob.h"
#include "role/parser.h"

static struct pam_conv conv = {
    misc_conv,
    NULL
};

// check program prog for current user, put result in status
int librole_pam_check(pam_handle_t *pamh, const char *prog, int *status)
{
    int result;
    char user_name[LIBROLE_MAX_NAME];

    pamh = 0;

    result = librole_get_user_name(getuid(), user_name, LIBROLE_MAX_NAME);
    if (result != LIBROLE_OK)
        return result;

    result = pam_start(prog, user_name, &conv, &pamh);
    if (result != PAM_SUCCESS)
        return LIBROLE_PAM_ERROR;

    result = pam_authenticate(pamh, 0);
    if (result != PAM_SUCCESS) {
        pam_end(pamh, result);
        return LIBROLE_PAM_ERROR;
    }
    result = pam_acct_mgmt(pamh, 0);
    if (result != PAM_SUCCESS) {
        pam_end(pamh, result);
        return LIBROLE_PAM_ERROR;
    }

    *status = result;

    return LIBROLE_OK;
}

int librole_pam_release(pam_handle_t *pamh, int status)
{
    int result;

    result = pam_end(pamh, status);
    if (result != PAM_SUCCESS)
        return LIBROLE_PAM_ERROR;

    return LIBROLE_OK;
}

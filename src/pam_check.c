/*
 * Copyright (c) 2008-2020 Etersoft
 * Copyright (c) 2020 BaseALT
 *
 * NSS library for roles and privileges.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA.
 */
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

/* check program prog for current user, put result in status */
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

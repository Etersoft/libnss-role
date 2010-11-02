#ifndef LIBROLE_PAM_CHECK_H
#define LIBROLE_PAM_CHECK_H

#include <security/pam_appl.h>

int librole_pam_check(pam_handle_t *, const char *, int *);

int librole_pam_release(pam_handle_t *, int);

#endif

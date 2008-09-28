#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>

#include <Role/PamCheck.h>
#include <Role/UserReader.h>
#include <Role/GetText.h>

static struct pam_conv conv = {
	misc_conv,
	NULL
};

PamCheck::PamCheck(const std::string &prog): pamh(0)
{
	int retval = PAM_SUCCESS;

	User me (getuid ());
//	if (pampw == NULL)
//		retval = PAM_USER_UNKNOWN;

	retval = pam_start (prog.c_str(), me.name(), &conv, &pamh);

	if (retval == PAM_SUCCESS) {
		retval = pam_authenticate (pamh, 0);
		if (retval != PAM_SUCCESS)
			pam_end (pamh, retval);
	}

	if (retval == PAM_SUCCESS) {
		retval = pam_acct_mgmt (pamh, 0);
		if (retval != PAM_SUCCESS)
			pam_end (pamh, retval);
	}

	if (retval != PAM_SUCCESS) {
		throw auth_error(_("PAM authentication failed"));
		//fprintf (stderr, _("%s: PAM authentication failed\n"),
		//	 Prog);
	}
}

PamCheck::~PamCheck()
{
	int retval = PAM_SUCCESS;

	retval = pam_chauthtok (pamh, 0);
	if (retval != PAM_SUCCESS) {
		pam_end (pamh, retval);
		throw auth_error (_("PAM chauthtok failed"));
		//fprintf (stderr, _("%s: PAM chauthtok failed\n"), Prog);
	}

	pam_end (pamh, PAM_SUCCESS);
}

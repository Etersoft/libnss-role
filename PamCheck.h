#ifndef LIBNSS_ROLE_PAMCHECK_H_
#define LIBNSS_ROLE_PAMCHECK_H_

#include <security/_pam_types.h>

#include <Role/RoleError.h>

class PamCheck
{
	pam_handle_t *pamh;
public:
	PamCheck (const std::string &prog);
	~PamCheck ();
};

#endif /*LIBNSS_ROLE_PAMCHECK_H_*/

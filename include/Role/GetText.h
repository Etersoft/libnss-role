#ifndef LIBNSS_ROLE_GETTEXT_H_
#define LIBNSS_ROLE_GETTEXT_H_

#if ENABLE_NLS
# include <locale.h>
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# undef bindtextdomain
# define bindtextdomain(Domain, Directory) /* empty */
# undef textdomain
# define textdomain(Domain) /* empty */
# define _(Text) Text
# define setlocale(Category, Locale) /* empty */
#endif

void InitGetText();

#endif /*LIBNSS_ROLE_GETTEXT_H_*/

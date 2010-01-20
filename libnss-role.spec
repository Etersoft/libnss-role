Name: libnss-role
Version: 0.2.3
Release: alt1

Summary: NSS API library and admin tools for role and privilegies

License: GPLv3
URL: http://tartarus.ru/wiki/Projects/libnss_role
Group: System/Libraries
Packager: Pavel Shilovsky <piastry@altlinux.ru>

Source: %name-%version.tar

Requires(pre): chrooted >= 0.3.5-alt1 chrooted-resolv sed
Requires(postun): chrooted >= 0.3.5-alt1 sed

BuildRequires: glibc-devel scons
BuildRequires: libpam-devel

%description
NSS API library and admin tools for privilegies and roles.

%package devel
Summary: Header for developing applications managing Roles
Group: Development/Other
Requires: %name = %version-%release

%description devel
Headers for developing applications managing Roles throw
NSS API library for privilegies and roles.

%prep
%setup -q

%build
scons

%install
scons install DESTDIR=%buildroot LIBDIR=%_libdir LIBSYSDIR=/%_lib
mkdir -p %buildroot%_sysconfdir
touch %buildroot%_sysconfdir/role

%post
if [ "$1" = "1" ]; then
    grep -q '^group:[[:blank:]].\+role' /etc/nsswitch.conf || \
    sed -i.rpmorig 's/^\(group:[[:blank:]].\+\)$/\1 role/' /etc/nsswitch.conf
fi
update_chrooted all

%postun
if [ "$1" = "0" ]; then
    sed -i -e 's/ role//' /etc/nsswitch.conf
fi
update_chrooted all

%files
%config(noreplace) %_sysconfdir/role
%_sysconfdir/pam.d/role*
/%_lib/libnss_*.so.*
%_sbindir/*
%_bindir/*
%_libdir/*.so*
%_man8dir/*

%files devel
%_includedir/Role

%changelog
* Wed Jan 20 2010 Pavel Shilovsky <piastry@altlinux.org> 0.2.3-alt1
- Fixed bugs in pam_check and parser
- Add error messages

* Wed Nov 11 2009 Pavel Shilovsky <piastry@altlinux.org> 0.2.2-alt2
- Fixed passing sisyphus-check

* Wed Nov 11 2009 Pavel Shilovsky <piastry@altlinux.org> 0.2.2-alt1
- Migrated to  c-language utilites
- Bugs' fixing

* Wed Sep 30 2009 Pavel Shilovsky <piastry@altlinux.org> 0.2.1-alt1
- Code refactoring and bug's fixing.

* Fri Sep 11 2009 Pavel Shilovsky <piastry@altlinux.org> 0.2.0-alt1
- Implemented module on C language
- Added work with gids

* Thu Dec 11 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.6-alt1
- Fixed install directory for system tools
- Fixed manuals
- Improved error handling

* Fri Nov 14 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.5-alt1
- Replace include files

* Fri Nov 14 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.4-alt1
- Fixed helps for utilities
- Added utilities manuals
- Change project URL

* Wed Oct 29 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.3-alt4
- Fixed libdir installation path for x86_64

* Wed Oct 29 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.3-alt3
- Fixed build with gcc4.3

* Sun Sep 28 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.3-alt2
- Fixed nsswitch.conf update scripts
- Prepared for i18n

* Wed Aug 27 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.3-alt1
- Added pam support
- Started utils i18n support
- Added class UserReader

* Fri Jul 11 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.2-alt1
- Improved error handling

* Fri Jul 04 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.1-alt1
- Updated utilities:
 + Fixed roledel
 + Added rolelst

* Thu Jun 26 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1-alt1
- Initial build for ALT


Name: libnss-role
Version: 0.1.6
Release: alt2

Summary: NSS API library and admin tools for role and privilegies

License: GPLv3
URL: http://tartarus.ru/projects/show/libnss-role
Group: System/Libraries
Packager: Evgeny Sinelnikov <sin@altlinux.ru>

Source: %name-%version.tar

Requires(pre): chrooted >= 0.3.5-alt1 chrooted-resolv sed
Requires(postun): chrooted >= 0.3.5-alt1 sed

BuildRequires: gcc-c++ glibc-devel scons
BuildRequires: boost-devel >= 1:1.36.0
BuildRequires: boost-iostreams-devel >= 1:1.36.0
BuildRequires: boost-program-options-devel >= 1:1.36.0
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
    grep -q '^group:[[:blank:]]*\(.\+[[:blank:]]\+\)*role\($\|[[:blank:]]\)' \
        /etc/nsswitch.conf || \
    sed -i.rpmorig -e 's/^\(group:.\+\)$/\1 role/g' \
        /etc/nsswitch.conf
fi
update_chrooted all

%postun
if [ "$1" = "0" ]; then
    sed -i -e 's/^group:role/group:/g' \
           -e 's/^\(group:\)\(.\+[[:blank:]]*\)*[[:blank:]]\+role\($\|[[:blank:]].*\)$/\1\2\3/g' \
        /etc/nsswitch.conf
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
* Thu Feb 26 2009 Evgeny Sinelnikov <sin@altlinux.ru> 0.1.6-alt2
- Remove post_ldconfig and postun_ldconfig
- Fixed potential problem in sections post and postun (#18984)
- Adjusted project URL

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


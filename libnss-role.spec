Name: libnss-role
Version: 0.1
Release: alt1

Summary: NSS API library

License: GPL
URL: http://tartarus.ru/wiki/RolesAndPrivilegies
Group: System/Libraries
Packager: Evgeny Sinelnikov <sin@altlinux.ru>

Source: %name-%version.tar

Requires(pre): chrooted >= 0.3.5-alt1 chrooted-resolv sed
Requires(postun): chrooted >= 0.3.5-alt1 sed

BuildRequires: gcc-c++ glibc-devel scons
BuildRequires: boost-devel boost-iostreams-devel boost-program-options-devel

%description
NSS API library for privilegies and roles.

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
scons install DESTDIR=%buildroot
mkdir -p %buildroot%_sysconfdir
touch %buildroot%_sysconfdir/role

%post
%post_ldconfig
grep -q '^group:[[:blank:]].\+role' /etc/nsswitch.conf || \
sed -i.rpmorig 's/^\(group:[[:blank:]].\+\)$/\1 role/' /etc/nsswitch.conf
update_chrooted all

%postun
%postun_ldconfig
sed -i -e 's/ role//' /etc/nsswitch.conf
update_chrooted all

%files
%config(noreplace) %_sysconfdir/role
/%_lib/libnss_*.so.*
%_bindir/*
%_libdir/*.so*

%files devel
%_includedir/Role

%changelog
* Thu Jun 26 2008 Evgeny Sinelnikov <sin@altlinux.ru> 0.1-alt1
- Initial build for ALT


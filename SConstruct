opts = Options()
opts.Add('DESTDIR', 'Directory to install', '')
env = Environment(options=opts)
Help(opts.GenerateHelpText(env))

LIBDEVSUFFIX = ".so"

NSS_NAME = "libnss_role"
NSS_LIBBASEVERSION = "2"
NSS_LIBSUBVERSION = "0.0"
NSS_LIBFULLVERSION = NSS_LIBBASEVERSION + '.' + NSS_LIBSUBVERSION
NSS_LIBBASESUFFIX = LIBDEVSUFFIX + '.' + NSS_LIBBASEVERSION
NSS_LIBFULLSUFFIX = LIBDEVSUFFIX + '.' + NSS_LIBFULLVERSION
NSS_SONAME = NSS_NAME + NSS_LIBBASESUFFIX
NSS_FULLNAME = NSS_NAME + NSS_LIBFULLSUFFIX

COMMON_NAME = "librole"
COMMON_LIBBASEVERSION = "0"
COMMON_LIBSUBVERSION = "0.0"
COMMON_LIBFULLVERSION = COMMON_LIBBASEVERSION + '.' + COMMON_LIBSUBVERSION
COMMON_LIBBASESUFFIX = LIBDEVSUFFIX + '.' + COMMON_LIBBASEVERSION
COMMON_LIBFULLSUFFIX = LIBDEVSUFFIX + '.' + COMMON_LIBFULLVERSION
COMMON_SONAME = COMMON_NAME + COMMON_LIBBASESUFFIX
COMMON_FULLNAME = COMMON_NAME + COMMON_LIBFULLSUFFIX
COMMON_DEVNAME = COMMON_NAME + LIBDEVSUFFIX

env["CCFLAGS"] = '-O2 -I.'

libenv = env.Clone()
libenv["SHLIBSUFFIX"] = [NSS_LIBFULLSUFFIX]
libenv["LINKFLAGS"] = ['-Wl,-soname,' + NSS_SONAME]
parser = libenv.SharedObject('RoleParserSimple', 'RoleParserSimple.cpp')
so = libenv.SharedLibrary(NSS_NAME, ['nss_role.cpp', parser])
solink = libenv.Command(NSS_SONAME, so[0], 'ln -sf %s %s' % (NSS_FULLNAME, NSS_SONAME))

commonenv = libenv.Clone()
commonenv["SHLIBSUFFIX"] = [COMMON_LIBFULLSUFFIX]
commonenv["LINKFLAGS"] = ['-Wl,-soname,' + COMMON_SONAME]
commonenv["LIBS"] = ['boost_iostreams']
commonfiles = ['LockFile.cpp', 'RoleCommon.cpp', 'RoleManager.cpp', parser, 'GroupReader.cpp', 'RoleParser.cpp', 'RoleStorage.cpp']
common = commonenv.SharedLibrary(COMMON_NAME, commonfiles)
commonlink = commonenv.Command(COMMON_SONAME, common[0], 'ln -sf %s %s' % (COMMON_FULLNAME, COMMON_SONAME))
commondevlink = commonenv.Command(COMMON_DEVNAME, common[0], 'ln -sf %s %s' % (COMMON_FULLNAME, COMMON_DEVNAME))
commonheaders = ['LockFile.h', 'RoleManager.h', 'RoleParserSimple.h', 'GroupReader.h', 'RoleParser.h', 'RoleStorage.h', 'RoleError.h']

env = env.Clone()
env["LIBS"] = ['role','boost_program_options']
env["LIBPATH"] = '.'
roleadd = env.Program('roleadd', 'roleadd.cpp')
roledel = env.Program('roledel', 'roledel.cpp')
rolelst = env.Program('rolelst', 'rolelst.cpp')

i = commonenv.Install('$DESTDIR/usr/lib', common)
commonenv.Alias('install', i)
i = commonenv.Install('$DESTDIR/usr/include/Role', commonheaders)
commonenv.Alias('install', i)
i = commonenv.Command('$DESTDIR/usr/lib/' + COMMON_SONAME, commonlink[0], 'cp -P %s /$DESTDIR/usr/lib/%s' % (COMMON_SONAME, COMMON_SONAME))
commonenv.Alias('install', i)
i = commonenv.Command('$DESTDIR/usr/lib/' + COMMON_DEVNAME, commondevlink[0], 'cp -P %s /$DESTDIR/usr/lib/%s' % (COMMON_DEVNAME, COMMON_DEVNAME))
commonenv.Alias('install', i)

i = env.Install('$DESTDIR/usr/bin', [roleadd, roledel, rolelst])
env.Alias('install', i)

i = libenv.Install('$DESTDIR/lib', so)
libenv.Alias('install', i)
i = libenv.Command('$DESTDIR/lib/' + NSS_SONAME, solink[0], 'cp -P %s /$DESTDIR/lib/%s' % (NSS_SONAME, NSS_SONAME))
libenv.Alias('install', i)

opts = Options()
opts.Add('DESTDIR', 'Directory to install', '')
env = Environment(options=opts)
Help(opts.GenerateHelpText(env))

NAME = "libnss_role"
SUFFIX = ".so.2"
SONAME = NAME + SUFFIX

env["CCFLAGS"] = '-O2'

libenv = env.Clone()
libenv["SHLIBSUFFIX"] = [SUFFIX + '.0.0']
libenv["LINKFLAGS"] = ['-Wl,-soname,' + SONAME]
so = libenv.SharedLibrary(NAME, ['nss_role.cpp', 'roleParserSimple.cpp'])

roleadd = env.Program('roleadd', ['roleadd.cpp', 'lockFile.cpp', 'roleManager.cpp', 'roleParserSimple.cpp', 'roleParser.cpp', 'roleStorage.cpp'])
env["LIBS"] = ['-lboost_program_options', '-lboost_iostreams']

i = env.Install('$DESTDIR/usr/bin', roleadd)
env.Alias('install', i)

i = libenv.Install('$DESTDIR/lib', so)
libenv.Alias('install', i)

if 'install' in COMMAND_LINE_TARGETS:
        i = libenv.Command(NAME + '.so', so[0], 'ln -sf %s $DESTDIR/lib/%s' % ( SONAME+'.0.0', SONAME))
        libenv.Alias('install', i)

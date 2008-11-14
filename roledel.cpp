#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include <RoleManager.h>
#include <Role/GetText.h>
#include <Role/Version.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::exception;

static po::variables_map vm;

typedef RoleManager::PrivNames PrivNames;

static int getOptions(int ac, char* av[])
{
	try {
		po::options_description desc(_("Usage: roledel [-s] ROLE [GROUPS]..."));
		desc.add_options()
			("help,h", _("produce help message"))
			("remove,r", _("remove role instead delete privilegies from it"))
			("version,v", _("print the version of roledel being used"));

		po::positional_options_description p;
		p.add("role-name", 1).add("priv-names", -1);

		po::options_description hidden(_("Hidden options"));
		hidden.add_options()
			("config,c", po::value<string>(), _("config name"))
			("role-name", po::value<string>(), _("role name"))
			("priv-names", po::value<PrivNames>(), _("privilegies names"));

		po::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		po::store(po::command_line_parser(ac, av).
			options(cmdline_options).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 0;
		}

		if (vm.count("version")) {
			cout << "roledel is utility for libnss_role version " << VERSION << "\n";
			return 0;
		}

	}
	catch(exception& e) {
		cerr << _("error: ") << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << _("Exception of unknown type!\n");
		return 2;
	}

	return 0;
}

PrivNames getPrivs()
{
	PrivNames privs;

	if (vm.count("priv-names"))
		privs = vm["priv-names"].as<PrivNames>();

	return privs;
}

int main (int argc, char *argv[])
{
	static const char *default_config = "/etc/role";
	const char *config = default_config;

	InitGetText();

	int ret = getOptions(argc, argv);

	if (ret != 0)
		return ret;

	if (vm.count("help") || vm.count("version"))
		return 0;

	if (vm.count("config"))
		config = vm["config"].as<string>().c_str();

	try {
		RoleManager manager(config, argv[0]);

		manager.Update();

		PrivNames privs;
		string name = vm["role-name"].as<string>();

		if (vm.count("priv-names"))
			privs = vm["priv-names"].as<PrivNames>();

		if (vm.count("remove"))
			manager.Remove(name);
		else
			manager.Delete(name, privs);

		manager.Store();
	}
	catch(exception& e) {
		cerr << _("error: ") << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << _("Exception of unknown type!\n");
		return 2;
	}

	return 0;
}

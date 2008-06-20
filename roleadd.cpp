#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include "roleParser.h"
#include "roleStorage.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::exception;
//using namespace std;

static po::variables_map vm;

static int getOptions(int ac, char* av[])
{
	try {
		po::options_description desc("Usage: roleadd [-s] ROLE [GROUPS]...");
		desc.add_options()
			("help", "produce help message")
			("set,s", "replace privileges for role");

		po::positional_options_description p;
		p.add("role-name", 1).add("priv-names", -1);

		po::options_description hidden("Hidden options");
		hidden.add_options()
			("config,c", po::value< string >(), "config name")
			("role-name", po::value< string >(), "role name")
			("priv-names", po::value< vector<string> >(), "privilegies names");

		po::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		po::store(po::command_line_parser(ac, av).
			options(cmdline_options).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}

		if (vm.count("set")) {
			cout << "Set done.\n";
		} else {
			cout << "Set was not set.\n";
		}
		if (vm.count("role-name")) {
			cout << "Role: " << vm["role-name"].as<string>() << ".\n";
		} else {
			cout << "Role was not set.\n";
		}
		if (vm.count("config")) {
			cout << "Config: " << vm["config"].as<string>() << ".\n";
		} else {
			cout << "Config was not set.\n";
		}
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << "Exception of unknown type!\n";
	}

	return 0;
}

int main (int argc, char *argv[])
{
	static const char *default_config = "/etc/role";
	const char *config = default_config;

	int ret = getOptions(argc, argv);

	if (ret != 0)
		return ret;

	if (vm.count("config"))
		config = vm["config"].as<string>().c_str();

	Roles roles;

	if (!RoleParser(config).Update(roles)) {
		std::cerr << "read error\n";
		return 1;
	}

	if (!RoleStorage(config).Store(roles)) {
		std::cerr << "write error\n";
		return 2;
	}

	return 0;
}

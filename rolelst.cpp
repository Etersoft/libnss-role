#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include <RoleParser.h>
#include <Role/GetText.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::exception;

static po::variables_map vm;

typedef vector<string> RoleNames;

static int getOptions(int ac, char* av[])
{
	try {
		po::options_description desc(_("Usage: rolelst [-i] [ROLES]..."));
		desc.add_options()
			("help,h", _("produce help message"));
//			("ids,i", "print ids too");

		po::positional_options_description p;
		p.add("role-names", -1);

		po::options_description hidden(_("Hidden options"));
		hidden.add_options()
			("config,c", po::value<string>(), _("config name"))
			("role-names", po::value<RoleNames>(), _("role names"));

		po::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		po::store(po::command_line_parser(ac, av).
			options(cmdline_options).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
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

int main (int argc, char *argv[])
{
	static const char *default_config = "/etc/role";
	const char *config = default_config;

	InitGetText();

	int ret = getOptions(argc, argv);

	if (ret != 0)
		return ret;

	if (vm.count("help"))
		return 0;

	if (vm.count("config"))
		config = vm["config"].as<string>().c_str();

	try {
		Roles roles;

		RoleParser(config).Update(roles);

		if (vm.count("role-names")) {
			GroupMap groupmap;
			RoleNames rn = vm["role-names"].as<RoleNames>();
			for (RoleNames::iterator i = rn.begin(); i != rn.end(); i++) {
				gid_t gid;
				try {
					gid = groupmap[*i];
				} catch (...) {
					continue;
				}
				Roles::iterator r = roles.find(gid);
				if (r != roles.end())
					output_role(cout, *r, groupmap);
			}
		} else
			cout << roles;
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

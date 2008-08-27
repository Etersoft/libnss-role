#include <Role/GetText.h>

void InitGetText()
{
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
}

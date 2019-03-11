#include <3rdParty/docopt/docopt.h>

#include <iostream>

using namespace docopt;
using namespace std;

int main(int argc, const char** argv)
{
	string usage =
R"(Naval Fate.

    Usage:
      naval_fate ship new <name>...
      naval_fate ship <name> move <x> <y> [--speed=<kn>]
      naval_fate ship shoot <x> <y>
      naval_fate mine (set|remove) <x> <y> [--moored | --drifting]
      naval_fate (-h | --help)
      naval_fate --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --speed=<kn>  Speed in knots [default: 10].
      --moored      Moored (anchored) mine.
      --drifting    Drifting mine.
)";
		
	vector<string> args{ argv + 1, argv + argc };
	auto result = docopt::docopt(usage, args);

	// print it out in JSON form
	cout << "{ ";
	bool first = true;
	for (auto const& arg : result) {
		if (first) {
			first = false;
		}
		else {
			cout << "," << endl;
		}

		cout << '"' << arg.first << '"' << ": " << arg.second;
	}
	cout << " }" << endl;

	return 0;
}

#include "RTGIwRRF.h"
#include <QtWidgets/QApplication>

#include <qfile.h>
#include <qtextstream.h>
#include <iostream>

#include "GenFS.h"
#include <ROOT_PATH.h>

static const char USAGE[] =
R"(RTGIwRRF

    Usage:
      RTGIwRRF [--notrootpath] --sobj=<sobjPath> [--maxdepth=<maxDepth>] [--samplenum=<sampleNum>] [--notdenoise] --csv==<csvPath>

    Options:
      --notrootpath            path is not from root path
      --sobj <sobjPath>        sobj path.
      --maxdepth <maxDepth>    max depth [default: 20]
      --samplenum <sampleNum>  sample num [default: 16]
      --notdenoise             not denoise
      --csv <csvPath>          output file path
)";

using namespace std;
using namespace App;

void ShowArgRst(const map<string, docopt::value> & rst);

int main(int argc, char *argv[])
{
	string meanAndStd = ROOT_PATH + "data/FNN/2_mean_and_std.csv";
	string dense0 = ROOT_PATH + "data/FNN/2_dense0.csv";
	string dense1 = ROOT_PATH + "data/FNN/2_dense1.csv";
	string dense2 = ROOT_PATH + "data/FNN/2_dense2.csv";
	cout << GenFS::Call(meanAndStd, dense0, Activation::ReLU, dense1, Activation::ReLU, dense2, Activation::Identity) << endl;
	return 0;

	/*
	vector<string> args{ argv + 1, argv + argc };
	auto result = docopt::docopt(USAGE, args);
	ShowArgRst(result);

	RTGIwRRF::ArgMap argMap;
	for (auto arg : ENUM_ARG::_values())
		argMap[arg] = result[string("--") + arg._to_string()];

	QApplication a(argc, argv);

	// load style sheet
	QFile f(":qdarkstyle/Resources/style.qss");
	if (f.exists()){
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}
	else
		printf("Unable to set stylesheet, file not found\n");

	Qt::WindowFlags flags = 0;
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;

	RTGIwRRF w(argMap, Q_NULLPTR, flags);
	w.show();
	return a.exec();
	*/
}

void ShowArgRst(const map<string, docopt::value> & rst) {
	cout << "[ Arg Result ]" << endl << endl;
	cout << "{" << endl;
	bool first = true;
	for (auto const & arg : rst) {
		if (first)
			first = false;
		else
			cout << "," << endl;

		cout << '"' << arg.first << '"' << ": " << arg.second;
	}
	cout << endl << "}" << endl << endl;
}

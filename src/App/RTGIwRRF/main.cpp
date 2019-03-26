#include "RTGIwRRF.h"

#include <CppUtil/Basic/File.h>

#include <3rdParty/docopt/docopt.h>

#include <QtWidgets/QApplication>

#include <qfile.h>
#include <qtextstream.h>
#include <iostream>

#include "GenFS.h"
#include <ROOT_PATH.h>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

void ShowArgRst(const map<string, docopt::value> & rst);
bool InitShaders(const vector<int>  & IDs);

static const char USAGE[] =
R"(RTGIwRRF

    Usage:
      RTGIwRRF [-g]

    Options:
      -g   generate shader
)";

int main(int argc, char *argv[])
{
	vector<string> args{ argv + 1, argv + argc };
	auto result = docopt::docopt(USAGE, args);
	ShowArgRst(result);

	if (result.find("-g") != result.end()) {
		cout << "Gen shaders ..." << endl;
		if (!InitShaders({ 3, 12, 13, 14, 15, 16 })) {
			cout << "Gen shaders failed." << endl;
			return 1;
		}
		cout << "Gen shaders completed!" << endl;
	}

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

	RTGIwRRF w(Q_NULLPTR, flags);
	w.show();
	return a.exec();
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

bool InitShaders(const vector<int>  & IDs) {
	vector<Connection> connections = { Connection::Dense,Connection::Dense, Connection::Dense };
	vector<Activation> activations = { Activation::tanh, Activation::tanh, Activation::Identity };
	const string dirBase = ROOT_PATH + "data/FNN/";
	for (auto ID : IDs) {
		string rst = GenFS::Call(ID, dirBase + to_string(ID) + "/", dirBase + "template.fs", connections, activations);
		if (rst.empty()) {
			printf("ERROR: GenFS for ID %d error\n", ID);
			return false;
		}

		string filePath = ROOT_PATH + "data/shaders/App/RTGIwRRF/" + to_string(ID) + "_modelKDTree.fs";
		File file(filePath, File::WRITE);
		if (!file.IsValid()) {
			printf("ERROR: file[%s] open failed\n", filePath.c_str());
			return false;
		}
		file.Printf("%s", rst.c_str());
		file.Close();
	}

	return true;
}

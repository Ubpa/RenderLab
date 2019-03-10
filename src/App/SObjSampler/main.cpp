#include "SObjSampler.h"
#include <QtWidgets/QApplication>

#include <qfile.h>
#include <qtextstream.h>

int main(int argc, char *argv[])
{
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

	SObjSampler w(Q_NULLPTR, flags);
	w.show();
	return a.exec();
}

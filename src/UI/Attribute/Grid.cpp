#include <UI/Grid.h>

#include <qlabel.h>
#include <sstream>
#include <qpushbutton.h>
#include <qcolordialog.h>

using namespace Ui;
using namespace std;

void Grid::AddTitle(const QString & title) {
	auto label = new QLabel(page);
	label->setText(title);
	gridLayout->addWidget(label, gridLayout->rowCount(), 0);
}

void Grid::AddEditVal(const QString & text, double val, double singleStep, const function<void(double)> & slot) {
	auto label = new QLabel(page);
	label->setText(text);

	auto spinbox = new QDoubleSpinBox(page);
	spinbox->setMaximum(DBL_MAX);
	spinbox->setMinimum(-DBL_MAX);
	spinbox->setSingleStep(singleStep);
	spinbox->setValue(val);

	int row = gridLayout->rowCount();
	gridLayout->addWidget(label, row, 0);
	gridLayout->addWidget(spinbox, row, 1);

	void (QDoubleSpinBox::*signalFunc)(double) = &QDoubleSpinBox::valueChanged;
	page->connect(spinbox, signalFunc, slot);
}

void Grid::AddShowText(const QString & left, const QString & right) {
	auto label0 = new QLabel(page);
	label0->setText(left);

	auto label1 = new QLabel(page);
	label1->setText(right);

	int row = gridLayout->rowCount();
	gridLayout->addWidget(label0, row, 0);
	gridLayout->addWidget(label1, row, 1);
}

void Grid::AddEditColor(const QString & text, glm::vec3 & color) {
	auto label = new QLabel(page);
	label->setText(text);

	auto button = new QPushButton(page);
	stringstream stylesheet;
	stylesheet << "background-color: rgb(" << 255 * color.r << ", " << 255 * color.g << ", " << 255 * color.b << ");";
	button->setStyleSheet(QString::fromStdString(stylesheet.str()));

	page->connect(button, &QPushButton::clicked, [&color, button]() {
		const QColor qcolor = QColorDialog::getColor(QColor(255 * color.r, 255 * color.g, 255 * color.b));
		color.r = qcolor.red() / 255.0f;
		color.g = qcolor.green() / 255.0f;
		color.b = qcolor.blue() / 255.0f;
		printf("color : (%f, %f, %f)\n", color.r, color.g, color.b);
		stringstream stylesheet;
		stylesheet << "background-color: rgb(" << int(255 * color.r) << ", " << int(255 * color.g) << ", " << int(255 * color.b) << ");";
		button->setStyleSheet(QString::fromStdString(stylesheet.str()));
	});

	int row = gridLayout->rowCount();
	gridLayout->addWidget(label, row, 0);
	gridLayout->addWidget(button, row, 1);
}

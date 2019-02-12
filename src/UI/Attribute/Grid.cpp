#include <UI/Grid.h>

#include <qlabel.h>

using namespace Ui;

void Grid::AddTitle(const QString & title) {
	auto label = new QLabel(page);
	label->setText(title);
	gridLayout->addWidget(label, gridLayout->rowCount(), 0);
}

QDoubleSpinBox * Grid::AddLine(const QString & text, double val, double singleStep, const std::function<void(double)> & slot) {
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

	return spinbox;
}

#include <UI/Grid.h>

#include <qlabel.h>
#include <sstream>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qslider.h>

using namespace Ui;
using namespace std;

void Grid::AddLine(const string & text, QWidget * widget) {
	auto label = new QLabel;
	label->setText(QString::fromStdString(text));

	AddLine(label, widget);
}

void Grid::AddLine(QWidget * widgetLeft, QWidget * widgetRight) {
	int row = gridLayout->rowCount();
	if (widgetLeft != nullptr)
		gridLayout->addWidget(widgetLeft, row, 0);
	if (widgetRight != nullptr)
		gridLayout->addWidget(widgetRight, row, 1);
}

void Grid::Clear() {
	while (gridLayout->count()) {
		QWidget *p = gridLayout->itemAt(0)->widget();
		p->setParent(NULL);
		gridLayout->removeWidget(p);
		p->deleteLater();
	}
}

void Grid::AddText(const string & title) {
	AddLine(title, nullptr);
}

void Grid::AddEditVal(const string & text, double val, double singleStep, const function<void(double)> & slot) {
	auto spinbox = new QDoubleSpinBox;
	spinbox->setMaximum(DBL_MAX);
	spinbox->setMinimum(-DBL_MAX);
	spinbox->setSingleStep(singleStep);
	spinbox->setValue(val);

	void (QDoubleSpinBox::*signalFunc)(double) = &QDoubleSpinBox::valueChanged;
	page->connect(spinbox, signalFunc, slot);

	AddLine(text, spinbox);
}

void Grid::AddEditVal(const string & text, double val, double minVal, double maxVal, int stepNum, const std::function<void(double)> & slot) {

	double d_step = (maxVal - minVal) / stepNum;
	int i_val = (val - minVal) / d_step;
	auto horizontalSlider = new QSlider;
	horizontalSlider->setOrientation(Qt::Horizontal);
	horizontalSlider->setMinimum(0);
	horizontalSlider->setMaximum(stepNum);
	horizontalSlider->setValue(i_val);

	auto spinbox = new QDoubleSpinBox;
	spinbox->setMinimum(minVal);
	spinbox->setMaximum(maxVal);
	spinbox->setSingleStep(d_step);
	spinbox->setValue(val);

	void (QSlider::*signalFunc0)(int) = &QSlider::valueChanged;
	page->connect(horizontalSlider, signalFunc0, [spinbox, minVal, d_step, slot](int i_val) {
		double d_val = minVal + i_val * d_step;

		spinbox->setValue(d_val);
		slot(d_val);
	});

	void (QDoubleSpinBox::*signalFunc1)(double) = &QDoubleSpinBox::valueChanged;
	page->connect(spinbox, signalFunc1, [horizontalSlider, minVal, d_step, slot](double d_val) {
		int i_val = (d_val - minVal) / d_step;

		horizontalSlider->setValue(i_val);
		slot(d_val);
	});

	AddText(text);
	AddLine(spinbox, horizontalSlider);
}

void Grid::AddText(const string & left, const string & right) {
	auto label1 = new QLabel;
	label1->setText(QString::fromStdString(right));

	AddLine(left, label1);
}

void Grid::AddEditColor(const string & text, glm::vec3 & color) {
	auto button = new QPushButton;
	stringstream stylesheet;
	stylesheet << "background-color: rgb(" << 255 * color.r << ", " << 255 * color.g << ", " << 255 * color.b << ");";
	button->setStyleSheet(QString::fromStdString(stylesheet.str()));

	page->connect(button, &QPushButton::clicked, [&color, button]() {
		const QColor qcolor = QColorDialog::getColor(QColor(255 * color.r, 255 * color.g, 255 * color.b));
		if (!qcolor.isValid())
			return;

		color.r = qcolor.red() / 255.0f;
		color.g = qcolor.green() / 255.0f;
		color.b = qcolor.blue() / 255.0f;
		printf("color : (%f, %f, %f)\n", color.r, color.g, color.b);
		stringstream stylesheet;
		stylesheet << "background-color: rgb(" << int(255 * color.r) << ", " << int(255 * color.g) << ", " << int(255 * color.b) << ");";
		button->setStyleSheet(QString::fromStdString(stylesheet.str()));
	});

	AddLine(text, button);
}

QComboBox * Grid::AddComboBox(const string & text, const string & curText, pSlotMap slotMap) {
	if (slotMap->find(curText) == slotMap->cend()) {
		printf("ERROR: curText is not in slopMap");
		return nullptr;
	}

	auto combobox = new QComboBox;

	for (auto item : *slotMap)
		combobox->addItem(QString::fromStdString(item.first));

	combobox->setCurrentText(QString::fromStdString(curText));

	// 只有用户交互才会响应
	void (QComboBox::*signalFunc)(const QString &) = &QComboBox::activated;
	page->connect(combobox, signalFunc, [slotMap](const QString & itemText) {
		string str = itemText.toStdString();
		auto target = slotMap->find(str);
		if(target == slotMap->cend())
			return;

		target->second();
	});

	AddLine(text, combobox);

	return combobox;
}

void Grid::AddComboBox(QComboBox * combobox, const std::string & text, const std::string & curText, pSlotMap slotMap) {
	combobox->clear();

	for (auto item : *slotMap)
		combobox->addItem(QString::fromStdString(item.first));

	combobox->setCurrentText(QString::fromStdString(curText));

	// 只有用户交互才会响应
	void (QComboBox::*signalFunc)(const QString &) = &QComboBox::activated;
	page->connect(combobox, signalFunc, [slotMap](const QString & itemText) {
		string str = itemText.toStdString();
		auto target = slotMap->find(str);
		if (target == slotMap->cend())
			return;

		target->second();
	});

	AddLine(text, combobox);
}

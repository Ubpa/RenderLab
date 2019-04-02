#include <UI/Grid.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>

#include <qlabel.h>
#include <qgridlayout.h>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/qspinbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qslider.h>
#include <qcheckbox.h>

#include <sstream>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace Ui;
using namespace std;

Grid::Grid(QWidget * page)
	: isInit(false) {
	Init(page);
}

void Grid::Init(QWidget * page) {
	if (isInit)
		return;

	this->page = page;

	auto vboxLayout = new QVBoxLayout(page);
	gridLayout = new QGridLayout();
	auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	vboxLayout->addLayout(gridLayout);
	vboxLayout->addItem(verticalSpacer);

	isInit = true;
}

void Grid::AddRow(const string & text, QWidget * widget) {
	auto label = new QLabel;
	label->setText(QString::fromStdString(text));

	AddRow(label, widget);
}

void Grid::AddRow(QWidget * widgetLeft, QWidget * widgetRight) {
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
	AddRow(title, nullptr);
}

void Grid::AddEditVal(const string & text, double val, double singleStep, const function<void(double)> & slot) {
	auto spinbox = new QDoubleSpinBox;
	spinbox->setMaximum(DBL_MAX);
	spinbox->setMinimum(-DBL_MAX);
	spinbox->setSingleStep(singleStep);
	spinbox->setValue(val);

	void (QDoubleSpinBox::*signalFunc)(double) = &QDoubleSpinBox::valueChanged;
	page->connect(spinbox, signalFunc, slot);

	AddRow(text, spinbox);
}

void Grid::AddEditVal(const string & text, int val, const function<void(int)> & slot) {
	auto spinbox = new QSpinBox;
	spinbox->setMaximum(INT_MAX);
	spinbox->setMaximum(INT_MIN);
	spinbox->setSingleStep(1);
	spinbox->setValue(val);

	void (QSpinBox::*signalFunc)(int) = &QSpinBox::valueChanged;
	page->connect(spinbox, signalFunc, slot);

	AddRow(text, spinbox);
}

void Grid::AddEditVal(const string & text, double val, double minVal, double maxVal, int stepNum, const function<void(double)> & slot) {
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
	AddRow(spinbox, horizontalSlider);
}

void Grid::AddEditVal(const string & text, int val, int minVal, int maxVal, const function<void(int)> & slot) {
	auto horizontalSlider = new QSlider;
	horizontalSlider->setOrientation(Qt::Horizontal);
	horizontalSlider->setMinimum(minVal);
	horizontalSlider->setMaximum(maxVal);
	horizontalSlider->setValue(val);

	auto spinbox = new QSpinBox;
	spinbox->setMinimum(minVal);
	spinbox->setMaximum(maxVal);
	spinbox->setSingleStep(1);
	spinbox->setValue(val);

	void (QSlider::*signalFunc0)(int) = &QSlider::valueChanged;
	page->connect(horizontalSlider, signalFunc0, [=](int val) {
		spinbox->setValue(val);
		slot(val);
	});

	void (QSpinBox::*signalFunc1)(int) = &QSpinBox::valueChanged;
	page->connect(spinbox, signalFunc1, [=](int val) {
		horizontalSlider->setValue(val);
		slot(val);
	});

	AddText(text);
	AddRow(spinbox, horizontalSlider);
}

void Grid::AddEditVal(const string & text, volatile bool & val) {
	auto checkbox = new QCheckBox;
	checkbox->setChecked(val);
	checkbox->setText(QString::fromStdString(text));

	page->connect(checkbox, &QCheckBox::stateChanged, [&](int state) {
		if (state == Qt::Unchecked)
			val = false;
		else if (state == Qt::Checked)
			val = true;
	});

	AddRow(checkbox);
}

void Grid::AddText(const string & left, const string & right) {
	auto label1 = new QLabel;
	label1->setText(QString::fromStdString(right));

	AddRow(left, label1);
}

void Grid::AddEditColor(const string & text, CppUtil::RGBf & color) {
	auto button = new QPushButton;
	stringstream stylesheet;
	stylesheet << "background-color: rgb(" <<
		Math::Clamp<int>(255 * color.r, 0, 255) << ", " <<
		Math::Clamp<int>(255 * color.g, 0, 255) << ", " <<
		Math::Clamp<int>(255 * color.b, 0, 255) << ");";
	button->setStyleSheet(QString::fromStdString(stylesheet.str()));

	page->connect(button, &QPushButton::clicked, [&color, button]() {
		const QColor qcolor = QColorDialog::getColor(QColor(255 * color.r, 255 * color.g, 255 * color.b));
		if (!qcolor.isValid())
			return;

		color.r = qcolor.red() / 255.0f;
		color.g = qcolor.green() / 255.0f;
		color.b = qcolor.blue() / 255.0f;

		stringstream stylesheet;
		stylesheet << "background-color: rgb(" << int(255 * color.r) << ", " << int(255 * color.g) << ", " << int(255 * color.b) << ");";
		button->setStyleSheet(QString::fromStdString(stylesheet.str()));
	});

	AddRow(text, button);
}

void Grid::AddComboBox(QComboBox * combobox, const string & text, const vector<string> & items, const function<void(const string &)> & slot) {
	combobox->clear();

	for (auto item : items)
		combobox->addItem(QString::fromStdString(item));

	// 只有用户交互才会响应
	void (QComboBox::*signalFunc)(const QString &) = &QComboBox::activated;
	page->connect(combobox, signalFunc, [=](const QString & item) {
		slot(item.toStdString());
	});

	AddRow(text, combobox);
}

void Grid::AddComboBox(const string & text, const vector<string> & items, const function<void(const string &)> & slot) {
	AddComboBox(new QComboBox, text, items, slot);
}

bool Grid::AddComboBox(QComboBox * combobox, const string & text, const string & curText, pSlotMap slotMap) {
	if (slotMap->find(curText) == slotMap->cend()) {
		printf("ERROR: curText is not in slopMap");
		return false;
	}

	vector<string> items;
	for (auto item : *slotMap)
		items.push_back(item.first);

	auto slot = [=](const string & item) {
		auto target = slotMap->find(item);
		if (target == slotMap->cend())
			return;

		target->second();
	};

	AddComboBox(combobox, text, items, slot);

	combobox->setCurrentText(QString::fromStdString(curText));

	return true;
}

bool Grid::AddComboBox(const string & text, const string & curText, pSlotMap slotMap) {
	return AddComboBox(new QComboBox, text, curText, slotMap);
}

void Grid::AddComboBox(const string & text, const vector<string> & items, const string & btnText, const function<void(const string &)> & slot) {
	auto combobox = new QComboBox;

	for (auto item : items)
		combobox->addItem(QString::fromStdString(item));

	auto btn = new QPushButton;
	btn->setText(QString::fromStdString(btnText));
	page->connect(btn, &QPushButton::clicked, [=]() {
		slot(combobox->currentText().toStdString());
	});

	AddRow(text);
	AddRow(btn, combobox);
}

void Grid::AddEditImage(const string & text, Image::CPtr img, const function<void(Image::Ptr)> & slot) {
	auto imgLabel = new QLabel;
	imgLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	
	if (img == nullptr)
		ClearImgLabel(imgLabel);
	else
		SetImgLabel(imgLabel, img);

	auto loadImgBtn = new QPushButton;
	loadImgBtn->setText("Load");
	page->connect(loadImgBtn, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getOpenFileName(nullptr,
			"选择一张图片",
			"./",
			"Image Files (*.png)");
		if (fileName.isEmpty())
			return;
		
		auto img = ToPtr(new Image(fileName.toStdString().c_str()));
		if(SetImgLabel(imgLabel, img))
			slot(img);
	});

	auto clearImgBtn = new QPushButton;
	clearImgBtn->setText("Clear");
	page->connect(clearImgBtn, &QPushButton::clicked, [=]() {
		ClearImgLabel(imgLabel);
		slot(nullptr);
	});

	AddText(text);
	AddRow(imgLabel);
	AddRow(loadImgBtn);
	AddRow(clearImgBtn);
}

bool Grid::SetImgLabel(QLabel * imgLabel, Image::CPtr img) {
	const QImage::Format formatMap[5] = {
			QImage::Format::Format_Invalid,
			QImage::Format::Format_Alpha8,
			QImage::Format::Format_Invalid,
			QImage::Format::Format_RGB888,
			QImage::Format::Format_RGBA8888,
	};

	if (img == nullptr || !img->IsValid() || formatMap[img->GetChannel()] == QImage::Format::Format_Invalid) {
		QImage qImg(32, 32, QImage::Format::Format_RGB888);
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++)
				qImg.setPixel(QPoint(i, j), qRgb(255, 255, 255));
		}
		QPixmap imgIcon;
		imgIcon.convertFromImage(qImg.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		imgLabel->setPixmap(imgIcon);
		return false;
	}
	
	const float * imgData = img->GetData();
	const auto valNum = img->GetValNum();
	vector<unsigned char> imgUC(valNum);
	for (int i = 0; i < valNum; i++)
		imgUC[i] = static_cast<unsigned char>(255.f * Math::Clamp(imgData[i], 0.0f, 0.999999f));

	QImage qImg(imgUC.data(), img->GetWidth(), img->GetHeight(), formatMap[img->GetChannel()]);
	QPixmap imgIcon;
	imgIcon.convertFromImage(qImg.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	imgLabel->setPixmap(imgIcon);
	return true;
}

void Grid::ClearImgLabel(QLabel * imgLabel) {
	QImage qImg(32, 32, QImage::Format::Format_RGB888);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++)
			qImg.setPixel(QPoint(i, j), qRgb(255, 255, 255));
	}
	QPixmap imgIcon;
	imgIcon.convertFromImage(qImg.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	imgLabel->setPixmap(imgIcon);
}

void Grid::AddLine() {
	auto line0 = new QFrame();
	line0->setObjectName(QString::fromUtf8("line"));
	line0->setFrameShape(QFrame::HLine);
	line0->setFrameShadow(QFrame::Sunken);

	auto line1 = new QFrame();
	line1->setObjectName(QString::fromUtf8("line"));
	line1->setFrameShape(QFrame::HLine);
	line1->setFrameShadow(QFrame::Sunken);

	AddRow(line0, line1);
}

void Grid::AddTitle(const string & text) {
	AddLine();
	AddRow(text);
}

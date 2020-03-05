#pragma once

#include <Basic/HeapObj.h>

#include <UGM/UGM>

#include <qstring.h>

#include <functional>
#include <vector>
#include <map>

class QLabel;
class QWidget;
class QGridLayout;
class QComboBox;
class QPushButton;


namespace Ubpa {
	class Image;

	class Grid : public HeapObj {
	public:
		Grid() : isInit(false), page(nullptr), gridLayout(nullptr) { }
		Grid(QWidget * page);

	public:
		static const Ptr<Grid> New() { return Ubpa::New<Grid>(); }
		static const Ptr<Grid> New(QWidget * page) { return Ubpa::New<Grid>(page); }

	protected:
		virtual ~Grid() = default;

	public:
		void Init(QWidget * page);

		// spinbox
		void AddEditVal(const std::string & text, double val, double singleStep, const std::function<void(double)> & slot);
		template <typename numT>
		void AddEditVal(const std::string & text, volatile numT & val, double singleStep) {
			AddEditVal(text, val, singleStep, [&val](double v) {val = v; });
		}
		void AddEditVal(const std::string & text, int val, const std::function<void(int)> & slot);
		void AddEditVal(const std::string & text, volatile int & val) {
			AddEditVal(text, val, [&val](int v) {val = v; });
		}
		void AddEditVal(const std::vector<std::string> & texts, const Ubpa::valf3 & val, const Ubpa::valf3 & singleStep, const std::function<void(const Ubpa::valf3 &)> & slot);
		void AddEditVal(const std::vector<std::string> & texts, Ubpa::valf3 & val, const Ubpa::valf3 & singleStep) {
			AddEditVal(texts, val, singleStep, [&](const Ubpa::valf3 & newVal) {val = newVal; });
		}

		// slider
		void AddEditVal(const std::string & text, double val, double minVal , double maxVal, int stepNum, const std::function<void(double)> & slot);
		void AddEditVal(const std::string & text, int val, int minVal, int maxVal, const std::function<void(int)> & slot);
		void AddEditVal(const std::string & text, volatile int & val, int minVal, int maxVal) {
			AddEditVal(text, val, minVal, maxVal, [&val](int v) {val = v; });
		}
		template <typename numT>
		void AddEditVal(const std::string & text, volatile numT & val, double minVal, double maxVal, int stepNum) {
			AddEditVal(text, val, minVal, maxVal, stepNum, [&val](double v) {val = v; });
		}

		void AddEditVal(const std::vector<std::string> & texts, const Ubpa::valf3 & val, const Ubpa::valf3 & minVal, const Ubpa::valf3 & maxVal, const Ubpa::vali3 & stepNum, const std::function<void(const Ubpa::valf3 &)> & slot);
		void AddEditVal(const std::vector<std::string> & texts, Ubpa::valf3 & val, const Ubpa::valf3 & minVal, const Ubpa::valf3 & maxVal, const Ubpa::vali3 & stepNum) {
			AddEditVal(texts, val, minVal, maxVal, stepNum, [&](const Ubpa::valf3 & newVal) {val = newVal; });
		}

		// checkbox
		void AddEditVal(const std::string & text, volatile bool & val);

		// text
		void AddText(const std::string & left, const std::string & right);
		void AddText(const std::string & text);
		template <typename numT>
		void AddText(const std::string & text, numT val) { AddText(text, QString::number(val).toStdString()); }

		// color : [0, 1] x 3
		void AddEditColor(const std::string & text, Ubpa::rgbf & color);

		// combobox
		typedef std::map<std::string, std::function<void()>> SlotMap;
		typedef std::shared_ptr<std::map<std::string, std::function<void()>>> pSlotMap;
		typedef std::weak_ptr<std::map<std::string, std::function<void()>>> wpSlotMap;

		bool AddComboBox(QComboBox * combobox, const std::string & text, const std::string & curText, pSlotMap slotMap);
		bool AddComboBox(const std::string & text, const std::string & curText, pSlotMap slotMap);

		void AddComboBox(QComboBox * combobox, const std::string & text, const std::vector<std::string> & items,
			const std::function<void(const std::string &)> & slot);
		void AddComboBox(const std::string & text, const std::vector<std::string> & items,
			const std::function<void(const std::string &)> & slot);
		void AddComboBox(const std::string & text, const std::vector<std::string> & items, const std::string & btnText,
			const std::function<void(const std::string &)> & slot);

		// image
		void AddEditImage(const std::string & text, PtrC<Image> img, const std::function<void(Ptr<Image>)> & slot);

		// image
		void AddEditImage(const std::string & text, Ptr<Image> & img) {
			AddEditImage(text, img, [&](Ptr<Image> newImg) {img = newImg; });
		}

		void AddButton(const std::string & text, const std::function<void()> & slot);

		// clear and delete
		void Clear();

		// line
		void AddLine();
		void AddTitle(const std::string & text);
	private:
		void AddRow(const std::string & text, QWidget * widget = nullptr);
		void AddRow(QWidget * widgetLeft, QWidget * widgetRight = nullptr);

		static bool SetImgLabel(QLabel * imgLabel, PtrC<Image> img);
		static void ClearImgLabel(QLabel * imgLabel);

	private:
		bool isInit;
		QWidget * page;
		QGridLayout * gridLayout;
	};
}

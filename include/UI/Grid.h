#ifndef _UI_GRID_H_
#define _UI_GRID_H_

#include <CppUtil/Basic/HeapObj.h>

#include <CppUtil/Basic/RGB.h>

#include <CppUtil/Basic/Val2.h>
#include <CppUtil/Basic/Val3.h>
#include <CppUtil/Basic/Val4.h>

#include <qstring.h>

#include <functional>
#include <vector>
#include <map>

class QLabel;
class QWidget;
class QGridLayout;
class QComboBox;

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace Ui {
	class Grid : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Grid)
	public:
		Grid() : isInit(false), page(nullptr), gridLayout(nullptr) { }
		Grid(QWidget * page);

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
		void AddEditVal(const std::vector<std::string> & texts, const CppUtil::Val3 & val, const CppUtil::Val3 & singleStep, const std::function<void(const CppUtil::Val3 &)> & slot);
		void AddEditVal(const std::vector<std::string> & texts, CppUtil::Val3 & val, const CppUtil::Val3 & singleStep) {
			AddEditVal(texts, val, singleStep, [&](const CppUtil::Val3 & newVal) {val = newVal; });
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

		void AddEditVal(const std::vector<std::string> & texts, const CppUtil::Val3 & val, const CppUtil::Val3 & minVal, const CppUtil::Val3 & maxVal, const CppUtil::Val3i & stepNum, const std::function<void(const CppUtil::Val3 &)> & slot);
		void AddEditVal(const std::vector<std::string> & texts, CppUtil::Val3 & val, const CppUtil::Val3 & minVal, const CppUtil::Val3 & maxVal, const CppUtil::Val3i & stepNum) {
			AddEditVal(texts, val, minVal, maxVal, stepNum, [&](const CppUtil::Val3 & newVal) {val = newVal; });
		}

		// checkbox
		void AddEditVal(const std::string & text, volatile bool & val);

		// text
		void AddText(const std::string & left, const std::string & right);
		void AddText(const std::string & text);
		template <typename numT>
		void AddText(const std::string & text, numT val) { AddText(text, QString::number(val).toStdString()); }

		// color : [0, 1] x 3
		void AddEditColor(const std::string & text, CppUtil::RGBf & color);

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
		void AddEditImage(const std::string & text, CppUtil::Basic::CPtr<CppUtil::Basic::Image> img, const std::function<void(CppUtil::Basic::Ptr<CppUtil::Basic::Image>)> & slot);

		// image
		void AddEditImage(const std::string & text, CppUtil::Basic::Ptr<CppUtil::Basic::Image> & img) {
			AddEditImage(text, img, [&](CppUtil::Basic::Ptr<CppUtil::Basic::Image> newImg) {img = newImg; });
		}

		// clear and delete
		void Clear();

		// line
		void AddLine();
		void AddTitle(const std::string & text);
	private:
		void AddRow(const std::string & text, QWidget * widget = nullptr);
		void AddRow(QWidget * widgetLeft, QWidget * widgetRight = nullptr);

		static bool SetImgLabel(QLabel * imgLabel, CppUtil::Basic::CPtr<CppUtil::Basic::Image> img);
		static void ClearImgLabel(QLabel * imgLabel);

	private:
		bool isInit;
		QWidget * page;
		QGridLayout * gridLayout;
	};
}

#endif//!_UI_GRID_H_

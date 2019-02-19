#ifndef _UI_GRID_H_
#define _UI_GRID_H_

#include <CppUtil/Basic/HeapObj.h>

#include <qgridlayout.h>
#include <QtWidgets/QDoubleSpinBox>
#include <qcombobox.h>

#include <glm/vec3.hpp>

#include <functional>
#include <map>

class QLabel;

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace Ui {
	class Grid : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Grid)
	public:
		Grid(QWidget * page, QGridLayout * gridLayout)
			:page(page), gridLayout(gridLayout) { }

		// spinbox
		void AddEditVal(const std::string & text, double val, double singleStep, const std::function<void(double)> & slot);
		template <typename numT>
		void AddEditVal(const std::string & text, numT & val, double singleStep) {
			AddEditVal(text, val, singleStep, [&val](double v) {val = v; });
		}

		// slider
		void AddEditVal(const std::string & text, double val, double minVal , double maxVal, int stepNum, const std::function<void(double)> & slot);
		void AddEditVal(const std::string & text, int val, int minVal, int maxVal, const std::function<void(int)> & slot) {
			AddEditVal(text, val, minVal, maxVal, maxVal - minVal, slot);
		}
		template <typename numT>
		void AddEditVal(const std::string & text, numT & val, double minVal, double maxVal, int stepNum) {
			AddEditVal(text, val, minVal, maxVal, stepNum, [&val](double v) {val = v; });
		}

		// text
		void AddText(const std::string & left, const std::string & right);
		void AddText(const std::string & text);
		template <typename numT>
		void AddText(const std::string & text, numT val) { AddText(text, QString::number(val).toStdString()); }

		// color : [0, 1] x 3
		void AddEditColor(const std::string & text, glm::vec3 & color);

		// combobox
		typedef std::map<std::string, std::function<void()>> SlotMap;
		typedef std::shared_ptr<std::map<std::string, std::function<void()>>> pSlotMap;
		bool AddComboBox(QComboBox * combobox, const std::string & text, const std::string & curText, pSlotMap slotMap);
		bool AddComboBox(const std::string & text, const std::string & curText, pSlotMap slotMap) {
			return AddComboBox(new QComboBox, text, curText, slotMap);
		}

		// image
		void AddEditImage(const std::string & text, CppUtil::Basic::CPtr<CppUtil::Basic::Image> img, const std::function<void(CppUtil::Basic::Ptr<CppUtil::Basic::Image>)> & slot);

		// clear and delete
		void Clear();
	private:
		void AddLine(const std::string & text, QWidget * widget);
		void AddLine(QWidget * widgetLeft, QWidget * widgetRight = nullptr);

		static bool SetImgLabel(QLabel * imgLabel, CppUtil::Basic::CPtr<CppUtil::Basic::Image> img);
		static void ClearImgLabel(QLabel * imgLabel);

	private:
		QWidget * page;
		QGridLayout * gridLayout;
	};
}

#endif//!_UI_GRID_H_

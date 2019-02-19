#ifndef _UI_GRID_H_
#define _UI_GRID_H_

#include <qgridlayout.h>
#include <QtWidgets/QDoubleSpinBox>

#include <glm/vec3.hpp>

#include <functional>

namespace Ui {
	class Grid{
	public:
		Grid(QWidget * page, QGridLayout * gridLayout)
			:page(page), gridLayout(gridLayout) { }

		void AddTitle(const QString & title);

		// spinbox
		void AddEditVal(const QString & text, double val, double singleStep, const std::function<void(double)> & slot);
		template <typename numT>
		void AddEditVal(const QString & text, numT & val, double singleStep) {
			AddEditVal(text, val, singleStep, [&val](double v) {val = v; });
		}

		// slider
		void AddEditVal(const QString & text, double val, double minVal , double maxVal, int stepNum, const std::function<void(double)> & slot);
		void AddEditVal(const QString & text, int val, int minVal, int maxVal, const std::function<void(int)> & slot) {
			AddEditVal(text, val, minVal, maxVal, maxVal - minVal, slot);
		}
		template <typename numT>
		void AddEditVal(const QString & text, numT & val, double minVal, double maxVal, int stepNum) {
			AddEditVal(text, val, minVal, maxVal, stepNum, [&val](double v) {val = v; });
		}

		// textlabel
		void AddShowText(const QString & left, const QString & right);
		
		template <typename numT>
		void AddShowVal(const QString & text, numT val) { AddShowText(text, QString::number(val)); }

		// color : [0, 1] x 3
		void AddEditColor(const QString & text, glm::vec3 & color);

	private:
		void AddLine(const QString & text, QWidget * widget);
		void AddLine(QWidget * widgetLeft, QWidget * widgetRight);

	private:
		QWidget * page;
		QGridLayout * gridLayout;
	};
}

#endif//!_UI_GRID_H_

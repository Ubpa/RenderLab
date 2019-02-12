#ifndef _UI_GRID_H_
#define _UI_GRID_H_

#include <qgridlayout.h>
#include <QtWidgets/QDoubleSpinBox>

#include <functional>

namespace Ui {
	class Grid{
	public:
		Grid(QWidget * page, QGridLayout * gridLayout)
			:page(page), gridLayout(gridLayout) { }

		void AddTitle(const QString & title);
		QDoubleSpinBox * AddLine(const QString & text, double val, double singleStep, const std::function<void(double)> & slot);

	private:
		QWidget * page;
		QGridLayout * gridLayout;
	};
}

#endif//!_UI_GRID_H_

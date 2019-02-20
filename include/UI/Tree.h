#ifndef _UI_HIERARCHY_TREE_H_
#define _UI_HIERARCHY_TREE_H_

#include <qtreewidget.h>

namespace Ui {
	class Tree : public QTreeWidget {
		Q_OBJECT
	public:
		explicit Tree(QWidget *parent = nullptr);

		//virtual void mousePressEvent(QMouseEvent *event);
		//virtual void mouseMoveEvent(QMouseEvent *event);

		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dragLeaveEvent(QDragLeaveEvent *event);
		virtual void dragMoveEvent(QDragMoveEvent *event);
		virtual void dropEvent(QDropEvent *event);
	private:

	};
}

#endif//!_UI_HIERARCHY_TREE_H_

#pragma once

#include <qtreewidget.h>

namespace Ubpa {
	class Tree : public QTreeWidget {
		Q_OBJECT
	public:
		explicit Tree(QWidget *parent = nullptr);

	public: //event
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);

		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dragLeaveEvent(QDragLeaveEvent *event);
		virtual void dragMoveEvent(QDragMoveEvent *event);
		virtual void dropEvent(QDropEvent *event);

		virtual void contextMenuEvent(QContextMenuEvent *event);
	
	private slots:
		void on_tree_Hierarchy_itemClicked(QTreeWidgetItem *item, int column);
		void on_tree_Hierarchy_itemPressed(QTreeWidgetItem *item, int column);
		void on_tree_Hierarchy_itemEntered(QTreeWidgetItem *item, int column);
		void on_tree_Hierarchy_itemActivated(QTreeWidgetItem *item, int column);
		void on_tree_Hierarchy_itemDoubleClicked(QTreeWidgetItem *item, int column);
		void on_tree_Hierarchy_itemSelectionChanged();
		void on_tree_Hierarchy_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void on_tree_Hierarchy_itemChanged(QTreeWidgetItem *item, int column);

	private:
		bool canDrag;
		QTreeWidgetItem * needDragItem;
	};
}

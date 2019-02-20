#include <UI/Tree.h>

#include <UI/Hierarchy.h>
#include <CppUtil/Basic/GStorage.h>

#include <qevent.h>
#include <qmimedata.h>

using namespace CppUtil::Basic;
using namespace Ui;
using namespace std;

Tree::Tree(QWidget *parent)
	: QTreeWidget(parent){

	setAcceptDrops(true);
}
/*
void Tree::mousePressEvent(QMouseEvent *event) {
	printf("mouse press\n");
	QTreeWidget::mousePressEvent(event);
}
void Tree::mouseMoveEvent(QMouseEvent *event) {
	printf("mouse move\n");
	QTreeWidget::mouseMoveEvent(event);
}
*/

void Tree::dragEnterEvent(QDragEnterEvent *event) {
	//printf("drag enter\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	//QTreeWidget::dragEnterEvent(event);
}

void Tree::dragLeaveEvent(QDragLeaveEvent *event) {
	//printf("drag leave\n");
	event->accept();
	//QTreeWidget::dragLeaveEvent(event);
}

void Tree::dragMoveEvent(QDragMoveEvent *event) {
	//printf("drag move\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	auto mE = new QMouseEvent(QEvent::MouseMove, event->posF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	mouseMoveEvent(mE);
	delete mE;
	//QTreeWidget::dragMoveEvent(event);
}

void Tree::dropEvent(QDropEvent *event) {
	//printf("drop\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	
	string key = event->mimeData()->text().toStdString();
	//printf("text: %s\n", key.c_str());

	QTreeWidgetItem * item;
	GS::GetV(key, item);
	GS::UnReg<QTreeWidgetItem *>(key);

	Hierarchy::GetInstance()->Move(item, currentItem());
	//QTreeWidget::dropEvent(event);
}

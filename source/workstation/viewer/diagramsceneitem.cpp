#include "diagramsceneitem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
DiagramSceneItem::DiagramSceneItem(QMenu *itemMenu, DiagramType diagramType, DiagramScene* parent) :
DiagramScene(itemMenu),
DiagramItem(diagramType,itemMenu),
parent_(parent)
{
}

void DiagramSceneItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
		emit opened(this);
	else if(event->button() == Qt::RightButton)
		emit opened(parent_);
}

//void DiagramSceneItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
//{
//	int i=5;
//	i++;
//}
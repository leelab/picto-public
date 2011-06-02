#include "ArrowPortItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
ArrowPortItem::ArrowPortItem(QString name,QGraphicsItem *parent, QGraphicsScene *scene) :
DiagramItem(NULL,name,parent,scene)
{
}

void ArrowPortItem::removeDependantGraphics()
{
	removeArrows();
}

void ArrowPortItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

void ArrowPortItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

void ArrowPortItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        static_cast<ArrowPortItem*>(arrow->startItem())->removeArrow(arrow);
        static_cast<ArrowDestinationItem*>(arrow->endItem())->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

void ArrowPortItem::updateDependantGraphics()
{
    foreach (Arrow *arrow, arrows) 
	{
        arrow->updatePosition();
    }
}
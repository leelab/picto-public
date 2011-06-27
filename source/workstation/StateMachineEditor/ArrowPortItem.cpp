#include "ArrowPortItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
ArrowPortItem::ArrowPortItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene) :
DiagramItem(editorState,NULL,name,parent,scene)
{
}

ArrowPortItem::~ArrowPortItem()
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
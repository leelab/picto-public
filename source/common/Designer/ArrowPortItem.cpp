#include "ArrowPortItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
ArrowPortItem::ArrowPortItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent,QSharedPointer<Asset> asset) :
DiagramItem(editorState,NULL,name,parent)
{
	asset_ = asset;
	QPen invisiblePen;
	invisiblePen.setWidth(0);
	invisiblePen.setColor(QColor(0,0,0,0));
	setPen(invisiblePen);
	//setFlag(QGraphicsItem::ItemIsSelectable,false);
	setFlag(QGraphicsItem::ItemIsMovable,false);
	//setCursor(Qt::ArrowCursor);
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

void ArrowPortItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event)
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		editorState_->setEditMode(EditorState::Select);
		//setCursor(Qt::ArrowCursor);
	}
}

void ArrowPortItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		editorState_->setEditMode(EditorState::MoveItem);
		//setCursor(Qt::ArrowCursor);
	}
}
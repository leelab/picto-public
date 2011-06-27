#include "WireableItem.h"

#include "../../common/controlelements/controlelement.h"
#include "ArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "arrow.h"
#include <QGraphicsScene>
WireableItem::WireableItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
	arrowDest_ = NULL;
	lastSourcePos_ = 0;
}

WireableItem::~WireableItem()
{
}

void WireableItem::addArrowSource(QString name)
{
	DiagramItem* newArrowSource = new ArrowSourceItem(name,getEditorState(),this,scene());
	newArrowSource->setPos(QPointF(boundingRect().width()/2.0,lastSourcePos_));
	lastSourcePos_ += newArrowSource->boundingRect().height();
	newArrowSource->setZValue(zValue()+1);
	arrowSources_.push_back(newArrowSource);
}

void WireableItem::enableArrowDest()
{
	if(arrowDest_)
		return;
	arrowDest_ = new ArrowDestinationItem("",getEditorState(),this,scene());
	arrowDest_->setPos(QPointF(-boundingRect().width()/2.0+arrowDest_->boundingRect().width(),boundingRect().height()/2.0));
	arrowDest_->setZValue(zValue()+1);	
}

QList<DiagramItem*> WireableItem::getArrowSources()
{
	return arrowSources_;
}

DiagramItem* WireableItem::getArrowDest()
{
	return arrowDest_;
}
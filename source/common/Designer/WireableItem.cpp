#include "WireableItem.h"

#include "../../common/controlelements/controlelement.h"
#include "ResultArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
WireableItem::WireableItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
	arrowDest_ = NULL;
	maxArrowSourceWidth_ = 0;
}

WireableItem::~WireableItem()
{
}

void WireableItem::addResultArrowSource(QSharedPointer<Asset> sourceAsset)
{
	Q_ASSERT(sourceAsset);
	DiagramItem* newArrowSource = new ResultArrowSourceItem(sourceAsset->getName(),getEditorState(),this,sourceAsset);
	newArrowSource->setZValue(zValue()+1);
	if(newArrowSource->getWidth() > maxArrowSourceWidth_)
		maxArrowSourceWidth_ = newArrowSource->getWidth();
	if(maxArrowSourceWidth_ > getRect().width())
		setWidth(maxArrowSourceWidth_);
	arrowSources_.push_back(newArrowSource);
	updateArrowPortDimensions();
}

void WireableItem::enableArrowDest()
{
	if(arrowDest_)
		return;
	arrowDest_ = new ArrowDestinationItem(getEditorState(),this,getAsset());
	arrowDest_->setZValue(zValue()+1);
	arrowDest_->setWidth(10);
	updateArrowPortDimensions();	
}

QList<DiagramItem*> WireableItem::getArrowSources()
{
	return arrowSources_;
}

DiagramItem* WireableItem::getArrowDest()
{
	return arrowDest_;
}

void WireableItem::setRect(QRectF rect)
{
	if(rect.width() < maxArrowSourceWidth_)
		rect.setWidth(maxArrowSourceWidth_);
	AssetItem::setRect(rect);
	updateArrowPortDimensions();
}

void WireableItem::updateArrowPortDimensions()
{
	float width = getWidth();
	QPointF lastSourceOffset = QPointF(0,0);
	bool keepTrying = true;
	while(keepTrying)
	{
		keepTrying = false;
		foreach(DiagramItem* source, arrowSources_)
		{
			source->setWidth(width);
			source->setPos(getRect().bottomLeft()+lastSourceOffset);
			lastSourceOffset = lastSourceOffset + QPointF(0,source->boundingRect().height());
		}
	}
	float height = getHeight()+lastSourceOffset.y();
	if(arrowDest_)
	{
		arrowDest_->setPos(getRect().topLeft()-QPointF(arrowDest_->getWidth(),0));
		arrowDest_->setHeight(height);
	}
}
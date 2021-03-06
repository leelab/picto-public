#include "WireableItem.h"

#include "../../common/controlelements/controlelement.h"
#include "ResultArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new WireableItem.  By default no attached start bar is included.*/
WireableItem::WireableItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
ExperimentItem(editorState,contextMenu,asset)
{
	arrowDest_ = NULL;
	maxArrowSourceWidth_ = 0;
}

WireableItem::~WireableItem()
{
}

/*! \brief Adds a ResultArrowSourceItem hanging off of the bottom of this WireableItem to
 *	represent a child result from which control flow can continue on to another element.
 */
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

/*! \brief Adds an ArrowDestinationItem "start bar" to this Wireable item.
 *	\details Most StateMachineElements require "start bars" but ControlElements,
 *	for example, don't.
 */
void WireableItem::enableArrowDest()
{
	if(arrowDest_)
		return;
	arrowDest_ = new ArrowDestinationItem(getEditorState(),this,getAsset());
	arrowDest_->setZValue(zValue()+1);
	arrowDest_->setWidth(10);
	updateArrowPortDimensions();	
}

/*! \brief Returns all "arrow sources" in other words, all ResultArrowSourceItem objecs.
*/
QList<DiagramItem*> WireableItem::getArrowSources()
{
	return arrowSources_;
}

/*! \brief Returns this WireableItem's ArrowDestinationItem or NULL if it doesn't have one.*/
DiagramItem* WireableItem::getArrowDest()
{
	return arrowDest_;
}

void WireableItem::setRect(QRectF rect)
{
	if(rect.width() < maxArrowSourceWidth_)
		rect.setWidth(maxArrowSourceWidth_);
	ExperimentItem::setRect(rect);
	updateArrowPortDimensions();
}

/*! \brief Updates the dimensions of all of the attached ArrowPortItem objects according 
 *	to this WireableItem's current size.
 */
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
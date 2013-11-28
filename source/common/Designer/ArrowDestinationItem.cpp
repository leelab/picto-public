#include <QGraphicsScene>
#include "ArrowDestinationItem.h"
#include "ArrowSourceItem.h"
#include "Arrow.h"
#include "../storage/DataStore.h"
#include "../../common/memleakdetect.h"
ArrowDestinationItem::ArrowDestinationItem(QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> asset) :
ArrowPortItem("",editorState,parent,asset)
{
	scriptItemManager_ = QSharedPointer<ScriptItemManager>(new ScriptItemManager(editorState,this,asset,false));
}

/*! \brief The idea of this function was to define a point where the arrow should contact it.
 *	in practice, this function is not used.
 */
QPointF ArrowDestinationItem::getContactPoint()
{
	return getRect().center()-QPointF(getRect().width()/2.0,0);
}

void ArrowDestinationItem::setRect(QRectF rect)
{
	ArrowPortItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	int alpha = editorState_->getCurrentAnalysis().isNull()?255:100;
	grad.setColorAt(0,QColor(0,210,0,alpha));
	grad.setColorAt(1,QColor(0,130,0,alpha));
	QBrush brush(grad);
	setBrush(brush);

	scriptItemManager_->setScriptBoundingRect(rect);
}
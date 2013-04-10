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

QPointF ArrowDestinationItem::getContactPoint()
{
	return getRect().center()-QPointF(getRect().width()/2.0,0);
}

void ArrowDestinationItem::setRect(QRectF rect)
{
	ArrowPortItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(0,210,0));
	grad.setColorAt(1,QColor(0,130,0));
	QBrush brush(grad);
	setBrush(brush);

	scriptItemManager_->setScriptBoundingRect(rect);
}
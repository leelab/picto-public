#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

#include "ResultArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include "../../common/memleakdetect.h"
ResultArrowSourceItem::ResultArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset) :
ArrowSourceItem(name,editorState,parent,windowAsset)
{
	scriptItemManager_ = QSharedPointer<ScriptItemManager>(new ScriptItemManager(editorState,this,windowAsset,true));
}

void ResultArrowSourceItem::setRect(QRectF rect)
{
	ArrowSourceItem::setRect(rect);
	scriptItemManager_->setScriptBoundingRect(rect);
}
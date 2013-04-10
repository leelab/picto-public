#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

#include "StartBarItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include "../../common/memleakdetect.h"
StartBarItem::StartBarItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset) :
ArrowSourceItem(name,editorState,parent,windowAsset)
{
	scriptItemManager_ = QSharedPointer<ScriptItemManager>(new ScriptItemManager(editorState,this,windowAsset,false));
	setPos(QPointF(0,0));
	setWidth(20);
	setHeight(4000);
}

QPointF StartBarItem::getContactPoint()
{
	return getRect().center()+QPointF(getRect().width()/2.0,0);
}

void StartBarItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	//Set the script manager's rectangle according to the visible part of the scene in the view.
	updateForViewportChanges();
	ArrowSourceItem::paint(painter,option,widget);
}

void StartBarItem::setRect(QRectF rect)
{
	ArrowSourceItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(0,210,0));
	grad.setColorAt(1,QColor(0,130,0));
	QBrush brush(grad);
	setBrush(brush);
	
	//Set the script manager's rectangle according to the visible part of the scene in the view.
	updateForViewportChanges();
}

void StartBarItem::updateForViewportChanges()
{
	//Set the script manager's rectangle according to the visible part of the scene in the view.
	QGraphicsScene* myScene = scene();
	if(!myScene)
		return;
	Q_ASSERT(myScene->views().size() == 1);
	QGraphicsView* myView = myScene->views().first();
	//QPointF topLeftScenePointVisible = myView->mapToScene(QPoint(0,0));
	//QPointF sceneSizeVisible = QPoint(myView->viewport()->width(),myView->viewport()->height());
	//QRectF newScriptRect = getRect();
	//newScriptRect.setTop(topLeftScenePointVisible.y());
	//newScriptRect.setHeight(sceneSizeVisible.y());

	QPointF topLeftViewportPoint(myView->horizontalScrollBar()->value(), myView->verticalScrollBar()->value());
	QPointF bottomRightViewportPoint = topLeftViewportPoint + myView->viewport()->rect().bottomRight();
	QTransform sceneToViewTransform = myView->transform();
	QTransform viewToSceneTransform = sceneToViewTransform.inverted();
	QRectF visibleSceneRect = viewToSceneTransform.mapRect(QRectF(topLeftViewportPoint,bottomRightViewportPoint));
	QRectF newScriptRect = getRect();
	newScriptRect.setTop(visibleSceneRect.top());
	newScriptRect.setBottom(visibleSceneRect.bottom());
	if(scriptRect_ == newScriptRect)
	{
		return;
	}
	scriptRect_ = newScriptRect;
	scriptItemManager_->setScriptBoundingRect(scriptRect_);
	update();
}
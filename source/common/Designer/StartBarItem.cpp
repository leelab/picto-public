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
/*! \brief The idea of this function was to define a point where the arrow should contact it.
 *	in practice, this function is not used.
 */
QPointF StartBarItem::getContactPoint()
{
	return getRect().center()+QPointF(getRect().width()/2.0,0);
}

/*! \brief Wheneverr this item is repainted, we need to update the ScriptItemManager as to the current visible canvas region.
 *	so that the scripts will all show up on screen.  This calls updateForViewportChanges() to do that.
 */
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
	int alpha = editorState_->getCurrentAnalysis().isNull()?255:100;
	grad.setColorAt(0,QColor(0,210,0,alpha));
	grad.setColorAt(1,QColor(0,130,0,alpha));
	QBrush brush(grad);
	setBrush(brush);
	
	//Set the script manager's rectangle according to the visible part of the scene in the view.
	updateForViewportChanges();
}

/*! \brief Reads the position of the top and bottom areas of the displayed canvas and updates the ScriptItemManager accordingly so that
 *	all scripts will appear on screen.
 */
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
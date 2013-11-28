#include <QtWidgets>
#include <QGraphicsOpacityEffect>

#include "arrow.h"
#include <math.h>
#include "ArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "StartBarItem.h"
#include "../../common/storage/datastore.h"
#include "../../common/statemachine/MachineContainer.h"
#include "AssetItem.h"
#include "../../common/memleakdetect.h"

const qreal Pi = 3.14;

/*! \brief Constructs a new Arrow that represents the input transition, starts at startItem and ends at endItem.
 *	\details The input contextMenu will be displayed when the transition is "right clicked".  editorState
 *	is used to retrieve various information about the current state of the designer system.
 *	This constructor is private because Arrows should be created with the create() functions.
 *	\note Arrow's are always drawn with a ZValue that is higher than both of the DiagramItems to which it is
 *	attached so that it will always be visible on top of them.
 */
Arrow::Arrow(QSharedPointer<EditorState> editorState, QSharedPointer<Asset> transition, DiagramItem *startItem, DiagramItem *endItem, QMenu *contextMenu,
         QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
	editorState_ = editorState;
	transition_ = transition;
    myStartItem = static_cast<ArrowPortItem*>(startItem);
    myEndItem = static_cast<ArrowPortItem*>(endItem);
	myContextMenu = contextMenu;
	myStartItem->addArrow(this);
	myEndItem->addArrow(this);
	int maxZVal = (startItem->zValue()>endItem->zValue())?startItem->zValue():endItem->zValue();
	setZValue(maxZVal);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
	QPen pen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	pen.setCosmetic(true);
    setPen(pen);
	setGraphicsEffect(new QGraphicsOpacityEffect());
	qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())->setOpacity(1.0);
}

/*! \brief When the Arrow is deleted, it needs to be removed from the QGraphicsScene.  This takes care of that.
*/
Arrow::~Arrow()
{
	QGraphicsScene* scenePtr = scene();
	if(scenePtr)
		scenePtr->removeItem(this);
}
/*! \brief Creates a new arrow representing a transition from one StateMachineElement to another within the current
	*	windowAsset.  startItem and endItem must be passed in because the Design assets don't have pointers to their
	*	corresponding UI elements or the transitions to which they are connected.
	*	The contextMenu passed in here is the one that will be displayed if the arrow is "right clicked."
	* \sa DiagramScene::insertTransition()
	*/
Arrow* Arrow::Create(QSharedPointer<EditorState> editorState, QSharedPointer<Transition> transition, DiagramItem *startItem, DiagramItem *endItem, 
		QMenu *contextMenu, QGraphicsItem *parent)
{
	if(!dynamic_cast<ArrowSourceItem*>(startItem) || !dynamic_cast<ArrowDestinationItem*>(endItem))
		return NULL;
	if(transition.isNull())
		return NULL;
	return new Arrow(editorState, transition,startItem,endItem,contextMenu,parent);
}

/*! \brief Creates a new arrow representing a transition from the current windowAsset's start bar to an element within the 
 *	windowAsset.
 *	The contextMenu passed in here is the one that will be displayed if the arrow is "right clicked."
 * \sa DiagramScene::insertTransition()
 */
Arrow* Arrow::Create(QSharedPointer<EditorState> editorState, QSharedPointer<Asset> windowAsset, DiagramItem *startItem, DiagramItem *endItem, 
		QMenu *contextMenu, QGraphicsItem *parent)
{
	if(!dynamic_cast<ArrowSourceItem*>(startItem) || !dynamic_cast<ArrowDestinationItem*>(endItem))
		return NULL;
	if(windowAsset.isNull() || windowAsset.dynamicCast<MachineContainer>().isNull())
		return NULL;

	//Find Asset Item ancestors
	QSharedPointer<Asset> source = getAssetAncestor(startItem);
	QSharedPointer<Asset> result = static_cast<ArrowSourceItem*>(startItem)->getAsset();
	QSharedPointer<Asset> dest = getAssetAncestor(endItem);
	if(dynamic_cast<StartBarItem*>(startItem))
		source = windowAsset;
	if(source.isNull() || dest.isNull())
		return NULL;
	QSharedPointer<Transition> newTrans;
	if(source == windowAsset)
		newTrans = QSharedPointer<Transition>(new Transition(QSharedPointer<Asset>(),QSharedPointer<Asset>(),dest));
	else
		newTrans = QSharedPointer<Transition>(new Transition(source,result,dest));
	if(!windowAsset.staticCast<MachineContainer>()->addTransition(newTrans))
		return NULL;
	return new Arrow(editorState, newTrans,startItem,endItem,contextMenu,parent);
}

/*! \brief Returns a QRectF that contains this Arrow's dimensions within its bounds.*/
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

/*! \brief Returns the shape of this Arrow as a QPainterPath in local coordinates. 
 *	\details The shape is used by the QGraphicsItem system for many things, including 
 *	collision detection, hit tests, and for the QGraphicsScene::items() functions.
 */
QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

/*! \brief Updates the line according to the current position of the start and end DiagramItem objects
 *	to which it is attached.*/
void Arrow::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

/*! \brief Draws the Arrow on screen from its start DiagramItem to its end DiagramItem.
 *	\details This function includes all of the detailed calculations to figure out where
 *	exactly on the start DiagramItem the arrow should start, where on the end DiagramItem
 *	the arrow should end, how to draw the arrow head at the necessary angle, etc.  It is
 *	called by the Qt QGraphicsItem system whenever the arrow needs to be redrawn.
 *	\note It is this code that creates the arrow's sliding effect causing it to be able to
 *	slide up and down the DiagramItem of its destination element and its canvas start bar.
 */
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    //if (myStartItem->collidesWithItem(myEndItem))
    //    return;
	QPointF startPos = mapFromItem(myStartItem,0,0)+QPointF(myStartItem->getWidth(),myStartItem->getHeight()/2.0);
	bool startItemIsStartBar = myStartItem->getHeight() > 1000;
	if(startItemIsStartBar)	//Its a start bar
		startPos.setY(0);  //Do calculations as if we're starting from top corner.  Later we'll update startPos for a horizontal line
	//Find the end point with the nearest Y value.
	QPointF endTop = mapFromItem(myEndItem,0,0);
	QPointF endBottom = mapFromItem(myEndItem,0,myEndItem->boundingRect().height());
	QPointF endPos;
	if((startPos.y() >= endTop.y()) && (startPos.y() <= endBottom.y()) )
	{ 
		endPos = endTop;
		endPos.setY(startPos.y());
	}
	else if(startPos.y() < endTop.y())
	{
		endPos = endTop;
	}
	else
	{
		endPos = endBottom;
	}
	//endPos = mapFromItem(myEndItem,0,0)+QPointF(0,myStartItem->myEndItem()/2.0);
	if(startPos == endPos)
	{
		updatePosition();
		return;
	}

	if(startItemIsStartBar)	//If start item is a start bar, make the arrow start at the same height as it stops
		startPos.setY(endPos.y());

    QPen myPen = pen();
	QColor finalColor(myColor);
	//If there's an active analysis, grey out the arrow
	qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())->setOpacity(1.0);
	if(!editorState_->getCurrentAnalysis().isNull())
		qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())->setOpacity(100.0/255.0);
    myPen.setColor(finalColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(finalColor);


    QLineF centerLine(startPos, endPos);
    QPolygonF endPolygon = myEndItem->polygon();
    QPointF p1 = endPolygon.first() + endPos;
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < endPolygon.count(); ++i) {
		p2 = endPolygon.at(i) + endPos;
		polyLine = QLineF(p1, p2);
		QLineF::IntersectType intersectType =
			polyLine.intersect(centerLine, &intersectPoint);
		if (intersectType == QLineF::BoundedIntersection)
			break;
		p1 = p2;
    }
	if(p1 == p2)
	{
		updatePosition();
		return;
	}

    setLine(QLineF(intersectPoint, startPos));


    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;

        painter->drawLine(line());
        painter->drawPolygon(arrowHead);
        if (isSelected()) {
            painter->setPen(QPen(finalColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}

/*! \brief Shows the context menu passed into the constructor whenever this Arrow is right clicked.
*/
void Arrow::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

/*! \brief Reimplements various selection funtionality.
 *	\details If multiple items are selected at once in the scene, for example, the last one
 *	selected ends up being the one whos properties are shown in the right hand property widget.
 *	If this arrow is the last one selected, we reset the "selected item" to NULL, so that the
 *	WindowAsset's information will be displayed.
 */
QVariant Arrow::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
	QVariant inputVal = value;
	if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		if(inputVal.toBool())
		{
			editorState_->setSelectedItem(this);
		}
		else
		{
			//If this was the last selected item, set the selected item to NULL.  This
			//has the effect of switching the selected asset to the window asset.
			if(editorState_->getSelectedItem() == this)
				editorState_->setSelectedItem(NULL);
		}
	}
    return inputVal;
}

/*! \brief Returns the Asset associated with the input DiagramItem.
 *	\details Since the arrow isn't terminated at AssetItems but at ArrowPort items,
 *	this function allows us to retrieve the Asset objects associated with the Asset Items
 *	containing the ArrowPort items to which this arrow is attached.
 */
QSharedPointer<Asset> Arrow::getAssetAncestor(DiagramItem* item)
{
	QGraphicsItem* parent = item;
	while(parent && !dynamic_cast<AssetItem *>(parent))
		parent = parent->parentItem();
	if(!parent)
		return QSharedPointer<Asset>();
	return dynamic_cast<AssetItem *>(parent)->getAsset();
}
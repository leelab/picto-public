/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

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

//! [0]
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

Arrow::~Arrow()
{
	QGraphicsScene* scenePtr = scene();
	if(scenePtr)
		scenePtr->removeItem(this);
}

Arrow* Arrow::Create(QSharedPointer<EditorState> editorState, QSharedPointer<Transition> transition, DiagramItem *startItem, DiagramItem *endItem, 
		QMenu *contextMenu, QGraphicsItem *parent)
{
	if(!dynamic_cast<ArrowSourceItem*>(startItem) || !dynamic_cast<ArrowDestinationItem*>(endItem))
		return NULL;
	if(transition.isNull())
		return NULL;
	return new Arrow(editorState, transition,startItem,endItem,contextMenu,parent);
}

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
//! [0]

//! [1]
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}
//! [2]

//! [3]
void Arrow::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}
//! [3]

//! [4]
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
//! [4] //! [5]

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
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;
//! [6] //! [7]
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

void Arrow::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}
//! [7]

QSharedPointer<Asset> Arrow::getAssetAncestor(DiagramItem* item)
{
	QGraphicsItem* parent = item;
	while(parent && !dynamic_cast<AssetItem *>(parent))
		parent = parent->parentItem();
	if(!parent)
		return QSharedPointer<Asset>();
	return dynamic_cast<AssetItem *>(parent)->getAsset();
}
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

#include <QtGui>

#include "diagramitem.h"
#include "arrow.h"

//! [0]
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu, QString name,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene)
{
	arrowDest_ = NULL;
	lastSourcePos_ = 0;
    myDiagramType = diagramType;
    myContextMenu = contextMenu;
	nameText_ = new QGraphicsTextItem(this);
	nameText_->setZValue(1000.0);
    QPainterPath path;
    switch (myDiagramType) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
			nameText_->setPos(-100,-100);
			setPolygon(myPolygon);
            break;
        case Conditional:
            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                      << QPointF(100, 0) << QPointF(0, -100)
                      << QPointF(-100, 0);
			nameText_->setPos(0,0);
			setPolygon(myPolygon);
            break;
        case Step:
            myPolygon << QPointF(-100, -100) << QPointF(100, -100)
                      << QPointF(100, 100) << QPointF(-100, 100)
                      << QPointF(-100, -100);
			nameText_->setPos(-100,-100);
			setFlag(QGraphicsItem::ItemIsMovable, true);
			setFlag(QGraphicsItem::ItemIsSelectable, true);
			setPolygon(myPolygon);
            break;
        default:
            break;
    }
	setName(name);
}

void DiagramItem::setName(QString name)
{
	name_ = name;
	updateLabel();
}
void DiagramItem::setType(QString type)
{
	type_ = type;
	updateLabel();
}
QString DiagramItem::getName()
{
	return name_;
}
QString DiagramItem::getType()
{
	return type_;
}
void DiagramItem::updateLabel()
{
	QString text = getName();
	if((text != "") && (getType() != ""))
	{
		text.append("\n");
	}
	if(getType() != "")
	{
		text.append("(").append(getType()).append(")");
	}		
	nameText_->setPlainText(text);
	QRectF textbound = nameText_->boundingRect();
	QRectF polybound = polygon().boundingRect();
	bool needsToStretch = false;
	if(polybound.width() < textbound.width())
	{
		needsToStretch = true;
		polybound.setWidth(textbound.width());
	}
	if(polybound.height() < textbound.height())
	{
		needsToStretch = true;
		polybound.setHeight(textbound.height());
	}
	if(needsToStretch)
	{
		setPolygonFromRect(polybound);
		nameText_->setPos(QPointF(-polybound.width(),-polybound.height()));
	}
}
//! [0]

//! [1]
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}
//! [2]
void DiagramItem::setPolygonFromRect(QRectF rect)
{
	myPolygon.clear();
	myPolygon	<< QPointF(-rect.width(),-rect.height()) 
				<< QPointF(0,-rect.height()) 
				<< QPointF(0,0)
				<< QPointF(-rect.width(),0)
				<< QPointF(-rect.width(),-rect.height());
	setPolygon(myPolygon);
}
//! [3]
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
void DiagramItem::addArrowSource(QString name)
{
	DiagramItem* newArrowSource = new DiagramItem(ArrowSource,myContextMenu,name,this,scene());
	newArrowSource->setPos(QPointF(boundingRect().width()/2.0,lastSourcePos_));
	lastSourcePos_ += newArrowSource->boundingRect().height();
	newArrowSource->setZValue(zValue()+1);
	arrowSources_.push_back(newArrowSource);
}

void DiagramItem::enableArrowDest()
{
	if(arrowDest_)
		return;
	
	arrowDest_ = new DiagramItem(ArrowDestination,myContextMenu,"",this,scene());
	arrowDest_->setPos(QPointF(-boundingRect().width()/2.0+arrowDest_->boundingRect().width(),boundingRect().height()/2.0));
	arrowDest_->setZValue(zValue()+1);	
}

QList<DiagramItem*> DiagramItem::getArrowSources()
{
	return arrowSources_;
}

DiagramItem* DiagramItem::getArrowDest()
{
	return arrowDest_;
}
//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}
//! [5]

//! [6]
QVariant DiagramItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
    }
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		emit selectedChange(this);


    return value;
}
//! [6]
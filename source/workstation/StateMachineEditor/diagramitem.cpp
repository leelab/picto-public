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
DiagramItem::DiagramItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QString name,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene)
{
	editorState_ = editorState;
    myContextMenu = contextMenu;
	nameText_ = NULL;
	setName(name);
}

DiagramItem::~DiagramItem()
{}

void DiagramItem::setName(QString name,QPointF pos)
{
	name_ = name;
	updateLabel();
	if(pos != QPointF())
		nameText_->setPos(pos);
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
	if(!nameText_)
	{
		nameText_ = new QGraphicsTextItem(this);
		nameText_->setZValue(1000.0);
	}
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

//! [2]
void DiagramItem::setPolygonFromRect(QRectF rect)
{
	QPolygonF myPolygon;
	myPolygon	<< QPointF(-rect.width(),-rect.height()) 
				<< QPointF(0,-rect.height()) 
				<< QPointF(0,0)
				<< QPointF(-rect.width(),0)
				<< QPointF(-rect.width(),-rect.height());
	setPolygon(myPolygon);
}
//! [3]


//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(polygon());

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
		updateDependantGraphics();
		positionChanged(value.toPoint());		
    }
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		emit selectedChange(this);


    return value;
}
//! [6]
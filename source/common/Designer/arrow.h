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

#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>
#include "../../common/statemachine/Transition.h"
#include "ArrowPortItem.h"

QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

//! [0]
class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };
	static Arrow* Create(QSharedPointer<EditorState> editorState, QSharedPointer<Transition> transition, DiagramItem *startItem, DiagramItem *endItem, 
		QMenu *contextMenu, QGraphicsItem *parent);
	static Arrow* Create(QSharedPointer<EditorState> editorState, QSharedPointer<Asset> windowAsset, DiagramItem *startItem, DiagramItem *endItem, 
		QMenu *contextMenu, QGraphicsItem *parent);
	virtual ~Arrow();
    int type() const
        { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)
        { myColor = color; }
    DiagramItem *startItem() const
        { return myStartItem; }
    DiagramItem *endItem() const
        { return myEndItem; }
	QSharedPointer<Asset> getAsset(){return transition_;};


public slots:
    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
	Arrow(QSharedPointer<EditorState> editorState, QSharedPointer<Asset> transition, DiagramItem *startItem, DiagramItem *endItem, QMenu *contextMenu,
      QGraphicsItem *parent = 0);
	static QSharedPointer<Asset> getAssetAncestor(DiagramItem* item);
	QSharedPointer<Asset> transition_;
	QSharedPointer<EditorState> editorState_;
    ArrowPortItem *myStartItem;
    ArrowPortItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
	QMenu *myContextMenu;
};
//! [0]

#endif

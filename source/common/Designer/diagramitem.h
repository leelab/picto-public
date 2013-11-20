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

#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include "EditorState.h"
#include "../storage/SearchRequest.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;

/*! \brief This is the base class for all graphical elements in the Picto Designer apart from transition arrows.
 *	\details Objects of this class draw a rectangle on the Designer canvas.  They provide an option to set a
 *	name and type for the rectangle that are drawn onto it.  The dimensions of the rectangle can be set and
 *	\todo Finish documenting this class
 */
class DiagramItem : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io, ArrowSource, ArrowDestination};

    DiagramItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QString name = "",
        QGraphicsItem *parent = 0);
	virtual ~DiagramItem();

	void setName(QString name,QPointF pos = QPointF());
	void setType(QString type);
	QString getName();
	QString getType();
	void setWidth(float width);
	float getWidth();
	void setHeight(float height);
	float getHeight();
	void updateLabel();
    QPixmap image() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    int type() const
        { return Type;}

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void updateDependantGraphics(){};
	virtual void positionChanged(QPoint){};
	virtual void setRect(QRectF rect);
	void setHighlightColor(int highlightIndex,QColor color);
	void enableOutline(int highlightIndex,bool enabled);
	void highlightNameChars(int highlightIndex, QString searchString,bool caseSensitive);
	QSharedPointer<EditorState> editorState_;
	QRectF getRect();
	QRectF getIconRect(){return iconRect_;};
	QSharedPointer<EditorState> getEditorState(){return editorState_;};
	QMenu *myContextMenu;

private:
	QString name_;
	QString type_;
	QGraphicsTextItem* nameText_;
	QString searchString_;
	bool searchCaseSensitive_;
	int searchHighlightIndex_;
	bool stickInPlace_;
	int catchUpFrames_;
	QRectF rect_;
	QRectF iconRect_;
	QMap<int,QColor> highlightColors_;
	QMap<int,bool> outlines_;
private slots:
	void editModeChanged(int mode);
};
//! [0]

#endif

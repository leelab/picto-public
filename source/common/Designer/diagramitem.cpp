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

#include "diagramitem.h"
#include "arrow.h"
#include "../../common/memleakdetect.h"

//! [0]
DiagramItem::DiagramItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QString name,
             QGraphicsItem *parent) : 
QGraphicsPolygonItem(parent/*, scene*/)
{
	editorState_ = editorState;
    myContextMenu = contextMenu;
	nameText_ = NULL;
	stickInPlace_ = true;
	catchUpFrames_ = 0;
	searchHighlightIndex_ = 0;
	searchString_ = "";
	searchCaseSensitive_ = false;
	setName(name);
	connect(editorState_.data(),SIGNAL(editModeChanged(int)),this,SLOT(editModeChanged(int)));

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
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

void DiagramItem::setWidth(float width)
{
	QRectF newRect = getRect();

	newRect.setWidth(width);
	setRect(newRect);
}

float DiagramItem::getWidth()
{
	return rect_.width();
}

void DiagramItem::setHeight(float height)
{
	QRectF newRect = getRect();
	newRect.setHeight(height);
	setRect(newRect);
}

float DiagramItem::getHeight()
{
	return rect_.height();
}

void DiagramItem::updateLabel()
{
	if(!nameText_)
	{
		nameText_ = new QGraphicsTextItem(this);
		nameText_->setDefaultTextColor(QColor(Qt::white));
		nameText_->setZValue(1000.0);
	}
	QString text = getName();

	//If there is currently a search string, highlight the appropriate characters in the name.
	if(!searchString_.isEmpty())
	{
		int foundPos = 0;
		QString colorStartTag = QString("<FONT COLOR=\"%1\">")
			.arg(highlightColors_[searchHighlightIndex_].name());
		QString colorEndTag("</FONT>");
		while((foundPos = text.indexOf(searchString_,foundPos,searchCaseSensitive_?Qt::CaseSensitive:Qt::CaseInsensitive)) >= 0)
		{
			text = text.insert(foundPos+searchString_.length(),colorEndTag);
			text = text.insert(foundPos,colorStartTag);
			foundPos += searchString_.length()+colorStartTag.length()+colorEndTag.length();
		}
	}

	//Add the type info
	if((text != "") && (getType() != ""))
	{
		text.append("<br>");
	}
	if(getType() != "")
	{
		text.append("(").append(getType()).append(")");
	}		
	//Write the text string to the label
	nameText_->setHtml(text);

	//Find out how much room the label takes up
	//If the graphic needs to make room for this label, change its size accordingly
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
		setRect(polybound);
	}
	nameText_->setPos(rect_.topLeft());
}
//! [0]

//! [1]

//! [2]
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

void DiagramItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QGraphicsPolygonItem::paint(painter,option,widget);
	QRectF rect = getRect();
	QPainter::CompositionMode currCompMode = painter->compositionMode();
	painter->translate(-1,-1);
	rect.setWidth(rect.width()+2);
	rect.setHeight(rect.height()+2);
	foreach(int highlightIndex,outlines_.keys())
	{
		Q_ASSERT(highlightColors_.contains(highlightIndex));
		painter->translate(2,2);
		rect.setWidth(rect.width()-4);
		rect.setHeight(rect.height()-4);
		painter->setPen(QPen(highlightColors_[highlightIndex],2));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(rect);
	}
	painter->setCompositionMode(currCompMode);
}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
	if(myContextMenu)
	   myContextMenu->exec(event->screenPos());
}
//! [5]
//! [6]
QVariant DiagramItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
	QVariant inputVal = value;
	//See if the change is a position change.
	if	(	(editorState_->getEditMode() == EditorState::Select)
		&&	(change == QGraphicsItem::ItemPositionChange)
		)
	{
		//See how big the change is.  We don't accept small changes 
		//because they can get triggered by double clicking.
		QPointF translation = inputVal.toPointF() - pos();
		QRectF bounds = boundingRect();
		if	(
				stickInPlace_
			&&	isSelected()
			&&	(	(abs(translation.x()) > (bounds.width()/3))
				||	(abs(translation.y()) > (bounds.height()/3))
				)	
			)
			//This is a large change, stop sticking the item in place
			stickInPlace_ = false;

		if(!stickInPlace_)
		{
			if(catchUpFrames_ < 60)
			{
				//We haven't yet caught our position up from sticking in place.
				//This will move the item smoothly to the desired location after sticking.
				catchUpFrames_++;
				inputVal.setValue(pos()+((catchUpFrames_/60.0)*translation));
			}
		}
		if(!stickInPlace_ || !isSelected())
		{	//If we're not sticking, update graphics underneath this one and report
			//to child classes that the position change.
			//Note that we also do this if the item isn't selected.  This occurs when it
			//is being placed in the scene when it is first created.
			updateDependantGraphics();
			positionChanged(inputVal.toPoint());
		}
		else
		{	//The item is sticking right now.  Don't allow a position change.
			return pos();
		}
    }
	else if (change == QGraphicsItem::ItemSelectedChange)
	{	//Don't allow selection changes except in Select mode.
		if(editorState_->getEditMode() != EditorState::Select)
			return !inputVal.toBool();
	}
	else if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		if(inputVal.toBool())
		{
			editorState_->setSelectedItem(this);
		}
		else
		{
			//When an item is deselected, it should be reinitialized to stick in place.
			stickInPlace_ = true;
			catchUpFrames_ = 0;

			//If this was the last selected item, set the selected item to NULL.  This
			//has the effect of switching the selected asset to the window asset.
			if(editorState_->getSelectedItem() == this)
				editorState_->setSelectedItem(NULL);
		}
	}


    return inputVal;
}

void DiagramItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
	if(editorState_->getEditMode() == EditorState::Select 
		&& mouseEvent->button() == Qt::LeftButton)
	{
		if(!editorState_->getSelectedAsset().isNull())
		{
			editorState_->setWindowAsset(editorState_->getSelectedAsset());
		}
	}
}

void DiagramItem::setRect(QRectF rect)
{
	QRectF textbound;
	if(nameText_)
	{
		textbound = nameText_->boundingRect();
		if(rect.width() < textbound.width())
			rect.setWidth(textbound.width());
	}
	rect_ = rect;
	iconRect_ = QRectF(rect_.left(),rect_.top()+textbound.height(),rect_.width(),rect_.height()-textbound.height());
	setPolygon(QPolygonF(rect_));
	updateLabel();
}

void DiagramItem::setHighlightColor(int highlightIndex,QColor color)
{
	highlightColors_[highlightIndex] = color;
}

void DiagramItem::enableOutline(int highlightIndex,bool enabled)
{
	if(!enabled)
	{
		if(outlines_.contains(highlightIndex))
		{
			outlines_.remove(highlightIndex);
			setRect(getRect());
			update();
		}
		return;
	}
	outlines_[highlightIndex] = true;
	update(getRect());
}

void DiagramItem::highlightNameChars(int highlightIndex, QString searchString, bool caseSensitive)
{
	searchString_ = searchString;
	searchHighlightIndex_ = highlightIndex;
	searchCaseSensitive_ = caseSensitive;
	updateLabel();
	update(getRect());
}

QRectF DiagramItem::getRect()
{
	return rect_;
}

void DiagramItem::editModeChanged(int mode)
{
	if(mode == EditorState::Select)
	{
		setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
	}
	else
	{
		setAcceptedMouseButtons(Qt::RightButton);
	}
}
//! [6]
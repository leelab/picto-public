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

#include "diagramscene.h"
#include "StateMachineElementItem.h"
#include "../../common/storage/DataStore.h"
#include "../common/statemachine/transition.h"
#include "arrow.h"
using namespace Picto;

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent, QSharedPointer<Asset> sceneAsset)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
	diagItemFactory_ = QSharedPointer<DiagramItemFactory>(new DiagramItemFactory(myItemMenu,this));
	myMode = Select;
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
	insertionItem_ = "";
	sceneAsset_ = sceneAsset;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item =
            qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

QGraphicsLineItem* DiagramScene::insertTransition(DiagramItem* source, DiagramItem* dest)
{
	Arrow *arrow = Arrow::Create(source, dest);
	if(!arrow)
		return NULL;
    arrow->setColor(myLineColor);
    addItem(arrow);
    arrow->updatePosition();
	return arrow;
}
//! [4]
DiagramItem* DiagramScene::insertDiagramItem(QSharedPointer<Asset> asset,QPointF pos)
{
	DiagramItem *item = diagItemFactory_->create(asset);
	if(!item)
		return NULL;
	item->setBrush(myItemColor);
	connect(item, SIGNAL(selectedChange(QGraphicsItem *)),
			this, SLOT(setSelectedItem(QGraphicsItem *)));
	item->setPos(pos);
	addItem(item);

	emit itemInserted(static_cast<DiagramItem*>(item));
	return item;
}

void DiagramScene::setSceneAsset(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	sceneAsset_ = asset;
	clear();
	QStringList childTypes = dataStore->getDefinedChildTags();
	QPointF childAssetLoc(100,100);
	QList<QSharedPointer<Transition>> transitions;
	QList<DiagramItem*> diagItems;
	foreach(QString childType, childTypes)
	{
		QList<QSharedPointer<Asset>> assets = dataStore->getGeneratedChildren(childType);
		foreach(QSharedPointer<Asset> childAsset,assets)
		{
			DiagramItem* diagItem = insertDiagramItem(childAsset,childAssetLoc);
			if(diagItem)
			{
				diagItems.push_back(diagItem);
				childAssetLoc = childAssetLoc+QPointF(1.5*diagItem->boundingRect().width(),0);
			}
			else if( (childAsset->assetType() == "Transition") || (childAsset->assetType() == "ControlLink") )
			{
				transitions.push_back(childAsset.staticCast<Transition>());
			}
		}
	}
	foreach(QSharedPointer<Transition> transition,transitions)
	{
		DiagramItem* start = NULL;
		DiagramItem* end = NULL;
		QString source = transition->getSource();
		QString sourceResult = transition->getSourceResult();
		QString dest = transition->getDestination();
		WireableItem* wireItem;
		foreach(DiagramItem* diagItem,diagItems)
		{
			wireItem = static_cast<WireableItem*>(diagItem);
			if(!start && (source == wireItem->getName()))
			{
				QList<DiagramItem*> results = wireItem->getArrowSources();
				foreach(DiagramItem* result,results)
				{
					if(result->getName() == sourceResult)
					{
						start = result;
						break;
					}
				}
			}
			if(!end && (dest == wireItem->getName()))
			{
				end = wireItem->getArrowDest();
			}
			if(start && end)
			{
				break;
			}
		}
		if(start && end)
		{
			insertTransition(start,end);
		}
	}
	emit sceneAssetChanged(sceneAsset_->getParentAsset());
}

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setInsertionItem(QString itemName)
{
	insertionItem_ = "";
    if (itemName == tr("Blue Grid"))
        setBackgroundBrush(QPixmap(":/icons/background1.png"));
    else if (itemName == tr("White Grid"))
        setBackgroundBrush(QPixmap(":/icons/background2.png"));
    else if (itemName == tr("Gray Grid"))
        setBackgroundBrush(QPixmap(":/icons/background3.png"));
    else if (itemName == tr("No Grid"))
        setBackgroundBrush(QPixmap(":/icons/background4.png"));
	else
	{
		insertionItem_ = itemName;
		return;
	}
    update();
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}
void DiagramScene::setSelectedItem(QGraphicsItem *item)
{
	AssetItem* assetItem = dynamic_cast<AssetItem*>(item);
	if(!assetItem)
		return;
	QSharedPointer<Asset> asset = assetItem->getAsset();
	if(asset.isNull())
		return;
	if(item->isSelected() && (selectedAsset_ != asset))
	{
		selectedAsset_ = asset;
		emit assetSelected(selectedAsset_);
	}
	else if(!item->isSelected() && (selectedAsset_ == asset))
	{
		selectedAsset_ = sceneAsset_;
		emit assetSelected(selectedAsset_);
	}
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

void DiagramScene::deleteSelectedItems()
{
	foreach (QGraphicsItem *item, selectedItems()) 
	{
        if (item->type() == DiagramItem::Type) 
		{
            qgraphicsitem_cast<DiagramItem *>(item)->removeDependantGraphics();
        }
        removeItem(item);
    }
}

void DiagramScene::bringToFront()
{
    if (selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [5]

//! [6]
void DiagramScene::sendToBack()
{
    if (selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [5]

//! [6]
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    //DiagramItem *item;
    switch (myMode) {
        case Select:
			if(insertionItem_ == "")
			{
			}
			else if(insertionItem_ != "Text")
			{
				insertDiagramItem(QSharedPointer<Asset>(),mouseEvent->scenePos());
			}
			else
			{
				textItem = new DiagramTextItem();
				textItem->setFont(myFont);
				textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
				textItem->setZValue(1000.0);
				connect(textItem, SIGNAL(lostFocus(DiagramTextItem *)),
						this, SLOT(editorLostFocus(DiagramTextItem *)));
				connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
						this, SIGNAL(itemSelected(QGraphicsItem *)));
				addItem(textItem);
				textItem->setDefaultTextColor(myTextColor);
				textItem->setPos(mouseEvent->scenePos());
				emit textInserted(textItem);
			}
            break;
//! [6] //! [7]
        case InsertLine:
			line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
											mouseEvent->scenePos()));
			line->setPen(QPen(myLineColor, 2));
			addItem(line);
			break;
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]
void DiagramScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
	if(mouseEvent->button() == Qt::LeftButton)
	{
		if(selectedAsset_)
			setSceneAsset(selectedAsset_);
	}
	else if(mouseEvent->button() == Qt::RightButton)
	{
		if(sceneAsset_ && sceneAsset_->getParentAsset())
			setSceneAsset(sceneAsset_->getParentAsset());
	}
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == Select) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;
//! [11] //! [12]

		//Ignore text graphics.
		while(startItems.count() && (startItems.first()->type() == QGraphicsTextItem::Type))
		{
			startItems.removeFirst();
		}
		while(endItems.count() && (endItems.first()->type() == QGraphicsTextItem::Type))
		{
			endItems.removeFirst();
		}

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem =
                qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem =
                qgraphicsitem_cast<DiagramItem *>(endItems.first());
			insertTransition(startItem,endItem);
        }
    }
//! [12] //! [13]
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

//! [14]
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
//! [14]

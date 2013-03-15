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

#include "diagramscene.h"
#include "StateMachineElementItem.h"
#include "../../common/storage/DataStore.h"
#include "../common/statemachine/transition.h"
#include "../../common/statemachine/StateMachine.h"
#include "../../common/statemachine/state.h"
#include "../../common/statemachine/pausepoint.h"
#include "../../common/statemachine/ResultContainer.h"
#include "../../common/statemachine/Result.h"
#include "StartBarItem.h"
#include "arrow.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
DiagramScene::DiagramScene(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QObject *parent)
    : QGraphicsScene(parent)
{
	editorState_ = editorState;
	connect(editorState_.data(), SIGNAL(windowAssetChanged(QSharedPointer<Asset>)), this, SLOT(setSceneAsset(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(lineColorChanged(QColor)), this, SLOT(setLineColor(QColor)));
	connect(editorState_.data(), SIGNAL(textColorChanged(QColor)), this, SLOT(setTextColor(QColor)));
	connect(editorState_.data(), SIGNAL(itemColorChanged(QColor)), this, SLOT(setItemColor(QColor)));
	connect(editorState_.data(), SIGNAL(fontChanged(QFont)), this, SLOT(setFont(QFont)));
	connect(editorState_.data(), SIGNAL(backgroundPatternChanged(QPixmap)), this, SLOT(setBackgroundPattern(QPixmap)));
    myItemMenu = contextMenu;
	diagItemFactory_ = QSharedPointer<DiagramItemFactory>(new DiagramItemFactory(editorState,myItemMenu,this));
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
	insertionItem_ = "";
	newItemIndex_ = 1;
	startBar_ = NULL;
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &)
{
    if (isItemChange(Arrow::Type)) {
        Arrow *item =
            qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(editorState_->getLineColor());
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &)
{
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(editorState_->getTextColor());
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &)
{
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(editorState_->getItemColor());
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &)
{
    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(editorState_->getFont());
    }
}

QGraphicsLineItem* DiagramScene::insertTransition(DiagramItem* source, DiagramItem* dest, QSharedPointer<Asset> transition)
{
	Arrow *arrow;
	if(transition.isNull())
		arrow = Arrow::Create(editorState_->getWindowAsset(),source,dest,myItemMenu,NULL);
	else
		arrow = Arrow::Create(transition.staticCast<Transition>(),source,dest,myItemMenu,NULL);
	if(!arrow)
		return NULL;
    arrow->setColor(editorState_->getLineColor());
    addItem(arrow); //Don't need to add it cause its already parented by this
    arrow->updatePosition();
	if(transition.isNull())
		editorState_->setLastActionUndoable();
	return arrow;
}
//! [4]
DiagramItem* DiagramScene::insertDiagramItem(QSharedPointer<Asset> asset,QPointF pos)
{
	DiagramItem *item = diagItemFactory_->create(asset);
	if(!item)
		return NULL;
	if(item->pos() == QPointF())
		item->setPos(pos);
	addItem(item);

	emit itemInserted(static_cast<DiagramItem*>(item));
	return item;
}

// Returns a QRectF that contains all items in this
//scene except the start bar.  If we didn't have the start bar,
//we could just used itemsBoundingRect()
QRectF DiagramScene::getDefaultZoomRect()
{
	if(!startBar_)
		return itemsBoundingRect();
	QRectF returnVal;
	//Iterate through all items in scene
	foreach(QGraphicsItem* item,items())
	{
		//If item isn't start bar, add it to the returnVal rectangle
		if(item == startBar_)
			continue;
		returnVal = returnVal.united(item->boundingRect());
	}
	//Return the rectangle
	return returnVal;
}

void DiagramScene::setSceneAsset(QSharedPointer<Asset> asset)
{					
	QSharedPointer<DataStore> dataStore(asset.staticCast<DataStore>());

	clear();
	//Add a start bar
	startBar_ = NULL;
	newItemIndex_ = 1;
	QStringList childTypes = dataStore->getDefinedChildTags();
	QPointF childAssetLoc(sceneRect().center().x(),sceneRect().center().y());
	QList<QSharedPointer<Transition>> transitions;
	QList<DiagramItem*> diagItems;
	if(dataStore->inherits("Picto::StateMachine") || dataStore->inherits("Picto::State"))
	{
		startBar_ = new StartBarItem("",editorState_,NULL,dataStore);
		diagItems.push_back(startBar_);
		addItem(startBar_);
	}
	foreach(QString childType, childTypes)
	{
		QList<QSharedPointer<Asset>> assets = dataStore->getGeneratedChildren(childType);
		foreach(QSharedPointer<Asset> childAsset,assets)
		{
			if( childAsset->assetType() == "Transition" )
			{
				transitions.push_back(childAsset.staticCast<Transition>());
				continue;
			}
			DiagramItem* diagItem = insertDiagramItem(childAsset,childAssetLoc);
			if(diagItem)
			{
				//If something here has a default name, we should start after its index.
				if(diagItem->getName().contains(QRegExp("NewItem[1-9]")))
				{
					int itemIndex = diagItem->getName().remove("NewItem").toInt();
					if(itemIndex >= newItemIndex_)
						newItemIndex_ = itemIndex+1;
				}
				
				//Now add it to the list and update the default placement location
				diagItems.push_back(diagItem);
				childAssetLoc = childAssetLoc+QPointF(1.5*diagItem->boundingRect().width(),0);
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
		if(source.isEmpty() && sourceResult.isEmpty() && startBar_)
			start = startBar_;
		WireableItem* wireItem;
		QSharedPointer<Asset> asset;
		foreach(DiagramItem* diagItem,diagItems)
		{
			wireItem = dynamic_cast<WireableItem*>(diagItem);
			if(!wireItem)
				continue;
			asset = wireItem->getAsset();
			if(asset.isNull())
				continue;
			if(!start && (source == asset->getName()))
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
			if(!end && (dest == asset->getName()))
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
			insertTransition(start,end,transition);
		}
	}
	editorState_->setWindowItemsLoaded();
}

void DiagramScene::setBackgroundPattern(QPixmap pattern)
{
	setBackgroundBrush(pattern);
	update();
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
	QList<QGraphicsItem*> items;
	foreach (QGraphicsItem *item, selectedItems()) 
	{
		removeItem(item);
		items.push_back(item);
    }

	AssetItem* assetItem;
	bool needsReset = false;
	//We don't actually have to delte anything here, just set the assets as deleted.  This is because
	//when we serialize out and back in, the deleted assets won't be included and all of their children
	//are automatically not included as well.  Transitions are also set to delete themselves whenever 
	//an attached asset is delete, so that's not a problem either.
	foreach (QGraphicsItem *item, items) 
	{
        if (item->type() == DiagramItem::Type) 
		{
			assetItem = dynamic_cast<AssetItem*>(item);
			if(assetItem && assetItem->getAsset())
			{
				assetItem->getAsset()->setDeleted();
				needsReset = true;
			}
            //delete qgraphicsitem_cast<DiagramItem *>(item);
		}
		else if(item->type() == Arrow::Type)
		{
			Arrow* arrow = dynamic_cast<Arrow*>(item);
			if(arrow && arrow->getAsset())
			{
				arrow->getAsset()->setDeleted();
				needsReset = true;
			}
			//delete qgraphicsitem_cast<DiagramItem *>(item);
		}
		//else
			//delete item;
    }
	if(needsReset)
		editorState_->triggerExperimentReset();
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
    switch (editorState_->getEditMode()) {
        case Select:
			{
				QSharedPointer<Asset> newAsset = createNewAsset();
				if(newAsset.isNull())
					break;
				if(insertDiagramItem(newAsset,mouseEvent->scenePos()))
				{
					if(newAsset.dynamicCast<UIEnabled>())
						newAsset.staticCast<UIEnabled>()->setName(QString("NewItem%1").arg(newItemIndex_++));
					editorState_->setLastActionUndoable();
					editorState_->triggerItemInserted();
				}
				break;
			}
//! [6] //! [7]
        case InsertLine:
			line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
											mouseEvent->scenePos()));
			line->setPen(QPen(editorState_->getLineColor(), 2));
			addItem(line);
			editorState_->triggerItemInserted();
			break;
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (editorState_->getEditMode() == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (editorState_->getEditMode() == Select) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && editorState_->getEditMode() == InsertLine) {
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
		while(startItems.count() && (!dynamic_cast<ArrowPortItem*>(startItems.first())))
		{
			startItems.removeFirst();
		}
		while(endItems.count() && (!dynamic_cast<ArrowPortItem*>(endItems.first())))
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

void DiagramScene::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift)
	{
		editorState_->setEditMode(EditorState::Navigate);
		event->accept();
		return;
	}
	QGraphicsScene::keyPressEvent(event);
}

void DiagramScene::keyReleaseEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift)
	{
		editorState_->setEditMode(editorState_->getLastEditMode());
		event->accept();
		return;
	}
	QGraphicsScene::keyReleaseEvent(event);
}

QSharedPointer<Asset> DiagramScene::createNewAsset()
{
	QString category = editorState_->getInsertionItemCategory();
	if(category == "")
		return QSharedPointer<Asset>();
	QString type = editorState_->getInsertionItemType();
	QString errorStr;
	QSharedPointer<DataStore> dataStore = editorState_->getWindowAsset().staticCast<DataStore>();
	QSharedPointer<Asset> newAsset = dataStore->createChildAsset(category,type,errorStr);
	
	//Due to the nature of scriptables and scriptable containers as elements whose functionality 
	//involves spanning state machine levels, they need to be specifically added to their container 
	//after being created.
	if(newAsset.dynamicCast<Scriptable>() && dataStore.dynamicCast<ScriptableContainer>())
		dataStore.staticCast<ScriptableContainer>()->addScriptable(newAsset.staticCast<Scriptable>());
	if(newAsset.dynamicCast<ScriptableContainer>() && dataStore.dynamicCast<ScriptableContainer>())
		dataStore.staticCast<ScriptableContainer>()->addChildScriptableContainer(newAsset.staticCast<ScriptableContainer>());
	////////////////////////////////////////////////////////////////////////////////////////////


	return newAsset;
}
//! [13]
void DiagramScene::insertTextItem(QString text,QPointF pos)
{
	textItem = new DiagramTextItem();
	textItem->setFont(editorState_->getFont());
	textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
	textItem->setZValue(1000.0);
	connect(textItem, SIGNAL(lostFocus(DiagramTextItem *)),
			this, SLOT(editorLostFocus(DiagramTextItem *)));
	connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
			this, SIGNAL(itemSelected(QGraphicsItem *)));
	addItem(textItem);
	textItem->setDefaultTextColor(editorState_->getTextColor());
	textItem->setPos(pos);
	emit textInserted(textItem);
}
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

#include <QtWidgets>
#include <QApplication>
#include <QClipboard>

#include "diagramscene.h"
#include "StateMachineElementItem.h"
#include "../../common/storage/DataStore.h"
#include "../common/statemachine/transition.h"
#include "../../common/statemachine/StateMachine.h"
#include "../../common/statemachine/state.h"
#include "../../common/statemachine/pausepoint.h"
#include "../../common/statemachine/ResultContainer.h"
#include "../../common/statemachine/Result.h"
#include "AnalysisItem.h"
#include "StartBarItem.h"
#include "arrow.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief The default buffer from the top left of the screen where new items are added when they are placed automatically.
 *	\note This is not well named for historical reasons, its name should be fixed.
 */
#define DEFAULT_ITEM_BORDER_BUFFER 50

DiagramScene::DiagramScene(QSharedPointer<EditorState> editorState, QMenu *itemContextMenu, QMenu *sceneContextMenu,  QObject *parent)
    : QGraphicsScene(parent)
{
	editorState_ = editorState;
	connect(editorState_.data(), SIGNAL(windowAssetChanged(QSharedPointer<Asset>)), this, SLOT(setSceneAsset(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(lineColorChanged(QColor)), this, SLOT(setLineColor(QColor)));
	connect(editorState_.data(), SIGNAL(textColorChanged(QColor)), this, SLOT(setTextColor(QColor)));
	connect(editorState_.data(), SIGNAL(itemColorChanged(QColor)), this, SLOT(setItemColor(QColor)));
	connect(editorState_.data(), SIGNAL(fontChanged(QFont)), this, SLOT(setFont(QFont)));
	connect(editorState_.data(), SIGNAL(backgroundPatternChanged(QPixmap)), this, SLOT(setBackgroundPattern(QPixmap)));
    myItemMenu = itemContextMenu;
	sceneMenu_ = sceneContextMenu;
	diagItemFactory_ = QSharedPointer<DiagramItemFactory>(new DiagramItemFactory(editorState,myItemMenu,this));
    line = 0;
    textItem = 0;
	insertionItem_ = "";
	newItemIndex_ = 1;
	startBar_ = NULL;
	copier_ = QSharedPointer<Copier>(new Copier(editorState));
	useNavigateMode_ = false;
	mouseOverScene_ = false;
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
void DiagramScene::setLineColor(const QColor &)
{
    if (isItemChange(Arrow::Type)) {
        Arrow *item =
            qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(editorState_->getLineColor());
        update();
    }
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
void DiagramScene::setTextColor(const QColor &)
{
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(editorState_->getTextColor());
    }
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
void DiagramScene::setItemColor(const QColor &)
{
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(editorState_->getItemColor());
    }
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
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

/*! \brief Creates an Arrow to represent the input transition from the source DiagramItem to the dest DiagramItem and adds it to the scene.
 *	\details If the input transition is NULL, a new transition object will be created and added as a child of the current Window Asset as 
 *	well.
 */
QGraphicsLineItem* DiagramScene::insertTransition(DiagramItem* source, DiagramItem* dest, QSharedPointer<Asset> transition)
{
	Arrow *arrow;
	if(transition.isNull())
		arrow = Arrow::Create(editorState_, editorState_->getWindowAsset(),source,dest,myItemMenu,NULL);
	else
		arrow = Arrow::Create(editorState_, transition.staticCast<Transition>(),source,dest,myItemMenu,NULL);
	if(!arrow)
		return NULL;
    arrow->setColor(editorState_->getLineColor());	//This is currently always black, but who knows, maybe we'll want this configurable someday for some reason.
    addItem(arrow); //Don't need to add it cause its already parented by this
    arrow->updatePosition();
	if(transition.isNull())
		editorState_->setLastActionUndoable();
	return arrow;
}

/*! \brief Creates a diagram item for the input Asset according to its type and adds it to the scene.
 *	\details If the input Asset already has an attached UIElement including its position, that position will
 *	be used to position it, otherwise it will be positioned based on the input pos value.
 *	After the DiagramItem is added itemInserted() is emitted and the DiagramItem is returned from the function.
 */
DiagramItem* DiagramScene::insertDiagramItem(QSharedPointer<Asset> asset,QPointF pos)
{
	DiagramItem *item = diagItemFactory_->create(asset);
	if(!item)
		return NULL;
	addItem(item);
	if(item->pos() == QPointF())
	{
		QPointF newPos = pos+QPointF(0.5*item->boundingRect().width(),0);//Put it at the input position plus a small left buffer
		item->setPos(newPos);
		if(asset->inherits("Picto::UIEnabled"))
			asset.staticCast<UIEnabled>()->setPos(newPos.toPoint());
	}

	emit itemInserted(static_cast<DiagramItem*>(item));
	return item;
}

/*! \brief Returns a QRectF that contains all items in this scene except the start bar.  
 *	\details If we didn't have the start bar, we could just used itemsBoundingRect(), but we do, so we use this
 */
QRectF DiagramScene::getDefaultZoomRect()
{
	if(!startBar_)
		return itemsBoundingRect();
	QRectF returnVal;
	//Iterate through all items in scene
	QGraphicsItem* itemBuffer;
	foreach(QGraphicsItem* item,items())
	{
		//If item isn't the start bar or in the startbar or a transition, add it to the returnVal rectangle
		itemBuffer = item;
		bool inStartBar = false;
		while(itemBuffer)
		{
			if(itemBuffer == startBar_)
			{
				inStartBar = true;
				break;
			}
			itemBuffer = itemBuffer->parentItem();
		}
		if(inStartBar)
			continue;
		if(dynamic_cast<Arrow*>(item))
			continue;
		
		QRectF itemBoundRect = item->boundingRect();
		QPoint itemPos = item->pos().toPoint();
		if((itemPos.x() < 0) || itemPos.y() < 0)
			continue;
		//ItemBoundRect and pos were doing something screwy here.  We should probably look into this at some
		//point, but removing items with pos less than zero and seting top left of bounding rect to pos
		//seemed to fix the problem.
		itemBoundRect = QRectF(itemPos.x(),itemPos.y(),itemBoundRect.width(),itemBoundRect.height());;
		if(returnVal.isNull())
			returnVal = itemBoundRect;
		else
			returnVal = returnVal.united(itemBoundRect);
	}
	if(returnVal.x() < 0)
		returnVal.setX(0);
	if(returnVal.y() < 0)
		returnVal.setY(0);
	//Return the rectangle
	return returnVal;
}

/*! \brief Sets the current scene asset (ie. The asset who's canvas we are looking at) to the input.
 *	\details This function is called when the EditorState object's windowAssetChanged() signal is called.
 *	What we refer to here as the SceneAsset, is referred to everywhere else as the WindowAsset.  The 
 *	function goes through all of the input asset's children, creates DiagramItem objects for them using
 *	insertDiagramItem(), adds the necessary transition Arrow objects using insertTransition()
 *	adds the necessary AnalysisItem objects using insertDiagramItem(), then tells the EditorState that
 *	the asset was loaded into the current window using EditorState::setWindowItemsLoaded().
 */
void DiagramScene::setSceneAsset(QSharedPointer<Asset> asset)
{					
	QSharedPointer<DataStore> dataStore(asset.staticCast<DataStore>());

	clear();
	//Add a start bar
	startBar_ = NULL;
	newItemIndex_ = 1;
	QStringList childTypes = dataStore->getDefinedChildTags();
	//Set the default item placement location to the top left of the scene
	QPointF childAssetLoc(sceneRect().topLeft().x()+DEFAULT_ITEM_BORDER_BUFFER,sceneRect().topLeft().y()+DEFAULT_ITEM_BORDER_BUFFER);
	QList<QSharedPointer<Transition>> transitions;
	QList<DiagramItem*> diagItems;
	if(dataStore->inherits("Picto::StateMachine") || dataStore->inherits("Picto::State"))
	{
		startBar_ = new StartBarItem("",editorState_,NULL,dataStore);
		diagItems.push_back(startBar_);
		addItem(startBar_);
		//Put the default placement location at the top right of the start bar
		childAssetLoc = startBar_->boundingRect().topRight()+QPointF(DEFAULT_ITEM_BORDER_BUFFER,DEFAULT_ITEM_BORDER_BUFFER);
	}
	//Add all experiment assets
	foreach(QString childType, childTypes)
	{
		QList<QSharedPointer<Asset>> assets = dataStore->getGeneratedChildren(childType);
		foreach(QSharedPointer<Asset> childAsset,assets)
		{
			if( childAsset->inherits("Picto::Transition") )
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
				
				//Now add it to the list and update the default placement location to the top
				//right of the currently added items
				diagItems.push_back(diagItem);
				childAssetLoc = QPointF(getDefaultZoomRect().right(),diagItem->pos().y());
			}
		}
	}

	//Add all transitions
	foreach(QSharedPointer<Transition> transition,transitions)
	{
		DiagramItem* start = NULL;
		DiagramItem* end = NULL;
		int sourceId = transition->getSourceId();
		int sourceResultId = transition->getSourceResultId();
		int destId = transition->getDestinationId();
		if(!sourceId && !sourceResultId && startBar_)
			start = startBar_;
		WireableItem* wireItem;
		QSharedPointer<Asset> asset;
		QSharedPointer<Asset> resultAsset;
		foreach(DiagramItem* diagItem,diagItems)
		{
			wireItem = dynamic_cast<WireableItem*>(diagItem);
			if(!wireItem)
				continue;
			asset = wireItem->getAsset();
			if(asset.isNull())
				continue;
			if(!start && (sourceId == asset->getAssetId()))
			{
				QList<DiagramItem*> results = wireItem->getArrowSources();
				foreach(DiagramItem* result,results)
				{
					resultAsset = static_cast<ArrowSourceItem*>(result)->getAsset();
					if(resultAsset && resultAsset->getAssetId() == sourceResultId)
					{
						start = result;
						break;
					}
				}
			}
			if(!end && (destId == asset->getAssetId()))
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

	//Add Analysis assets for the current analysis
	QSharedPointer<Analysis> currAnalysis = editorState_->getCurrentAnalysis();
	if(!currAnalysis.isNull())
	{
		childTypes = dataStore->getAssociateChildTags(currAnalysis->getAssociateId());
		foreach(QString childType, childTypes)
		{
			QList<QSharedPointer<Asset>> assets = dataStore->getAssociateChildren(currAnalysis->getAssociateId(),childType);
			foreach(QSharedPointer<Asset> childAsset,assets)
			{
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
				
					//Now add it to the list and update the default placement location to the top
					//right of the currently added items
					diagItems.push_back(diagItem);
					childAssetLoc = QPointF(getDefaultZoomRect().right(),diagItem->pos().y());
				}
			}
		}
	}
	editorState_->setWindowItemsLoaded();
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
void DiagramScene::setBackgroundPattern(QPixmap pattern)
{
	setBackgroundBrush(pattern);
	update();
}

/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
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

/*! \brief Deletes all currently selected DiagramItems and Arrows along with their referenced Asset and Transition objects.
 *	\details If a deletion actually took place, this function calls EditorState::triggerExperimentReset() to cause
 *	the design to be reloaded from XML thereby removing deleted elements and reloading this scene with the correct
 *	diagrams.
 *	\sa Designer::resetExperiment()
 */
void DiagramScene::deleteSelectedItems()
{

	QList<QGraphicsItem*> items = selectedItems();
	//Check to see if any of the selected items contain hidden analysis elements.  If so, warn user and verify action.
	SearchRequest searchRequest(SearchRequest::INACTIVE_ANALYSES,SearchRequest::EXISTS);
	foreach (QGraphicsItem *item, items) 
	{
		AssetItem* assetItem = dynamic_cast<AssetItem*>(item);
		if(!assetItem || !assetItem->getAsset())
			continue;
		QSharedPointer<DataStore> dataStore = assetItem->getAsset().dynamicCast<DataStore>();
		if(dataStore)
		{
			if(dataStore->searchRecursivelyForQuery(searchRequest))
			{
				//There are hidden analyses, verify delete
				int result = QMessageBox::question(NULL,"Delete Hidden Analysis Elements?","This delete operation will cause elements from inactive Analyses to be deleted.  Continue?",QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
				switch(result)
				{
				case QMessageBox::No:
					return;
				case QMessageBox::Yes:
					break;
				default:
					return;
				};
				break;
			}
		}
    }

	bool removeExperimentalAssets = true;
	if(!editorState_->getCurrentAnalysis().isNull())
		removeExperimentalAssets = false;
	//Remove all the graphic items except experimental items when an analysis is active
	foreach (QGraphicsItem *item, items) 
	{
		if(removeExperimentalAssets || qgraphicsitem_cast<AnalysisItem*>(item))
			removeItem(item);
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

/*! \brief Uses a Copier object to copy the Asset objects referenced by all selected DiagramItems.
 *	\note Transitions are not copied even if they are selected.
 *	\sa getSelectedAssets()
 */
void DiagramScene::copySelectedItems()
{
	copier_->copy(getSelectedAssets(),(!editorState_->getCurrentAnalysis().isNull()));
}

/*! \brief Imports AnalysisElements that were just exported into the currently selected Asset.
 *	\details Shows a warning and returns if no Assets or more than one Asset are selected.
 *	Warnings will also be displayed by the Copier object if there are other problems with the
 *	import.  For more information see Copier::paste().
 */
void DiagramScene::analysisImportSelectedItem()
{
	//Make sure only one asset is selected
	QList<QSharedPointer<Asset>> selectedAssets = getSelectedAssets();
	if(selectedAssets.isEmpty())
	{
		QMessageBox::warning(NULL,"Import Failed","Please select the container to which elements should be imported.");
		return;
	}
	if(selectedAssets.size() > 1)
	{
		QMessageBox::warning(NULL,"Import Failed","Please select a single container to which elements should be imported.");
		return;
	}
	copier_->paste(selectedAssets.first());
}

/*! \brief Exports all AnalysisElements from the selected items.
 *	\details Exported text is stored in the computer's clipboard for importing.
 *	This function uses the Copier::copy() function.
 *	\sa Copier::copy()
 */
void DiagramScene::analysisExportSelectedItem()
{
	copier_->copy(getSelectedAssets(),true);
}

/*! \brief Pastes any Assets whose data was stored in the Clipboard (this should have been done by copySelectedItems()) into the current Window Asset.
 *	\details When pasteItems() was initiated from a contextMenuEvent(), the copied Asset's DiagramItems are pasted at the position where the user created the
 *	context menu.  Otherwise, they are pasted to the right side of the currently visible DiagramItems.
 */
void DiagramScene::pasteItems()
{
	//Make sure no assets are selected
	QList<QSharedPointer<Asset>> selectedAssets = getSelectedAssets();
	if(selectedAssets.size() > 0)
	{
		QMessageBox::warning(NULL,"Paste Failed","Paste can only occur within the current viewable window.  Please make sure that only the window asset is selected.");
		return;
	}

	QPoint pasteLoc;
	if(latestPastePos_.isNull())
	{
		QRectF zoomRect = getDefaultZoomRect();
		pasteLoc = zoomRect.topRight().toPoint();
		pasteLoc.setY(zoomRect.center().y());
	}
	else
		pasteLoc = latestPastePos_;
	copier_->paste(editorState_->getWindowAsset(),pasteLoc);
	
}

/*! \brief Extends the QGraphicsScene::mousePressEvent() to allow the designer to start drawing a Transition Arrow.
 *	\details The designer can draw a Transition Arrow when the EditorState::getEditMode() is the EditorState::InserLine 
 *	mode which occurs when the mouse hovers over a result bar hanging off of a StateMachineElement.
 */
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    if(editorState_->getEditMode() == EditorState::InsertLine)
	{
		editorState_->setEditMode(EditorState::DrawLine);
		line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
										mouseEvent->scenePos()));
		line->setPen(QPen(editorState_->getLineColor(), 2));
		addItem(line);
		editorState_->triggerItemInserted();
	}

    QGraphicsScene::mousePressEvent(mouseEvent);
}

/*! \brief Extends the QGraphicsScene::mouseMoveEvent() as part of the DiagramScene's context aware tools logic.
 *	\details This function allows the designer to continue drawing a Transition Arrow after they started doing this in the mousePressEvent().
 *	The DiagramScene knows if the designer is in the midst of drawing an Arrow by looking for the EditorState::DrawLine
 *	mode.  It also handles other context aware issues like changing the EditorState::EditMode
 *	depending on the current mouse position.
 *	\sa EditorState::getEditMode()
 */ 
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseMoveEvent(mouseEvent);
	mouseOverScene_ = false;
    if (editorState_->getEditMode() == EditorState::DrawLine) {
		if(line != 0)
		{
			QLineF newLine(line->line().p1(), mouseEvent->scenePos());
			line->setLine(newLine);
		}
    } 
	//else if (editorState_->getEditMode() == EditorState::Select) {
 //       QGraphicsScene::mouseMoveEvent(mouseEvent);
 //   }
	else if(editorState_->getEditMode() == EditorState::PlaceItem)
	{
		//Don't do anything in this case
	}
	else if(!itemAt(mouseEvent->scenePos(),QTransform()))
	{
		if(mouseEvent->isAccepted())
			return;
		EditorState::EditMode newEditMode = useNavigateMode_?EditorState::Navigate:EditorState::Select;
		if(editorState_->getEditMode() != newEditMode)
			editorState_->setEditMode(newEditMode);
		mouseOverScene_ = true;
	}
}

/*! \brief Extends the QGraphicsScene::mouseReleaseEvent() for placing new DiagramItems and finishing the Transition Arrow
 *	creation process.
 *	\details The designer's release of the mouse is the event trigger for consructing DiagramItems that were selected in the 
 *	Toolbox as well as adding Transitions for Arrows that were started during a mousePressEvent().  This logic is handled
 *	here.
 */
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	//Only respond to left buttons
	if (mouseEvent->button() != Qt::LeftButton)
	{
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
		return;
	}

	//If we're in Select mode, check if a new asset needs to be generated.
	//If it does, generate it.
    if(editorState_->getEditMode() == EditorState::PlaceItem)
	{
		QSharedPointer<Asset> newAsset = createNewAsset();
		if(newAsset)
		{
			DiagramItem *newItem = insertDiagramItem(newAsset,mouseEvent->scenePos());
			if(newItem)
			{
				if(newAsset.dynamicCast<UIEnabled>())
					newAsset.staticCast<UIEnabled>()->setName(QString("NewItem%1").arg(newItemIndex_++));
				editorState_->setLastActionUndoable();
				editorState_->triggerItemInserted();
				clearSelection();
				newItem->setSelected(true);
			}
		}
	}
	else
	//If a line is being drawn, check if we should add it or delete it.
	//If we should add it, create the transition and add it.
    if (line != 0 && editorState_->getEditMode() == EditorState::DrawLine)
	{

        QList<QGraphicsItem *> startItems = items(line->line().p1());
		for(QList<QGraphicsItem *>::iterator iter = startItems.begin();iter != startItems.end();)
		{
			if((*iter) == line)
				iter = startItems.erase(iter);
			else
				iter++;
		}

		QList<QGraphicsItem *> endItems = items(line->line().p2());
		for(QList<QGraphicsItem *>::iterator iter = endItems.begin();iter != endItems.end();)
		{
			if((*iter) == line)
				iter = endItems.erase(iter);
			else
				iter++;
		}

        removeItem(line);
        delete line;
		line = NULL;

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
		editorState_->setEditMode(EditorState::Select);
    }

    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

/*! \brief Extends the QGraphicsScene::keyPressEvent() for moving the EditorState between EditorState::Navigate
 *	and EditorState:Select mode.
 *	\details We use the Shift key state to transition between EditorState::Select mode and EditorState::Navigate mode.
 *	In Navigate mode, the mouse wheel adjusts the current canvas zoom and clicking the mouse allows the designer
 *	to drag the canvas position.  In EditorState::Select mode, clicking on a DiagramItem causes it to be selected.
 */
void DiagramScene::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift)
	{
		useNavigateMode_ = true;
		if(mouseOverScene_ && (editorState_->getEditMode() == EditorState::Select))
			editorState_->setEditMode(EditorState::Navigate);
		event->accept();
		return;
	}
	QGraphicsScene::keyPressEvent(event);
}

/*! \brief Extends the QGraphicsScene::keyReleaseEvent() for moving the EditorState between EditorState::Navigate
 *	and EditorState:Select mode.
 *	\details We use the Shift key state to transition between EditorState::Select mode and EditorState::Navigate mode.
 *	In Navigate mode, the mouse wheel adjusts the current canvas zoom and clicking the mouse allows the designer
 *	to drag the canvas position.  In EditorState::Select mode, clicking on a DiagramItem causes it to be selected.
 */
void DiagramScene::keyReleaseEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift)
	{
		useNavigateMode_ = false;
		if(mouseOverScene_ && (editorState_->getEditMode() == EditorState::Navigate))
			editorState_->setEditMode(EditorState::Select);
		event->accept();
		return;
	}
	QGraphicsScene::keyReleaseEvent(event);
}

/*! \brief Captures the position at which the designer right clicked for a ContextMenu in order to use 
 *	it as the paste position should the designer choose the paste action.
 *	\sa pasteItems()
 */
void DiagramScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsScene::contextMenuEvent(event);
	if(event->isAccepted())
		return;
	if(myItemMenu)
	{
		latestPastePos_ = event->scenePos().toPoint();
		sceneMenu_->exec(event->screenPos());
		latestPastePos_ = QPoint();
	}
}

/*! \brief Creates a new Asset based on the currently selected Asset type in the Toolbox.
 *	\sa mouseReleaseEvent()
 */
QSharedPointer<Asset> DiagramScene::createNewAsset()
{
	QString category = editorState_->getInsertionItemCategory();
	if(category == "")
		return QSharedPointer<Asset>();
	QString type = editorState_->getInsertionItemType();
	QString errorStr;


	QSharedPointer<Asset> newAsset;
	//If we're working on experimental design
	if(editorState_->getCurrentAnalysis().isNull())
	{
		//The children are created from the window asset
		QSharedPointer<DataStore> parentDataStore = editorState_->getWindowAsset().staticCast<DataStore>();
		newAsset = parentDataStore->createChildAsset(category,type,errorStr);
	}
	else //If we're working on analysis design
	{
		//The children are created from the current analysis and added to the window asset
		QSharedPointer<DataStore> parentDataStore = editorState_->getCurrentAnalysis().staticCast<DataStore>();
		newAsset = parentDataStore->createChildAsset(category,type,errorStr);
		if(!newAsset.isNull())
		{
			QSharedPointer<DataStore> windowDataStore = editorState_->getWindowAsset().staticCast<DataStore>();
			windowDataStore->AddAssociateChild(editorState_->getCurrentAnalysis()->getAssociateId(),category,newAsset);
		}
		
	}

	
	//Due to the nature of scriptables and scriptable containers as elements whose functionality 
	//involves spanning state machine levels, they need to be specifically added to their container 
	//after being created.
	//if(newAsset.dynamicCast<Scriptable>() && dataStore.dynamicCast<ScriptableContainer>())
	//	dataStore.staticCast<ScriptableContainer>()->addScriptable(newAsset.staticCast<Scriptable>());
	//if(newAsset.dynamicCast<ScriptableContainer>() && dataStore.dynamicCast<ScriptableContainer>())
	//	dataStore.staticCast<ScriptableContainer>()->addChildScriptableContainer(newAsset.staticCast<ScriptableContainer>());
	////////////////////////////////////////////////////////////////////////////////////////////


	return newAsset;
}
/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
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
/*! \brief This function is never used and left over from the QtToolkit sample.  It should probably be removed unless you want to keep it for reference.*/
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

/*! \brief Returns a list of Asset objects referenced by the currently selected DiagramItems.
 */
QList<QSharedPointer<Asset>> DiagramScene::getSelectedAssets()
{
	QList<QSharedPointer<Asset>> assets;
	QSharedPointer<Asset> asset;
	foreach (QGraphicsItem *item, selectedItems()) 
	{
		AssetItem* assetItem = dynamic_cast<AssetItem*>(item);
		if(!assetItem)
			continue;
		asset = assetItem->getAsset();
		if(asset)
			assets.append(asset);
    }
	return assets;
}


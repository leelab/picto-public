#include "EditorState.h"
#include <QtWidgets>
#include <QSharedPointer>
#include <QInputDialog>
#include "../../common/storage/DataStore.h"
#include "../../common/statemachine/uienabled.h"
#include "../../common/statemachine/uiinfo.h"
#include "AssetItem.h"
#include "ArrowDestinationItem.h"
#include "StartBarItem.h"
#include "../../common/memleakdetect.h"
EditorState::EditorState()
:
editMode_(Select),
lastEditMode_(Select),
font_(),
textColor_(Qt::black),
itemColor_(Qt::white),
lineColor_(Qt::black),
insertItemCategory_(Select),
insertItemType_(""),
windowAssetPath_(""),
selectedItem_(NULL)
{
    //currAnalysis_ = Analysis::Create().staticCast<Analysis>();	//TEMPORARY UNTIL THIS GETS SET FROM SOMEWHERE ELSE!!!!!!!!!!
}

QList<SearchRequest> EditorState::getSearchRequests()
{
	return searchRequests_.values();
}

void EditorState::setTopLevelAsset(QSharedPointer<Picto::Asset> topLevelAsset)
{
	topAsset_ = topLevelAsset;
	Q_ASSERT(!topAsset_.isNull());
	setCurrentAnalysis(QSharedPointer<Analysis>());
	//setWindowAsset(topAsset_);
}

//Returns false if the analysis could not be added to any tasks.
bool EditorState::setCurrentAnalysis(QSharedPointer<Analysis> currAnalysis)
{
	if(!topAsset_)
		return false;
	//Run through this function even if the current analysis hasn't changed, because
	//the current analysis in the designConfig may have been changed and this will 
	//reset it.

	//If there is a current analysis
	if(currAnalysis)
	{
		//Verify that its linked into the experiment.  
		QSharedPointer<Experiment> exp = topAsset_.staticCast<Experiment>();
		Q_ASSERT(exp);		
		//Link the analysis to the experiment
		QString feedback;
		bool linkResult = currAnalysis->LinkToAsset(exp,feedback);
		if(!linkResult)
		{	//If anything didn't go perfectly in the link (had to link by paths or failed) tell the user.
			QMessageBox msg;
			msg.setText("The Analysis was not fully compatible with this Experiment.");
			msg.setDetailedText(feedback);
			msg.setIcon(QMessageBox::Warning);
			msg.exec();
		}
	}
	currAnalysis_ = currAnalysis;
	QList<QUuid> activeAnalysisList;
	if(currAnalysis_)
		activeAnalysisList.append(currAnalysis_->getAssociateId());
	topAsset_->getDesignConfig()->setActiveAnalysisIds(activeAnalysisList);
	emit currentAnalysisChanged(currAnalysis_);
	return true;
}

void EditorState::setEditMode(int mode)
{
	lastEditMode_ = editMode_;
	editMode_ = EditMode(mode);
	emit editModeChanged(mode);
}

double EditorState::setZoom(double zoom)
{
	if(zoom < .3)
		zoom = .3;
	if(zoom > 2)
		zoom = 2;
	emit zoomChanged(zoom);
	return zoom;
}
void EditorState::setInsertionItem(QString className, QString friendlyName, QString category, QString type, QPixmap pixmap)
{
	insertItemCategory_ = category;
	insertItemType_ = type;
	insertItemPixmap_ = pixmap;
	if(!category.isEmpty() || !type.isEmpty())
		setEditMode(EditorState::PlaceItem);
	else if(getEditMode() == EditorState::PlaceItem)
		setEditMode(EditorState::Select);
	emit insertionItemChanged(className,friendlyName);
}

//Sets the current asset opened in the editor window to the input asset object
//If undoable is true, an undo point will be set after changing the window
//asset (so that undo after a window change will bring you back to the previous
//window and undo after a window change and other action will keep you in the 
//same window).
void EditorState::setWindowAsset(QSharedPointer<Asset> asset,bool undoable)
{
	if(asset.isNull())
		return;
	Q_ASSERT(!asset.dynamicCast<DataStore>().isNull());
	Q_ASSERT(!asset.dynamicCast<UIEnabled>().isNull());
	QSharedPointer<UIEnabled> uiEnabled = asset.staticCast<UIEnabled>();
	//If this asset has no editable descendants, it should not be openable.
	if(!uiEnabled->hasEditableDescendants())
	{
		return;
	}

	//Store the current window asset
	windowAsset_ = asset;

	//Clear any selected items to insert into the scene
	setInsertionItem();

	//Set the currently selected item to null
	setSelectedItem(NULL);
	
	//Tell everyone that the window asset changed.
	emit windowAssetChanged(windowAsset_);

	//Set the window asset as the currently selected asset.
	setSelectedAsset(asset);

	if(undoable)
	{
		//Set this action as undoable so that when people undo the next change they make, they
		//will still be on the window where they made that change, rather than moving back to 
		//the last place where they made an undoable change.
		setLastActionUndoable();
	}
}

void EditorState::setWindowAssetToParent()
{
	if(!getWindowAsset().isNull() 
		&& !getWindowAsset()->getParentAsset().isNull() 
		&& !getWindowAsset()->getParentAsset().dynamicCast<UIEnabled>().isNull())
		setWindowAsset(getWindowAsset()->getParentAsset());
}

void EditorState::setSelectedAsset(QSharedPointer<Asset> asset)
{
	if(selectedAsset_ == asset)
		return;
	selectedAsset_ = asset;
	emit selectedAssetChanged(selectedAsset_);
}

void EditorState::setSelectedItem(QGraphicsItem *item)
{
	AssetItem* assetItem = dynamic_cast<AssetItem*>(item);
	ArrowPortItem* arrowPortItem = dynamic_cast<ArrowPortItem*>(item);

	selectedItem_ = item;
	if(item && item->isSelected())
	{
		if(assetItem)
		{
			setSelectedAsset(assetItem->getAsset());
		}
		else if(arrowPortItem)
		{
			setSelectedAsset(arrowPortItem->getAsset());
		}
		else
		{
			setSelectedAsset(getWindowAsset());
		}
		if(arrowPortItem)
			emit arrowPortSelected(arrowPortItem->getAsset());
	}
	else
	{
		if(		!item 
			|| (assetItem && (assetItem->getAsset() == selectedAsset_)) )
			setSelectedAsset(getWindowAsset());
	}
	emit itemSelected(selectedItem_);
}

void EditorState::reloadWindow()
{
	Q_ASSERT(getWindowAsset());
	Q_ASSERT(getSelectedAsset());
	setInsertionItem();
	emit windowAssetChanged(getWindowAsset());
	emit selectedAssetChanged(getSelectedAsset());
}

void EditorState::requestSearch(SearchRequest searchRequest)
{
	if(searchRequest.type == SearchRequest::INVALID_TYPE || searchRequest.group==SearchRequest::INVALID_GROUP)
		return;
	if(searchRequest.type == SearchRequest::STRING && searchRequest.query == "")
		searchRequest.enabled = false;

	searchRequests_[searchRequest.getGroupTypeIndex()] = searchRequest;
	emit searchRequested(searchRequest);
}
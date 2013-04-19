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
windowAssetPath_("")
{
    //currAnalysis_ = Analysis::Create().staticCast<Analysis>();	//TEMPORARY UNTIL THIS GETS SET FROM SOMEWHERE ELSE!!!!!!!!!!
}

//Returns whether the current window asset is within the task attached to the current analysis
bool EditorState::inAnalysisTask()
{
	if(!currAnalysis_)
		return false;
	if(!currentTask_)
		return false;
	if(currAnalysis_->getLinkedTask().staticCast<Asset>() == currentTask_)
		return true;
	return false;
}

QList<SearchRequest> EditorState::getSearchRequests()
{
	return searchRequests_.values();
}

void EditorState::setTopLevelAsset(QSharedPointer<Picto::Asset> topLevelAsset)
{
	topAsset_ = topLevelAsset;
	Q_ASSERT(!topAsset_.isNull() && topAsset_->inherits("Picto::Experiment"));
	//setWindowAsset(topAsset_);
}

void EditorState::setCurrentAnalysis(QSharedPointer<Analysis> currAnalysis)
{
	if(!topAsset_)
		return;
	if(currAnalysis_ == currAnalysis)
		return;
	if(currAnalysis)
	{
		QSharedPointer<Experiment> exp = topAsset_.staticCast<Experiment>();
		//GET THE TASK AND USE THAT TO LINK THE ANALYSIS TO IT.  THEN DEAL WITH THE FACT THAT THE
		//ANALYSIS CAN ONLY BE ATTACHED TO A SINGLE TASK IN THE UI, NOT THE WHOLE EXPERIMENT.  ALSO,
		//MAKE SURE THAT THE PATHS STORED IN THE TASK DO NOT INCLUDE THE EXPERIMENT NAME!!!
		QSharedPointer<Task> linkableTask = currAnalysis->getLinkableTask(exp);
		if(linkableTask.isNull())
		{	//Allow the user to choose which task to attach the Analysis too.
			bool ok;
			QString taskName = QInputDialog::getItem(NULL,"Task Selection","Select Task to Analyze",exp->getTaskNames(),0,false,&ok);
			if(!ok)
				return;
			linkableTask = exp->getTaskByName(taskName);
			Q_ASSERT(linkableTask);
		}
		
		//Link the analysis to the task
		QString feedback;
		bool linkResult = currAnalysis->LinkToTask(linkableTask,feedback);
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
	emit currentAnalysisChanged(currAnalysis_);
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

void EditorState::setWindowAsset(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
	Q_ASSERT(!asset.dynamicCast<DataStore>().isNull());
	Q_ASSERT(!asset.dynamicCast<UIEnabled>().isNull());
	QSharedPointer<DataStore> dataStore = asset.staticCast<DataStore>();
	
	//Store the current window asset
	windowAsset_ = asset;

	//Check which task we're in and store it.
	QSharedPointer<Asset> bufferAsset = windowAsset_;
	currentTask_.clear();
	do{
		if(bufferAsset->inherits("Picto::Task"))
		{
			currentTask_ = bufferAsset;
			break;
		}
		bufferAsset = bufferAsset->getParentAsset();
	}while(bufferAsset);

	//Clear any selected items to insert into the scene
	setInsertionItem("","");
	
	//Tell everyone that the window asset changed.
	emit windowAssetChanged(windowAsset_);

	//Set the window asset as the currently selected asset.
	setSelectedAsset(asset);
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
		emit itemSelected(item);
	}
	else
	{
		if(		!item 
			|| (assetItem && (assetItem->getAsset() == selectedAsset_)) )
			setSelectedAsset(getWindowAsset());
	}
}

void EditorState::reloadWindow()
{
	Q_ASSERT(getWindowAsset());
	Q_ASSERT(getSelectedAsset());
	setInsertionItem("","");
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
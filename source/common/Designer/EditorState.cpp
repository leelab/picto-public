#include "EditorState.h"
#include <QtWidgets>
#include <QSharedPointer>
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
    
}

QList<SearchRequest> EditorState::getSearchRequests()
{
	return searchRequests_.values();
}

void EditorState::setTopLevelAsset(QSharedPointer<Picto::Asset> topLevelAsset)
{
	topAsset_ = topLevelAsset;
	Q_ASSERT(!topAsset_.isNull());
	//setWindowAsset(topAsset_);
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
	QSharedPointer<DataStore> dataStore = asset.staticCast<DataStore>();
	
	windowAsset_ = asset;
	Q_ASSERT(!asset.dynamicCast<UIEnabled>().isNull());

	setInsertionItem("","");
	emit windowAssetChanged(windowAsset_);
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
	ArrowDestinationItem* destItem = dynamic_cast<ArrowDestinationItem*>(item);
	StartBarItem* startItem = dynamic_cast<StartBarItem*>(item);

	selectedItem_ = item;
	if(item && item->isSelected())
	{
		if(assetItem)
		{
			setSelectedAsset(assetItem->getAsset());
		}
		else if(destItem)
		{
			setSelectedAsset(destItem->getAsset());
		}
		else
		{
			setSelectedAsset(getWindowAsset());
		}
		if(destItem || startItem)
			emit startBarSelected(selectedAsset_);
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
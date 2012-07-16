#include "EditorState.h"
#include <QtGui>
#include <QSharedPointer>
#include "../../common/storage/DataStore.h"
#include "../../common/statemachine/uienabled.h"
#include "../../common/statemachine/uiinfo.h"
#include "AssetItem.h"
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
	selectedItem_ = item;
	if(item && item->isSelected())
	{
		if(assetItem)
			setSelectedAsset(assetItem->getAsset());
		else
			setSelectedAsset(getWindowAsset());
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
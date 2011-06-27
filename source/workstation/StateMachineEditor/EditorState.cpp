#include "EditorState.h"
#include <QtGui>
#include <QSharedPointer>
#include "../../common/storage/DataStore.h"
#include "AssetItem.h"
EditorState::EditorState()
:
editMode_(Select),
font_(),
textColor_(Qt::black),
itemColor_(Qt::white),
lineColor_(Qt::black),
insertItemCategory_(Select),
insertItemType_("")
{
    
}

void EditorState::setWindowAsset(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
	Q_ASSERT(asset.dynamicCast<DataStore>());
	QSharedPointer<DataStore> dataStore = asset.staticCast<DataStore>();
	
	windowAsset_ = asset;
	setInsertionItem("","");
	emit windowAssetChanged(windowAsset_);
	setSelectedAsset(asset);
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
	if(item->isSelected())
	{
		if(assetItem)
			setSelectedAsset(assetItem->getAsset());
		else
			setSelectedAsset(getWindowAsset());
		emit itemSelected(item);
	}
	else
	{
		if(assetItem && assetItem->getAsset() == selectedAsset_)
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
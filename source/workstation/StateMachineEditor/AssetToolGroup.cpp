#include <QtGui>
#include "AssetToolGroup.h"
#include "../../common/storage/DataStore.h"
#include "../../common/storage/PropertyFactory.h"
#include "DiagramItemFactory.h"
using namespace Picto;


AssetToolGroup::AssetToolGroup(QSharedPointer<EditorState> editorState,QSharedPointer<Asset> asset, QWidget *parent) :
	ToolGroup(editorState,parent)
{
	setAsset(asset);
}

void AssetToolGroup::setAsset(QSharedPointer<Asset> asset)
{
	clearButtons();
	elemInfo_.clear();
	asset_ = asset;
	QSharedPointer<DataStore> dataStore = asset.dynamicCast<DataStore>();
	if(!dataStore.isNull())
	{
		QStringList childTags = dataStore->getValidChildTags();
		QSharedPointer<AssetFactory> assetFactory;
		QStringList types;
		foreach(QString childTag,childTags)
		{
			//Don't show tranitions or UIInfo.  Transitions are added
			//using wires.  UIInfo is added automatically.
			if(childTag == "Transition")
				continue;
			if(childTag == "UIInfo")
				continue;
			assetFactory = dataStore->getAssetFactory(childTag);
			if(!assetFactory.isNull() && assetFactory.dynamicCast<PropertyFactory>().isNull())
			{
				types = assetFactory->getTypes();
				foreach(QString type,types)
				{
					QString buttonName = childTag;
					if(type != "")
						buttonName.append(QString("\n(%1)").arg(type));
					AddButton(buttonName,DiagramItemFactory::getIcon(assetFactory->getUITemplate(type)),!assetFactory->reachedProductionLimit(type));
					ElemInfo info;
					info.tag = childTag;
					info.type = type;
					elemInfo_.push_back(info);
				}
			}
		}
	}
}

void AssetToolGroup::doButtonAction(int buttonId)
{
	if(buttonId >= elemInfo_.size())
		return;
	getEditorState()->setInsertionItem(elemInfo_[buttonId].tag,elemInfo_[buttonId].type);
}

void AssetToolGroup::disableButtonActions()
{
	getEditorState()->setInsertionItem("","");
}


#include <QtGui>
#include "PropertyToolGroup.h"
#include "../../common/storage/DataStore.h"
#include "../../common/storage/propertyfactory.h"
using namespace Picto;


PropertyToolGroup::PropertyToolGroup(QSharedPointer<EditorState> editorState,QSharedPointer<Asset> asset, QWidget *parent) :
	ToolGroup(editorState,parent)
{
	setAsset(asset);
}

void PropertyToolGroup::setAsset(QSharedPointer<Asset> asset)
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
			assetFactory = dataStore->getAssetFactory(childTag);
			if(!assetFactory.dynamicCast<PropertyFactory>().isNull())
			{
				types = assetFactory->getTypes();
				foreach(QString type,types)
				{
					QString buttonName = childTag;
					if(type != "")
						buttonName.append(QString("\n(%1)").arg(type));
					AddButton(buttonName,QIcon(),!assetFactory->reachedProductionLimit());
					ElemInfo info;
					info.tag = childTag;
					info.type = type;
					elemInfo_.push_back(info);
				}
			}
		}
	}
}

void PropertyToolGroup::doButtonAction(int buttonId)
{
	if(buttonId >= elemInfo_.size())
		return;
	QString category = elemInfo_[buttonId].tag;
	if(category == "")
		return;
	QString type = elemInfo_[buttonId].type;
	QString errorStr;
	QSharedPointer<DataStore> dataStore = getEditorState()->getWindowAsset().staticCast<DataStore>();
	dataStore->createChildAsset(category,type,errorStr);
	getEditorState()->setLastActionUndoable();
}

void PropertyToolGroup::disableButtonActions()
{
	getEditorState()->setInsertionItem("","");
}


#include <QtWidgets>
#include "AssetToolGroup.h"
#include "../../common/storage/DataStore.h"
#include "../../common/storage/PropertyFactory.h"
#include "DiagramItemFactory.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new AssetToolGroup for a Designer with the input editorState and where the current Window Asset is input
 * in asset.
 *	\details parent is a parent widget.  displayedUIGroups is a QStringList of UIGroup names that may be displayed in this
 *	AssetToolGroup.  Only child types for the input asset with a UIGroup found in displayedUIGroups will have a ToolButton 
 *	displayed in this AssetToolGroup.
 *	/sa AssetFactory::getUIGroup()
 */
AssetToolGroup::AssetToolGroup(QSharedPointer<EditorState> editorState,QStringList displayedUIGroups,QSharedPointer<Asset> asset, QWidget *parent) :
	ToolGroup(editorState,parent)
{
	//Initialize UI Group hash
	foreach(QString group,displayedUIGroups)
	{
		uIGroupMap_[group]  = true;
	}

	setAsset(asset);
}
/*! \brief Sets the current Window Asset within which any assets created using this AssetToolGroup will be added as children.
 */
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
			assetFactory = dataStore->getAssetFactory(childTag);
			if(!assetFactory.isNull() && assetFactory.dynamicCast<PropertyFactory>().isNull())
			{
				types = assetFactory->getTypes();
				foreach(QString type,types)
				{
					//If the user is not allowed to generate assets of this type using this factory, move on to the next type.
					if(assetFactory->getMaxAssets(type) == assetFactory->getMinAssets(type))
						continue;
					QString uiTemplate = assetFactory->getUITemplate(type);
					if(uiTemplate.isEmpty())	//This will be the case for objects that don't inherit from UIEnabled
						continue;
					QString uIGroup = assetFactory->getUIGroup(type);
					if(!isDisplayedGroup(uIGroup))
						continue;
					QString buttonName = assetFactory->getGeneratedAssetTypeName(type);
					//if(type != "")
					//	buttonName.append(QString("\n(%1)").arg(type));
					AddButton(buttonName,DiagramItemFactory::getIcon(assetFactory->getUITemplate(type)),!assetFactory->reachedProductionLimit(type));
					ElemInfo info;
					info.tag = childTag;
					info.type = type;
					info.assetFactory = assetFactory;
					elemInfo_.push_back(info);
				}
			}
		}
	}
}

/*! \brief Returns true if this AssetToolGroup contains no buttons.*/
bool AssetToolGroup::isEmpty()
{
	return (numButtons() <= 0);
}

void AssetToolGroup::doButtonAction(int buttonId)
{
	if(buttonId >= elemInfo_.size())
		return;
	if(getEditorState()->getEditMode() != EditorState::DrawLine)
	{
		QString className = "";
		QString friendlyName = "";
		if(elemInfo_[buttonId].assetFactory)
		{
			className = elemInfo_[buttonId].assetFactory->getGeneratedAssetClassName(elemInfo_[buttonId].type);
			friendlyName  = elemInfo_[buttonId].assetFactory->getGeneratedAssetTypeName(elemInfo_[buttonId].type);
		}
		//Tells the EditorState that the next time someone wants to insert something into the canvas, it should be an asset with
		//this buttons className, icon, etc.
		getEditorState()->setInsertionItem(className,friendlyName,elemInfo_[buttonId].tag,elemInfo_[buttonId].type,getButtonPixmap(buttonId));
	}
	else
		disableButtonActions();
}

void AssetToolGroup::disableButtonActions()
{
	getEditorState()->setInsertionItem();
}

bool AssetToolGroup::isEnabled(int buttonId)
{
	Q_ASSERT((buttonId >= 0) && (buttonId < elemInfo_.size()));
	if(elemInfo_[buttonId].assetFactory->reachedProductionLimit(elemInfo_[buttonId].type))
		return false;
	return true;
}
/*! \brief Returns true if Assets with the input UIGroup name should be displayed in this AssetToolGroup.
 *	\sa AssetToolGroup::AssetToolGroup()
 */
bool AssetToolGroup::isDisplayedGroup(QString UIGroup)
{
	return uIGroupMap_.contains(UIGroup);
}

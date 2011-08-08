#include "DiagramItemFactory.h"
#include "ResultItem.h"
#include "StateMachineElementItem.h"
#include "ControlElementItem.h"
#include "AssetItem.h"
#include "WireableItem.h"
#include "../../common/property/property.h"
#include "../../common/statemachine/uienabled.h"
#include <string>
using namespace std;

QMap<QString,IconDef> DiagramItemFactory::iconDefs_;
bool DiagramItemFactory::mapInitialized_ = false;

DiagramItemFactory::DiagramItemFactory(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QGraphicsScene *scene)
{
	if(!mapInitialized_)
	{
		addIconDefinition("UIEnabled",":/icons/statemachineelement.svg",50,100);
		addIconDefinition("StateMachineElement",":/icons/statemachineelement.svg",50,100);
		addIconDefinition("StateMachine",":/icons/statemachine.svg",50,100);
		addIconDefinition("State",":/icons/state.svg",100,100);
		addIconDefinition("ScriptElement",":/icons/scriptelement.svg",50,75);
		addIconDefinition("Scriptable",":/icons/scriptable.svg",50,75);
		addIconDefinition("Result",":/icons/result.svg",50,75);
		addIconDefinition("Reward",":/icons/reward.svg",50,100);
		addIconDefinition("Task",":/icons/task.svg",100,100);
		mapInitialized_ = true;
	}
	editorState_ = editorState;
	contextMenu_ = contextMenu;
	scene_ = scene;
}

DiagramItem* DiagramItemFactory::create(QSharedPointer<Asset> asset)
{
	AssetItem* returnVal = NULL;
	if(asset.isNull())
		returnVal =  NULL;
	QString type = asset->assetType();
	//Note that the order here is very important since, for example, StateMachineElements inherit Asset.
	if(asset->inherits("Picto::Property"))
		returnVal = NULL;
	else if(asset->inherits("Picto::Transition"))
		returnVal = NULL;
	else if(asset->inherits("Picto::ControlLink"))
		returnVal = NULL;
	else if(asset->inherits("Picto::UIInfo"))
		returnVal = NULL;
	else if(asset->inherits("Picto::Result"))
		returnVal = new ResultItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::StateMachineElement"))
		returnVal = new StateMachineElementItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::ControlElement"))
		returnVal = new ControlElementItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::UIEnabled"))
		returnVal = new WireableItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::Asset"))
		returnVal = new AssetItem(editorState_, contextMenu_,asset);
	else
		Q_ASSERT_X(false,"DiagramItemFactory::create","Unknown Item Type: " + type.toAscii());

	if(returnVal)
	{
		IconDef iconDef = getIconDef(asset);
		QRectF iconRect(-iconDef.width_/2.0,-iconDef.height_/2.0,iconDef.width_,iconDef.height_);
		returnVal->setRect(iconRect);
		returnVal->setSvgIcon(iconDef.fileName_);
		QSharedPointer<UIInfo> uiInfo = getUIInfo(asset);
		if(!uiInfo.isNull())
			returnVal->setPos(uiInfo.staticCast<UIInfo>()->getPos());
	}
	return returnVal;
}

QIcon DiagramItemFactory::getIcon(QString uITemplate)
{
	Q_ASSERT_X(iconDefs_.contains(uITemplate),"DiagramItemFactory::getIconDef","Unknown UI Template requested.");
	return QIcon(iconDefs_[uITemplate].fileName_);
}

void DiagramItemFactory::addIconDefinition(QString assetType, QString fileName, float width, float height)
{
	iconDefs_[assetType] = IconDef(fileName,width,height);
}

IconDef DiagramItemFactory::getIconDef(QSharedPointer<Asset> asset)
{
	Q_ASSERT_X(!asset.dynamicCast<UIEnabled>().isNull(),"DiagramItemFactory::getIconDef","Only assets that are UIEnabled can used in the DiagramItemFactory.");
	QSharedPointer<UIEnabled> uiEnabled = asset.staticCast<UIEnabled>();
	Q_ASSERT_X(iconDefs_.contains(uiEnabled->getUITemplate()),"DiagramItemFactory::getIconDef","Unknown UI Template for this Asset.");
	return iconDefs_[uiEnabled->getUITemplate()];
}

QSharedPointer<UIInfo> DiagramItemFactory::getUIInfo(QSharedPointer<Asset> asset)
{
	Q_ASSERT_X(!asset.dynamicCast<UIEnabled>().isNull(),"DiagramItemFactory::getIconDef","Only assets that are UIEnabled can used in the DiagramItemFactory.");
	QSharedPointer<UIEnabled> uiEnabled = asset.staticCast<UIEnabled>();
	QList<QSharedPointer<Asset>> UIInfoList = uiEnabled->getGeneratedChildren("UIInfo");
	Q_ASSERT(UIInfoList.size());
	return UIInfoList.first().staticCast<UIInfo>();
}
#include "DiagramItemFactory.h"
#include "ResultItem.h"
#include "StateMachineElementItem.h"
#include "ControlElementItem.h"
#include "StateItem.h"
#include "StateMachineItem.h"
#include "RewardItem.h"
#include "ScriptElementItem.h"
#include "TaskItem.h"
#include "WireableItem.h"
#include "../../common/property/property.h"
#include <string>
using namespace std;

DiagramItemFactory::DiagramItemFactory(QMenu *contextMenu, QGraphicsScene *scene)
{
	contextMenu_ = contextMenu;
	scene_ = scene;
}

DiagramItem* DiagramItemFactory::create(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return NULL;
	QString type = asset->assetType();
	if(type == "Property")
	{
		if(asset.staticCast<Property>()->name() == "Result")
			return new ResultItem(contextMenu_,asset);
	}
	else if(type == "DataStore")
		return new WireableItem(contextMenu_,asset);
	else if(type == "StateMachineElement")
		return new StateMachineElementItem(contextMenu_,asset);
	else if(type == "State")
		return new StateItem(contextMenu_,asset);
	else if(type == "StateMachine")
		return new StateMachineItem(contextMenu_,asset);
	else if(type == "ControlElement")
		return new ControlElementItem(contextMenu_,asset);
	else if(type == "Reward")
		return new RewardItem(contextMenu_,asset);
	else if(type == "ScriptElement")
		return new ScriptElementItem(contextMenu_,asset);
	else if(type == "Task")
		return new TaskItem(contextMenu_,asset);
	return NULL;
}
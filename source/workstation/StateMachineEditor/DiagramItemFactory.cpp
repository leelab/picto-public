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
#include "../../common/statemachine/uienabled.h"
#include "../../common/statemachine/uiinfo.h"
#include <string>
using namespace std;

DiagramItemFactory::DiagramItemFactory(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QGraphicsScene *scene)
{
	editorState_ = editorState;
	contextMenu_ = contextMenu;
	scene_ = scene;
}

DiagramItem* DiagramItemFactory::create(QSharedPointer<Asset> asset)
{
	DiagramItem* returnVal = NULL;
	if(asset.isNull())
		returnVal =  NULL;
	QString type = asset->assetType();
	if(type == "UIInfo")
		returnVal =  NULL;
	else if(type == "UIEnabled")
		returnVal = new WireableItem(editorState_, contextMenu_,asset);
	else if(type == "Property")
		returnVal =  NULL;
	else if(type == "Transition")
		returnVal =  NULL;
	else if(type == "ControlLink")
		returnVal =  NULL;
	else if(type == "Result")
		returnVal =  new ResultItem(editorState_, contextMenu_,asset);
	else if(type == "DataStore")
		returnVal =  new WireableItem(editorState_, contextMenu_,asset);
	else if(type == "StateMachineElement")
		returnVal =  new StateMachineElementItem(editorState_, contextMenu_,asset);
	else if(type == "State")
		returnVal =  new StateItem(editorState_, contextMenu_,asset);
	else if(type == "StateMachine")
		returnVal =  new StateMachineItem(editorState_, contextMenu_,asset);
	else if(type == "ControlElement")
		returnVal =  new ControlElementItem(editorState_, contextMenu_,asset);
	else if(type == "Reward")
		returnVal =  new RewardItem(editorState_, contextMenu_,asset);
	else if(type == "ScriptElement")
		returnVal =  new ScriptElementItem(editorState_, contextMenu_,asset);
	else if(type == "Task")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "ParameterContainer")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "Scene")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "VisualElement")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "Parameter")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "AudioElement")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "PredicateExpression")
		returnVal =  new ResultItem(editorState_, contextMenu_,asset);
	else if(type == "FlowElement")
		returnVal =  new StateMachineItem(editorState_, contextMenu_,asset);
	else if(type == "CompoundExpression")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else if(type == "ControlTarget")
		returnVal =  new TaskItem(editorState_, contextMenu_,asset);
	else
		Q_ASSERT_X(false,"DiagramItemFactory::create","Unknown Item Type: " + type.toAscii());
	QSharedPointer<UIEnabled> uiEnabled = asset.dynamicCast<UIEnabled>();
	if(returnVal && !uiEnabled.isNull())
	{
		QSharedPointer<Asset> uiInfo = uiEnabled->getGeneratedChildren("UIInfo").first();
		if(!uiInfo.isNull())
			returnVal->setPos(uiInfo.staticCast<UIInfo>()->getPos());
	}
	return returnVal;
}
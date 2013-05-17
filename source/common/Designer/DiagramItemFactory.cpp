#include <string>
#include <QGraphicsScene>
#include "DiagramItemFactory.h"
#include "ResultItem.h"
#include "WireableResultItem.h"
#include "StateMachineElementItem.h"
#include "ControlElementItem.h"
#include "AssetItem.h"
#include "WireableItem.h"
#include "../../common/property/property.h"
#include "../../common/statemachine/uienabled.h"
#include "../../common/statemachine/result.h"
#include "../../common/memleakdetect.h"
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
		addIconDefinition("PausePoint",":/icons/pausepoint.svg",100,100);
		addIconDefinition("ScriptElement",":/icons/scriptelement.svg",50,75);
		addIconDefinition("SwitchElement",":/icons/scriptelement.svg",50,75);
		addIconDefinition("Parameter",":/icons/parameter.svg",50,75);
		addIconDefinition("Variable",":/icons/variable.svg",50,75);
		addIconDefinition("ScriptFunction",":/icons/scriptfunction.svg",50,75);
		addIconDefinition("ControlTarget",":/icons/controltarget.svg",75,75);
		addIconDefinition("ControlElement",":/icons/controlelement.svg",75,100);
		addIconDefinition("VisualElement",":/icons/visualelement.svg",50,75);
		addIconDefinition("Scriptable",":/icons/scriptable.svg",50,75);
		addIconDefinition("Result",":/icons/result.svg",50,75);
		addIconDefinition("Reward",":/icons/reward.svg",50,100);
		addIconDefinition("Sensor",":/icons/sensor.svg",50,75);
		addIconDefinition("RandomGenerator",":/icons/random.svg",50,75);
		addIconDefinition("OperatorClick",":/icons/mouseclick.svg",50,75);
		addIconDefinition("Task",":/icons/task.svg",100,100);
		//Analysis Icons
		addIconDefinition("DataSource",":/icons/datasource.svg",100,100);
		addIconDefinition("ElementDataSource",":/icons/statemachineelement.svg",100,100);
		addIconDefinition("FrameDataSource",":/icons/framedatasource.svg",100,100);
		addIconDefinition("LFPDataSource",":/icons/lfpdatasource.svg",100,100);
		addIconDefinition("PropertyDataSource",":/icons/propertydatasource.svg",100,100);
		addIconDefinition("SignalDataSource",":/icons/datasource.svg",100,100);
		addIconDefinition("SpikeDataSource",":/icons/spikedatasource.svg",100,100);
		addIconDefinition("TimeDataSource",":/icons/timedatasource.svg",100,100);
		addIconDefinition("AnalysisOutput",":/icons/analysisoutput.svg",100,100);
		addIconDefinition("AnalysisPeriod",":/icons/analysisperiod.svg",100,100);
		addIconDefinition("AnalysisTool",":/icons/analysistool.svg",100,100);
		addIconDefinition("AnalysisTrigger",":/icons/analysistrigger.svg",100,100);
		addIconDefinition("ElementTrigger",":/icons/elementtrigger.svg",100,100);
		addIconDefinition("FrameTrigger",":/icons/frametrigger.svg",100,100);
		addIconDefinition("LFPTrigger",":/icons/lfptrigger.svg",100,100);
		addIconDefinition("PropertyTrigger",":/icons/propertytrigger.svg",100,100);
		addIconDefinition("SignalTrigger",":/icons/signaltrigger.svg",100,100);
		addIconDefinition("SpikeTrigger",":/icons/spiketrigger.svg",100,100);
		addIconDefinition("TimeTrigger",":/icons/timetrigger.svg",100,100);
		addIconDefinition("TransitionTrigger",":/icons/transitiontrigger.svg",100,100);
		addIconDefinition("FileOutput",":/icons/fileoutput.svg",100,100);
		addIconDefinition("NumericVariable",":/icons/numericvariable.svg",100,100);
		addIconDefinition("StarterContainer",":/icons/startercontainer.svg",100,100);
		addIconDefinition("EnderContainer",":/icons/endercontainer.svg",100,100);
		addIconDefinition("AnalysisDefinition",":/icons/analysisdefinition.svg",100,100);
		addIconDefinition("OutputSignal",":/icons/outputsignal.svg",100,100);
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
	//Note that the order here is very important since, for example, StateMachineElements inherit Asset.
	if(asset->inherits("Picto::Property"))
		returnVal = NULL;
	else if(asset->inherits("Picto::Transition"))
		returnVal = NULL;
	else if(asset->inherits("Picto::ControlLink"))
		returnVal = NULL;
	else if(asset->inherits("Picto::UIInfo"))
		returnVal = NULL;
	else if(asset->inherits("Picto::LogicResult"))
		returnVal = new WireableResultItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::Result"))
		returnVal = new WireableResultItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::StateMachineElement"))
		returnVal = new StateMachineElementItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::ControlElement"))
		returnVal = new ControlElementItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::UIEnabled"))
		returnVal = new WireableItem(editorState_, contextMenu_,asset);
	else if(asset->inherits("Picto::Asset"))
		returnVal = new AssetItem(editorState_, contextMenu_,asset);
	else
		Q_ASSERT_X(false,"DiagramItemFactory::create",QString("Unknown Item Type: %1").arg(asset->metaObject()->className()).toLatin1());

	if(returnVal)
	{
		IconDef iconDef = getIconDef(asset);
		QRectF iconRect(-iconDef.width_/2.0,-iconDef.height_/2.0,iconDef.width_,iconDef.height_);
		returnVal->setRect(iconRect);
		returnVal->setSvgIcon(iconDef.fileName_);
		QSharedPointer<UIEnabled> uiEnabled = asset.dynamicCast<UIEnabled>();
		if(uiEnabled)
		{
			QPointF pos = uiEnabled->getPos();
			if(!scene_->sceneRect().contains(pos))
				pos = scene_->sceneRect().center();
			returnVal->setPos(pos);
		}
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
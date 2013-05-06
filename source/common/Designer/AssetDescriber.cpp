#include "AssetDescriber.h"

bool AssetDescriber::isSetup_ = false;
QHash<QString,QSharedPointer<AssetDescription>> AssetDescriber::assetHash_;

void removeCopies(QStringList& stringList)
{
	for(QStringList::Iterator iter = stringList.begin();iter != stringList.end();iter++)
	{
		if(iter + 1 != stringList.end())
		{
			if((*iter) == (*(iter+1)))
				stringList.erase(iter+1);
		}
	}
}

QStringList AssetDescription::getProperties()
{
	QStringList returnVal;
	if(props.size())
		returnVal.append(props.keys());
	if(inherits)
		returnVal.append(inherits->getProperties());
	qStableSort(returnVal);
	removeCopies(returnVal);
	return returnVal;
}
QStringList AssetDescription::getScriptProperties()
{
	QStringList returnVal;
	if(scriptProps.size())
		returnVal.append(scriptProps.keys());
	if(inherits)
		returnVal.append(inherits->getScriptProperties());
	qStableSort(returnVal);
	removeCopies(returnVal);
	return returnVal;
}
QStringList AssetDescription::getScriptFunctions()
{
	QStringList returnVal;
	if(scriptFunctions.size())
		returnVal.append(scriptFunctions.keys());
	if(inherits)
		returnVal.append(inherits->getScriptFunctions());
	qStableSort(returnVal);
	removeCopies(returnVal);
	return returnVal;
}
QString AssetDescription::getPropertyDescription(QString name)
{
	if(props.contains(name))
		return props.value(name);
	if(inherits)
		return inherits->getPropertyDescription(name);
	return "";
}
QString AssetDescription::getScriptPropertyDescription(QString name)
{
	if(props.contains(name))
		return scriptProps.value(name);
	if(inherits)
		return inherits->getScriptPropertyDescription(name);
	return "";
}
QString AssetDescription::getScriptFunctionDescription(QString name)
{
	if(props.contains(name))
		return scriptFunctions.value(name);
	if(inherits)
		return inherits->getScriptFunctionDescription(name);
	return "";
}

QSharedPointer<AssetDescription> AssetDescriber::getAssetDescription(QString className)
{
	if(!isSetup_)
	{
		isSetup_ = true;
		setupDescriptions();
	}
	if(!assetHash_.contains(className))
		return QSharedPointer<AssetDescription>();
	return assetHash_.value(className);
}

QSharedPointer<AssetDescription> AssetDescriber::addDescription(QString className)
{
	QSharedPointer<AssetDescription> newDescription(new AssetDescription());
	Q_ASSERT(!assetHash_.contains(className));
	assetHash_[className] = newDescription;
	return newDescription;
}
void AssetDescriber::setupDescriptions()
{
	QSharedPointer<AssetDescription> curr;
	
	//Inherited Elements
	curr = addDescription("UIEnabled");
	curr->addProp("Name","Defines the name of this element.");

	curr = addDescription("Scriptable");
	curr->setInherits(getAssetDescription("UIEnabled"));
	curr->addProp("Name","Defines the name of this element.  When this element is accessed in scripts, this is the name used.");
	curr->addProp("UIEnabled","Defines whether run-time editable properties from this element will appear during task runs.");
	//curr->addProp("UIOrder","Not Yet Implemented.  Used to define the order of run-time editable properties");
	curr = addDescription("ScriptableContainer");
	curr->setInherits(getAssetDescription("Scriptable"));
	curr = addDescription("OutputElement");
	curr->setInherits(getAssetDescription("Scriptable"));
	curr = addDescription("Parameter");
	curr->setInherits(getAssetDescription("Scriptable"));
	curr = addDescription("Variable");
	curr->setInherits(getAssetDescription("Scriptable"));

	curr = addDescription("OutputSignal");
	curr->setInherits(getAssetDescription("OutputElement"));
	curr->addProp("Port","Defines the initial name of the physical output port where the voltage signal will be produced.  Currently on a standard Pictobox BNC0 (BNC lines 1-8) and PAR0 (8 Parallel lines in 20 pin rear output) are supported.");
	curr->addProp("Enabled","Defines whether pins controlled by this element are initially active.  When set false, regardless of other settings, they will have voltage of zero.");
	curr->addSProp("enabled","Sets or gets whether pins controlled by this element are currently active.  When set false, regardless of other settings, they will have voltage of zero.");

	curr = addDescription("VisualElement");
	curr->setInherits(getAssetDescription("OutputElement"));
	curr->addProp("Position","Defines the initial x,y position of this graphic.  x goes from 0 to 800 and y goes from 0 to 600 where (0,0) is the top left corner of the screen.");
	curr->addProp("Color","Defines the initial color of this graphic.  Color is made up of red, green, blue and alpha components where alpha defines the color opacity (0=transparent,255=opaque).");
	curr->addProp("Layer","Defines the inital z-layer of this graphic in the scene.  Graphics with higher layer will appear on top of graphics with lower layer.");
	curr->addProp("Visible","Defines whether this graphic is initially visible to the operator and user as a group.");
	curr->addProp("OperatorView","When Visible is true, defines whether this graphic is initially visible to the operator.");
	curr->addProp("SubjectView","When Visible is true, defines whether this graphic is initially visible to the user.");
	curr->addSProp("layer","Used to set or get the current layer of this graphic.  Graphics with higher layer will appear on top of graphics with lower layer.");
	curr->addSProp("visible","Used to set or get the current visibility of this graphic to the operator and user as a group.");
	curr->addSProp("operatorView","Used to set or get the current visibility of this graphic to the operator.");
	curr->addSProp("subjectView","Used to set or get the current visibility of this graphic to the user.");
	curr->addSProp("x","Used to set or get the x position of this graphic where 0 is the left side of the screen and 800 is the right side.");
	curr->addSProp("y","Used to set or get the y position of this graphic where 0 is the top of the screen and 600 is the bottom.");
	curr->addSProp("red","Used to set or get the red component of this graphics color.  Ranges from 0-255.");
	curr->addSProp("green","Used to set or get the green component of this graphics color.  Ranges from 0-255.");
	curr->addSProp("blue","Used to set or get the blue component of this graphics color.  Ranges from 0-255.");
	curr->addSProp("alpha","Used to set or get the alpha component of this graphics color where alpha defines the color opacity (0=transparent,255=opaque).");
	curr->addSFunc("setColor","A convenience function to set the rgb or rgba color components at once.  Inputs are (r,g,b) or (r,g,b,a) with each input ranging from 0-255.");
	curr->addSFunc("setPos","A convenience function to set both the x and y components of position at once.  Inputs are (x,y) and range from 0-800 and 0-600 respectively.");

	curr = addDescription("ResultContainer");
	curr->setInherits(getAssetDescription("ScriptableContainer"));

	curr = addDescription("StateMachineElement");
	curr->setInherits(getAssetDescription("ResultContainer"));
	curr->addProp("EntryScript","A script that can be used to transfer data between elements.  When execution transitions to this element, all properties contained in this element are set to their initial values and then this script runs.");
	curr->addProp("ExitScript","A script that can be used to transfer data between elements.  When execution is about to transition away from this element, just after the EntryScript runs on the executed result, this script runs.");
	curr->addProp("AnalysisEntryScript","When an analysis is active, this script is used to analyze experimental data.  Conceptually, this script runs just before the the experimental EntryScript.  In practice, it does not run on the PictoDirector but only on session 'viewers' and actually runs just before the effects of the experimental EntryScript are implemented.");
	curr->addProp("AnalysisExitScript","When an analysis is active, this script is used to analyze experimental data.  Conceptually, this script runs just after the the experimental ExitScript.  In practice, it does not run on the PictoDirector but only on session 'viewers' and actually runs just after the effects of the experimental ExitScript are implemented.");

	curr = addDescription("ControlElement");
	curr->setInherits(getAssetDescription("ResultContainer"));

	curr = addDescription("OutputElementContainer");
	curr->setInherits(getAssetDescription("StateMachineElement"));

	curr = addDescription("MachineContainer");
	curr->setInherits(getAssetDescription("OutputElementContainer"));
	curr->addProp("UIEnabled","");

	//Elements of State Machine Logic
	curr = addDescription("Experiment");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr = addDescription("Task");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->addProp("UIEnabled","");
	curr = addDescription("Result");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->addProp("UIEnabled","");
	curr = addDescription("LogicResult");
	curr->setInherits(getAssetDescription("Result"));
	curr = addDescription("PausePoint");
	curr->setInherits(getAssetDescription("OutputElementContainer"));
	curr = addDescription("Reward");
	curr->setInherits(getAssetDescription("StateMachineElement"));
	curr = addDescription("State");
	curr->setInherits(getAssetDescription("MachineContainer"));
	curr = addDescription("StateMachine");
	curr->setInherits(getAssetDescription("MachineContainer"));
	curr = addDescription("SwitchElement");
	curr->setInherits(getAssetDescription("StateMachineElement"));
	curr->addProp("UIEnabled","");
	
	//Control Logic State Machine Elements
	curr = addDescription("ChoiceController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr = addDescription("ScriptController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr->addProp("UIEnabled","");
	curr = addDescription("StopwatchController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr = addDescription("TargetController");
	curr->setInherits(getAssetDescription("ControlElement"));

	//Variables
	curr = addDescription("Var");
	curr->setInherits(getAssetDescription("Variable"));
	curr = addDescription("VarList");
	curr->setInherits(getAssetDescription("Variable"));
	curr = addDescription("VarMap");
	curr->setInherits(getAssetDescription("Variable"));
	
	//Parameters
	curr = addDescription("BooleanParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("ColorParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("DoubleParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("NumericParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("OperatorClickParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("PseudorandomIntParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("RandomDoubleParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("RandomIntParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("RangeParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("SignalValueParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr = addDescription("TimerParameter");
	curr->setInherits(getAssetDescription("Parameter"));

	//Logic Elements
	curr = addDescription("CircleTarget");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("RectTarget");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("ScriptFunction");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->addProp("UIEnabled","");

	//Stimulus Elements
	curr = addDescription("BinaryDataOutput");
	curr->setInherits(getAssetDescription("OutputSignal"));
	curr = addDescription("DigitalOutput");
	curr->setInherits(getAssetDescription("OutputSignal"));
	curr = addDescription("ArrowGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("BoxGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("CircleGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("DiamondGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("EllipseGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("GridGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("LineGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("OperatorInfoGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("PictureGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("RandomlyFilledGridGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("ShapeShifterGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("TextGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr = addDescription("TokenTrayGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
}
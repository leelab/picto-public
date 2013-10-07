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

QString AssetDescription::getOverview()
{
	return overview;
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
	if(scriptProps.contains(name))
		return scriptProps.value(name);
	if(inherits)
		return inherits->getScriptPropertyDescription(name);
	return "";
}
QString AssetDescription::getScriptFunctionDescription(QString name)
{
	if(scriptFunctions.contains(name))
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
	curr->addProp("Name","The name of this element.");

	curr = addDescription("Scriptable");
	curr->setInherits(getAssetDescription("UIEnabled"));
	curr->addProp("Name","The name of this element.  When this element is accessed in scripts, this is the name used.");
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
	curr->addProp("Port","The name of the physical output port where the voltage signal will be produced.  Currently on a standard Pictobox BNC0 (BNC lines 1-8) and PAR0 (8 Parallel lines in 20 pin rear output) are supported.");
	curr->addProp("Enabled","Defines whether pins controlled by this element are active.  When set false, regardless of other settings, they will have voltage of zero.");
	curr->addSProp("enabled","Sets or gets whether pins controlled by this element are currently active.  When set false, regardless of other settings, they will have voltage of zero.");

	curr = addDescription("VisualElement");
	curr->setInherits(getAssetDescription("OutputElement"));
	curr->addProp("Position","The x,y position of this graphic.  x goes from 0 to 800 and y goes from 0 to 600 where (0,0) is the top left corner of the screen.");
	curr->addProp("Color","The  color of this graphic.  Color is made up of red, green, blue and alpha components where alpha defines the color opacity (0=transparent,255=opaque).");
	curr->addProp("Layer","The z-layer of this graphic in the scene.  Graphics with higher layer will appear on top of graphics with lower layer.");
	curr->addProp("Visible","Defines whether this graphic is visible to either the operator, user or both depending on whether 'OperatorView' and/or 'SubjectView' are true.");
	curr->addProp("OperatorView","When 'Visible' is true, defines whether this graphic is visible to the operator.");
	curr->addProp("SubjectView","When 'Visible' is true, defines whether this graphic is visible to the user.");
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
	curr->addSProp("color","Sets/Gets the current color value as a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("setColor(r,g,b) or setColor(r,g,b,a)","A convenience function to set the rgb or rgba color components at once.  Inputs are (r,g,b) or (r,g,b,a) with each input ranging from 0-255.");
	curr->addSFunc("setPos(x,y)","A convenience function to set both the x and y components of position at once.  Inputs are (x,y) and range from 0-800 and 0-600 respectively.");

	curr = addDescription("AudioElement");
	curr->setInherits(getAssetDescription("OutputElement"));
	curr->setOverview("This element is used to play audio from an audio file during the course of an experiment.  Each audio element represents a single sound channel such that multiple audio elements can be played simultaneoulsy. At any point in the experiment, the play()/stop() functions of any audio elements that are in scope can be called to control the audio channel of those elements.  Audio elements only play sound if their play() function is called and continue until the audio file ends, stop() is called or the Audio Element goes out of scope.  Calling play() on an already playing audio file causes playback to restart from the beginning.  The file to be played back is set by entering the name of an 'in scope' Audio File parameter to the AudioFile property.  It can also be set programatically using the 'file' script property, in which case a string with the AudioFile parameter's name should be set to the script property.  If the Audio Element's AudioFile changes during playback of a previous file, the previous file's sound will stop.  The isPlaying() funciton can be used to find out if an audio file was playing at the time that the last frame was presented.  To avoid lags when switching the Audio File of an Audio element, make sure to precache any Audio Files that will be used by this Audio Element at the beginning of the experiment by using the cacheFile(fileElementName) function.  Setting the volume for an individual audio element is not yet supported.");
	curr->addProp("AudioFile","The name of an Audio File Parameter whose audio file contents will be played when this element's play() function is called.  The Audio File Parameter must be in scope.");
	curr->addProp("UIEnabled","");
	curr->addSProp("file","Sets/Gets the name of an Audio File Parameter whose audio file contents will be played when this element's play() function is called.  The name of the Audio File Parameter should be in string form (ie. surrounded by quotes).  When the audio file is changed, if the previously set file was playing it will be stopped upon presenting the next frame.");
	curr->addSFunc("play()","Starts playback of the audio File.  If play() is called while the audio file is already playing back, it will restart from the beginning.");
	curr->addSFunc("stop()","Stops playback of the audio file.");
	curr->addSFunc("isPlaying()","Returns true if the audio file was playing at the time the last frame was presented, false otherwise.  This is useful to check whether audio is playing in cases where an audio file is not stopped manually but allowed to play to the end.");
	curr->addSFunc("cacheFile(fileElementName)","If an audio file is set to an Audio Element for the first time during the course of an experiment, the element must perform the overhead necessary to create a new underlying 'sound' and this could cause some lag in experimental execution.  The system cache's the sound from the Audio Element stored in the AudioFile field automatically, but it can't predict what other Audio Element's sound it might need to cache.  To avoid this, call cacheFile(fileElementName) with each additional file that can be played back in this audio element at the beginning of the experiment so that the element can preload all sounds that it will need to play.");

	curr = addDescription("ResultContainer");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->addSFunc("getLatestResult())","Returns the name of the latest result that was triggered by this element.  If no result has been triggered since this element's parent's execution started, an empty string is returned.");

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
	curr->setOverview("This is the top level element of the design.  Experiments act as containers for the different the individual tasks that can be run by the user.  They also contain a few properties whose definitions span multiple tasks, namely the eye position calibration properties.  Unlike all other properties in Picto whose initial values are only applied when their containers enter execution scope, due to the necessity for high speed reaction in eye position calibration, these properties take effect immediately when set by the operator during a session run.  Also, unlike other elements in Picto, the Experiment element does not have a user defined name.  This allows descendants in the tree to access its scriptable components without needing to know the name of the experiment that the task is in.  Experimental script properties and functions are therefore accessed by using the 'Experiment.scriptProperty' notation.");
	curr->addProp("Name","");
	curr->addProp("UIEnabled","");
	curr->addProp("XGain","A factor by which horizontal eye input is multiplied in calculating the final eye's x coordinate in pixel space.");
	curr->addProp("YGain","A factor by which veritcal eye input is multiplied in calculating the final eye's y coordinate in pixel space.");
	curr->addProp("XOffset","An offset value that is added to the horizontal eye input before it is multiplied by xGain to calculate the eye's x coordinate in pixel space.");
	curr->addProp("YOffset","An offset value that is added to the vertical eye input before it is multiplied by yGain to calculate the eye's y coordinate in pixel space.");
	curr->addProp("XYSignalShear","A shear value used to shear the horizontal eye input as a function of vertical y input.  Hopefully, you won't need to use this often.");
	curr->addSProp("xGain","Sets/Gets the value of XGain.  See that property.");
	curr->addSProp("yGain","Sets/Gets the value of YGain.  See that property.");
	curr->addSProp("xOffset","Sets/Gets the value of XOffset.  See that property.");
	curr->addSProp("yOffset","Sets/Gets the value of YOffset.  See that property.");
	curr->addSProp("xySignalShear","Sets the value of XYSignalShear.  See that property.");

	curr = addDescription("Task");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->setOverview("This is the top level runnable element of a session.  When a session is started, the operator must choose a task to run.  Each task run is considered a separate entity by Picto.  When analyzing, for example, the operator Analyzes individual task runs.  Tasks may only contain one element, the top level 'StateMachine' element."); 
	curr->addProp("UIEnabled","");

	curr = addDescription("Result");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->setOverview("A result is the tunnel between one level of the state machine and the level above it.  It is transitioned to within its parent element, and transitioned from in its parent's parent.  In terms of execution order, once execution hits a result, the results 'EntryScript' is called, then the result's parent's container's 'ExitScript' is called followed by the transition to the next element in the result's parent's parent.");
	curr->addProp("UIEnabled","");
	curr->addProp("EntryScript","A script that can be used to transfer data between elements.  When execution transitions to this result, this script runs.  Note: The EntryScript of a result runs before the result's parent's ExitScript.");
	
	curr = addDescription("LogicResult");
	curr->setInherits(getAssetDescription("Result"));
	curr->setOverview("Logic Results differ from regular results in that they are not transitioned to by triggered by program logic.  In the case of a TargetController, for example, a 'Success' logic result would be triggered by the Picto system when the user fixated long enough on the set target.  In the case of a SwitchElement, the operator designs scripts to trigger particular logic results on certain conditions.");

	curr = addDescription("ControlTargetResult");
	curr->setInherits(getAssetDescription("Result"));
	curr->setOverview("Control Target Results differ from regular results in that they are not transitioned to by triggered by program logic.  They are used in a ChoiceController and triggered if the ControlTarget in their 'ControTarget' property is fixated on.  Unlike regular results, logic result do not have scripts attached to them.");
	curr->addProp("EntryScript","");
	curr->addProp("ControlTarget","The name of the control target which, if fixated on, will cause this result to be triggered.");

	curr = addDescription("PausePoint");
	curr->setInherits(getAssetDescription("OutputElementContainer"));
	curr->setOverview("Pause Points are used to define points in task execution where it will be logical for a pauses to occur.  When an operator presses the pause button during a session run, execution continues until it reaches the first pause point, where it waits for the operator to press the play button.  In some respects, pause points are like states that use a sort of 'play button controller.'  They are the only elements apart from States in which experimental time can pass and display any visual elements that are in scope or inside the pause element during the course of the pause.  Entry and exit scripts on a Pause Point are run regardless of whether the operator pressed pause.    State Machines, States and Pause Points contribute to the layered structure of a State Machine Design.  The rules of scope in each of these cases is that any element can access any other element in its own level or in a level above it.  This is true of scripts that are used to get and set values, and output stimuli that are considered enabled during a State or Pause Point when they are in scope.");
	curr->addProp("ForcePause","When this property is set true, execution will pause whenever it reaches this Pause Point regardless of whether the operator pressed pause.");
	
	curr = addDescription("Reward");
	curr->setInherits(getAssetDescription("StateMachineElement"));
	curr->setOverview("Reward elements are used to supply reward to the user during the course of execution.  Reward elements do not consume any experimental time.  When execution reaches these elements they schedule rewards to be provided according to their property settings, with the first reward being supplied at the time that the first phosphor of the next displayed frame appears.");
	curr->addProp("NumRewards","The number of rewards to initiate when execution reaches this reward element.");
	curr->addProp("RewardQty","The number of milliseconds for which each reward will be supplied.");
	curr->addProp("MinRewardPeriod","The minimum time that Picto will wait between supplying one reward from this element and the next.");
	curr->addSProp("number","Sets/Gets the current value of NumRewards.");
	curr->addSProp("unitQuantity","Sets/Gets the current value of RewardQty.");
	curr->addSProp("minRewardPeriod","Sets/Gets the current value of MinRewardPeriod.");

	curr = addDescription("State");
	curr->setInherits(getAssetDescription("MachineContainer"));
	curr->setOverview("Apart from Pause Points, States are the only elements in the state machine in which actual experimental time is consumed.  When a state is entered, its EntryScript is run, then a loop is entered in which all contained Control Elements are checked for their result conditions, if no result has occured the FrameScript is run, a frame is presented and Control Elements are checked again, etc.  The loop ends when one of the Control Elements triggers a result at which point execution transitions to one of the State's results, that result 'EntryScript' is run, the State's 'ExitScript' is run and execution continues in the level above.  During the course of state execution, all visual and other stimuli that are in scope and visible/enabled will be presented every frame.  Since the actual frame itself is the only time that the user experiences the operations of the program, Picto records all internal property and transition changes as occuring at the time that the frame following them occured.  It should be noted that the design inside a State looks slightly different from the regular State Machine design.  In a state there is no initial transition as execution occurs on one or many control elements 'simultaneously.'  Currently, there is no way of setting the order of checking multiple Control Elements in a state.  Each one is checked each frame.  Execution will continue from the result of the first one that triggers on a given frame.  State Machines, States and Pause Points contribute to the layered structure of a State Machine Design.  The rules of scope in each of these cases is that any element can access any other element in its own level or in a level above it.  This is true of scripts that are used to get and set values, and output stimuli that are considered enabled during a State or Pause Point when they are in scope.");
	curr->addProp("FrameScript","A script that can be used to transfer data between elements.  This script runs just before each frame is presented.  This means that when entering a state, the order of execution is EntryScript, FrameScript, PresentFrame.");
	curr->addProp("AnalysisFrameScript","When an analysis is active, this script is used to analyze experimental data.  Conceptually, this script runs just after the a frame is presented by the state, which occurs just after the 'FrameScript' is run.  In practice, it does not run on the PictoDirector but only on session 'viewers' and actually runs just after the frame is marked as having been presented.");

	curr = addDescription("StateMachine");
	curr->setInherits(getAssetDescription("MachineContainer"));
	curr->setOverview("State Machines are the containers for all execution logic in Picto.  They can contain any State Machine Elements, Variables, Parameters, Logic Elements or Stimulus Elements.  When execution hits a State Machine, the first thing that happens is that all of its contained elements reset their properties back to the initial states defined in the State Machine Editor.  Next the State Machine's Entry State is run, execution then follows from the initial state connected to the State Machine's Start Bar and continues internally until one of the State Machine's results are hit, or the operator presses stop.  After the result's scripts are run or the stop button is pressed, the State Machine's exit script is called and execution continues at the level above and transitions away from the result that was hit.  The two tunnels into and out of a State Machine level are there for the Start Bar and the Results.    State Machines, States and Pause Points contribute to the layered structure of a State Machine Design.  The rules of scope in each of these cases is that any element can access any other element in its own level or in a level above it.  This is true of scripts that are used to get and set values, and output stimuli that are considered enabled during a State or Pause Point when they are in scope.");

	curr = addDescription("SwitchElement");
	curr->setInherits(getAssetDescription("StateMachineElement"));
	curr->setOverview("Switch Elements are used to branch execution as a function of some logical condition.  They can be opened up and multiple Logic Results added to them.  The Switch Element script then chooses which Logic Result will be used to continue execution with its 'Script'.  The 'Script' chooses the result by returning a string that is equal to that result's name.  String results that don't match will result in runtime errors.  Please also note that in the interest of code clarity, the Switch Element script should only read the current Experimental state and not write to it.  Any actions in the Switch Element script that cause a write to occur in the Experimental state will trigger a runtime error.");
	curr->addProp("UIEnabled","");
	curr->addProp("Script","A script that is run to select a result.  This script should return a string that is equal to the name of one of the results.  Execution flow will then continue from that result.  This script should be considered 'read only' and operations that will result in a 'write' should go into Entry or Exit Scripts.  Write operations in this script will result in a runtime error.");
	
	//Control Logic State Machine Elements
	curr = addDescription("ChoiceController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr->setOverview("The Choice Controller, like the Target Controller is used to check if the user has fixated on a Control Target.  In the case of the Choice Controller however, multiple targets can be defined using internal Control Target Results and each one of these results creates a different possible result outcome from the Choice Controller.  Like in the case of a Target Controller, any Control Target that is being actively considered will be visible to the operator while this Choice Controller is active.  The possible results from a Choice Controller are hopefully self explanetory.  Please also note that the Choice Controller is essentially a convenience element.  Its operation can be fairly precisely simulated by simply using multipole Target Controllers.");
	curr->addProp("SignalChannel","The signal channel used to check for entry/exit into the targets defined in this controller's Control Target Results.  Currently only 'Position' (default) and 'Diameter' are acceptable, and 'Diameter' would me somewhat cumbersome considering that it is not mapped into any x,y screen position.");
	curr->addProp("TimeUnits","The time units (Sec,Ms,Us) of the values in 'FixationTime' and 'TotalTime'.");
	curr->addProp("FixationTime","The amount of time that the user must continuously fixate on a contained target before 'TotalTime' is reached in order for that target's Control Target Result to be triggered.");
	curr->addProp("TotalTime","The amount of time that the user has to succesfully fixate on one of the contained targets for a continuous 'FixationTime'.  Note: If the user is not fixating on a target at a time beyond 'TotalTime'-'FixationTime', 'Total Time Exceeded' will be triggered.");
	curr->addProp("AllowReentries","If true, the user's eye position may enter and exit the contained targets without triggering a 'Broke Fixation' result until they either fixate for 'FixationTime' or 'TotalTime' passes.");
	curr->addSProp("fixationTime","Sets/Gets the current value of the necessary 'FixationTime' considered succesful fixation.");
	curr->addSProp("totalTime","Sets/Gets the current value of the 'TotalTime' that the user has in order to correctly fixate before 'Total Time Exceeded' is triggered.");
	curr->addSFunc("bool userOnTarget()","Returns true if the user's eye position is on a target.  Warning: returns invalid data for sessions saved while Picto was in alpha stage (ie. original Ambiguity,Matching Pennies sessions)");
	curr->addSFunc("bool userEnteredTarget()","Returns true if the user's eye position entered a target between the last frame presented and the one before it.  Warning: returns invalid data for sessions saved while Picto was in alpha stage (ie. original Ambiguity,Matching Pennies sessions)");
	curr->addSFunc("bool userExitedTarget()","Returns true if the user's eye position exited a target between the last frame presented and the one before it.  Warning: returns invalid data for sessions saved while Picto was in alpha stage (ie. original Ambiguity,Matching Pennies sessions)");

	curr = addDescription("ScriptController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr->setOverview("A Script Controller is a broad use controller in which the designer can create their own logical checks for whether to continue in a state or exit from it.  Like in a SwitchElement, you can add multiple Logic Results to a Script Controller and trigger them by returning a string that is equal to one of the results's names.  In order to continue in the State without exiting, simply return any value that is not equal to a result name or nothing at all.");
	curr->addProp("UIEnabled","");
	curr->addProp("Script","A script that can be used to create operator designed control scenarios.  The script can either trigger a result by returning a string that is equal to the name of one of its contained results or wait for next frame by returning any other defined value.  This script runs during the standard Control Logic Element period, just after the 'EntryScript' and before each FrameScript execution.");

	curr = addDescription("StopwatchController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr->setOverview("A Stopwatch Controller is a simple timer controller that triggers its Success result as soon as its set timeout runs out. Please note that since time is checked only once per frame the time that the next frame from a state following this one will appear will be anywhere from one to two frames beyond the set 'Time'.  This is true of all time based conditions in Control Elements.");
	curr->addProp("TimeUnits","The time units (Sec,Ms,Us) of the value in 'Time'.");
	curr->addProp("Time","The time in 'TimeUnits' at which the 'Success' result will be triggered.  ");
	curr->addSProp("timeout","Sets/Gets the current value of the Time property which defines the time at which 'Success' will be triggered. ");

	curr = addDescription("TargetController");
	curr->setInherits(getAssetDescription("ControlElement"));
	curr->setOverview("The Target Controller is used to check if the user has fixated on a Control Target.  The target is defined in the 'ControlTarget' property.  While the Control Target is active, the target being considered will be visible to the operator.  The possible results from a Target Controller are hopefully self explanetory.");
	curr->addProp("SignalChannel","The signal channel used to check for entry/exit into the target defined in 'ControlTarget'.  Currently only 'Position' (default) and 'Diameter' are acceptable, and 'Diameter' would me somewhat cumbersome considering that it is not mapped into any x,y screen position.");
	curr->addProp("ControlTarget","The name of the target that the user must fixate on for 'FixationTime' in order for 'Success' to be triggered.");
	curr->addProp("TimeUnits","The time units (Sec,Ms,Us) of the values in 'FixationTime', 'TotalTime', 'MinInitialAcquisitionTime', 'MaxInitialAcquisitionTime' and 'MaxReacquisitionTime'.");
	curr->addProp("MinInitialAcquisitionTime","The time before which any fixation on the target is not counted toward the 'FixationTime'.");
	curr->addProp("MaxInitialAcquisitionTime","The time before which the user must begin their initial fixation.  If the user does not fixate on the target for at least a single frame before this time, 'Initial Aquistion Time Exceeded' will be triggered.");
	curr->addProp("MaxReacquisitionTime","When 'ReaquisitionAllowed' is true, this is the amount of time that the user has to look outside the target after having left it before 'Reacquisition Time Exceeded' is triggered.");
	curr->addProp("FixationTime","The amount of time that the user must continuously fixate on the target defined in 'ControlTarget' before 'TotalTime' is reached in order for 'Success' to be triggered.");
	curr->addProp("TotalTime","The amount of time that the user has to succesfully fixate on the target defined in 'ControlTarget' for a continuous 'FixationTime'.  Note: If the user is not fixating on the target at a time beyond 'TotalTime'-'FixationTime', 'Total Time Exceeded' will be triggered.");
	curr->addProp("ReacquisitionAllowed","If true, the user's eye position may enter and exit the target defined in 'ControlTarget' without triggering a 'Broke Fixation' result until they either fixate for 'FixationTime' or 'TotalTime' passes.");
	curr->addSProp("fixationTime","Sets/Gets the current value of the necessary 'FixationTime' considered succesful fixation.");
	curr->addSProp("totalTime","Sets/Gets the current value of the 'TotalTime' that the user has in order to correctly fixate before 'Total Time Exceeded' is triggered.");
	curr->addSProp("minAcquisitionTime","Sets/Gets the current value of 'MinInitialAcquisitionTime'.");
	curr->addSProp("maxAcquisitionTime","Sets/Gets the current value of 'MaxInitialAcquisitionTime'.");
	curr->addSProp("maxReacquisitionTime","Sets/Gets the current value of 'MaxReacquisitionTime'.");
	curr->addSProp("reacquisitionAllowed","Sets/Gets the current value of 'ReacquisitionAllowed'.");
	curr->addSFunc("bool userOnTarget()","Returns true if the user's eye position is on a target.");
	curr->addSFunc("bool userEnteredTarget()","Returns true if the user's eye position entered a target between the last frame presented and the one before it.");
	curr->addSFunc("bool userExitedTarget()","Returns true if the user's eye position exited a target between the last frame presented and the one before it.");

	//Variables
	curr = addDescription("NumberVariable");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("A Number Variable is used to hold experimentally significant numeric data.  For simple operations in a script, a local javascript var is sufficient, but this value does not maintain its state from frame to frame, and global javascript variables may not be used in Picto.  To store data outside of the local script scope, use a Picto Variable, parameter or other Picto element.");
	curr->addProp("Value","The value of the variable");
	curr->addSProp("value","Sets/Gets the current value of the variable.");

	curr = addDescription("StringVariable");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("A String Variable is used to hold experimentally significant string data.  For simple operations in a script, a local javascript var is sufficient, but this value does not maintain its state from frame to frame, and global javascript variables may not be used in Picto.  To store data outside of the local script scope, use a Picto Variable, parameter or other Picto element.");
	curr->addProp("Value","The value of the variable");
	curr->addSProp("value","Sets/Gets the current value of the variable.");

	curr = addDescription("VariableList");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("A Variable List is used to hold experimentally significant data in a list.  Data is entered in javascript var format and retreived as a number or string by using the appropriate functions.");
	curr->addSFunc("number length()","Returns the number of values in the array.");
	curr->addSFunc("append(value)","Appends the input value to the end of the array, increasing the array length by one.");
	curr->addSFunc("prepend(value)","Prepends the input value to the beginning of the array, increasing the array length by one.");
	curr->addSFunc("setValue(index,value)","Changes the current value of the array at index to value.  If index is less than zero or greater than length()-1, nothing happens.");
	curr->addSFunc("fromArray(array)","Resets the array from the input javascript array object.");
	curr->addSFunc("number getValueAsNum(index)","Returns the value at the input index as a number.  If an invalid index is supplied, an undefined value will be returned.");
	curr->addSFunc("number firstAsNum()","Returns the first value in the array as a number.  If the array is empty, and undefined value is returned.");
	curr->addSFunc("number lastAsNum()","Returns the last value in the array as a number.  If the array is empty, and undefined value is returned.");
	curr->addSFunc("number takeFirstAsNum()","Returns the first value in the array as a number and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("number takeLastAsNum()","Returns the last value in the array as a number and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("number takeAtAsNum(index)","Returns the value in the array at the input index as a number and deletes that value from the array, decreasing the array length by one.  If the index is invalid an undefined value is returned.");
	curr->addSFunc("string getValueAsString(index)","Returns the value at the input index as a string .  If an invalid index is supplied, an undefined value will be returned.");
	curr->addSFunc("string firstAsString()","Returns the first value in the array as a string .  If the array is empty, and undefined value is returned.");
	curr->addSFunc("string lastAsString()","Returns the last value in the array as a string .  If the array is empty, and undefined value is returned.");
	curr->addSFunc("string takeFirstAsString()","Returns the first value in the array as a string and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("string takeLastAsString()","Returns the last value in the array as a string and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("string takeAtAsString(index)","Returns the value in the array at the input index as a string and deletes that value from the array, decreasing the array length by one.  If the index is invalid an undefined value is returned.");
	curr->addSFunc("removeFirst()","Removes the first value from the array.  If the array is empty, nothing happens.");
	curr->addSFunc("removeLast()","Removes the last value from the array.  If the array is empty nothing happens.");
	curr->addSFunc("removeAt(index)","Removes the value at the input index from the array.  If the input index is invalid, nothing happens.");


	curr = addDescription("VariableMap");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("A Variable Map is used to hold experimentally significant data in a lookup table format.  Data is as a javascript var along with its lookup key.  Data is retrieved with that lookup key as either a number or string by using the appropriate functions.");
	curr->addSFunc("number length()","Returns the number of values in the map.");
	curr->addSFunc("setValue(key,value)","Sets/Changes the current value of the map at the input key to value.");
	curr->addSFunc("fromAssocArray(assocArray)","Resets the map from the input javascript Associative Array object.  Advanced Note: This cannot be used with toAssocArray as a way of storing Javascript objects as function components of the object are not usable after the transfer.");
	curr->addSFunc("number getValueAsNum(key)","Returns the value at the input key as a number.  If a key that is not in the map is supplied, an undefined value will be returned.");
	curr->addSFunc("string getValueAsString(key)","Returns the value at the input key as a string.  If a key that is not in the map is supplied, an undefined value will be returned.");
	curr->addSFunc("Array getKeys()","Returns a javascript array containing all of the keys currently in this map as strings.");
	curr->addSFunc("number takeAtAsNum(key)","Returns the value in the map at the input key as a number and deletes that value from the map.  If the key is invalid an undefined value is returned.");
	curr->addSFunc("string takeAtAsString(key)","Returns the value in the map at the input key as a string and deletes that value from the map.  If the key is invalid an undefined value is returned.");
	curr->addSFunc("removeAt(key)","Removes the value at the input key from the map.  If the input key is invalid, nothing happens.");

	//Parameters
	curr = addDescription("BooleanParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive a boolean parameter value from the operator during run time.");
	curr->addProp("Value","The boolean value.");
	curr->addSProp("value","Sets/Gets the current boolean value.");

	curr = addDescription("ColorParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive a color parameter value from the operator during run time.");
	curr->addProp("Value","The color value.");
	curr->addSProp("red","Sets/Gets the current red component of the color.");
	curr->addSProp("green","Sets/Gets the current green component of the color.");
	curr->addSProp("blue","Sets/Gets the current blue component of the color.");
	curr->addSProp("alpha","Sets/Gets the current alpha component of the color.");
	curr->addSProp("value","Sets/Gets the current color value as a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("setColor(r,g,b) or setColor(r,g,b,a)","A convenience function to set the rgb or rgba color components at once.  Inputs are (r,g,b) or (r,g,b,a) with each input ranging from 0-255.");

	curr = addDescription("DoubleParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive a floating point parameter value from the operator during run time.  The parameter value is confined to lie in a preset range.");
	curr->addProp("Value","The double value.");
	curr->addProp("Min","The minimum allowable double value. Operator will not be able to select a 'Value' below this.");
	curr->addProp("Max","The maximum allowable double value. Operator will not be able to select a 'Value' above this.");
	curr->addSProp("value","Sets/Gets the current double value.");

	curr = addDescription("FileParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to embed data from a file into an experimental design.");
	curr->addProp("FileName","The file whose contents are accessible to elements that connect to this parameter.  Note that this object embeds the contents of the file into the experiment at the time that the file is selected from the property area.  This means that changing the file on disk will not affect the experiment unless the file is reselected.");
	curr->addProp("UIEnabled","");

	curr = addDescription("NumericParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive an integer parameter value from the operator during run time.  The parameter value is confined to lie in a preset range.");
	curr->addProp("Value","The integer value.");
	curr->addSProp("value","Sets/Gets the current integer value.");

	curr = addDescription("OperatorClickParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive information about whether the operator clicked on their viewer screen and position of the click.  When the operator clicks on their viewer screen, requests for the newClick value will return true and requests for the x,y values will supply the operator click position in screen coordinates.  If newClick will be checked for an additional click before execution leaves its container, script code must set newClick back to false in order to prevent false positive detections by later scripting code.");
	curr->addSProp("x","Sets/Gets the x component of the last operator click position in screen units (0-800 from left to right).");
	curr->addSProp("y","Sets/Gets the y component of the last operator click position in screen units (0-600 from top to bottom).");
	curr->addSProp("newClick","This is set to true with the stored x,y positions updated whenever a new operator click occurs.  Note that once an operator click comes in, newClick will remain true until either its value is reset during a state transition or the design code sets it back to false.");

	curr = addDescription("PseudorandomIntParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retrieve a random sequence of numbers from a set range such that those numbers are never repeated until all numbers have been used.  For example, the range 1-4 could return 2,4,1,3 or 1,4,3,2 but never 3,2,2,1,4 unless the design specifically reshuffled the 2 back into the sequence.  New numbers are retrieved by calling 'randomize()' then checking the value script property.");
	curr->addProp("Value","The value stored in this object before any randomization occurs.");
	curr->addProp("UseSeed","Set this to true if you would like the order of randomized integers to be identical for every experimental run.");
	curr->addProp("DontRepeatAnyValue","When set to true, values returned from this randomizer will never be the same as those that immediately preceded them unless there are no other values in the list to choose from.");
	curr->addProp("Seed","When 'UseSeed' is true, this is the seed that is used in the randomizer.  For each seed a different reproducable random sequence will be produced.");
	curr->addProp("Min","The minimum value in the range of values that will be randomized.");
	curr->addProp("Max","The maximum value in the range of values that will be randomized.");
	curr->addSProp("value","Sets/Gets the current value stored in this element.");
	curr->addSProp("min","Sets/Gets the current minimum value in the range of values that are randomized.  Note that setting this value will cause randomization to be reinitialized to the list defined by the new 'Min' and 'Max'.");
	curr->addSProp("max","Sets/Gets the current maximum value in the range of values that are randomized.  Note that setting this value will cause randomization to be reinitialized to the list defined by the new 'Min' and 'Max'.");
	curr->addSFunc("randomize()","Generates the next pseudorandomly selected value and makes it accesible from the 'value' script property.");
	curr->addSFunc("reshuffleLastValue()","Moves the last generated value back into the list of values to be randomized so that it will be generated again before the current list runs out of values.");
	curr->addSFunc("reset()","Recreates the list of values to randomized according to the current'Min' and 'Max'.  After calling this, it is effectively as if randomize() has never been called.");

	curr = addDescription("RandomDoubleParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to generate a random double in a set range.  New random doubles are created by calling 'randomize()' and their values retrieved by using the 'value' script property.");
	curr->addProp("Value","The value stored in this object before any randomization occurs.");
	curr->addProp("UseSeed","Set this to true if you would like the order of randomized values to be identical for every experimental run.");
	curr->addProp("Seed","When 'UseSeed' is true, this is the seed that is used in the randomizer.  For each seed a different reproducable random sequence will be produced.");
	curr->addProp("Min","The minimum value in the range of values that will be randomized.");
	curr->addProp("Max","The maximum value in the range of values that will be randomized.");
	curr->addSProp("value","Sets/Gets the current value stored in this element.");
	curr->addSFunc("randomize()","Generates the next randomly selected value and makes it accesible from the 'value' script property.");

	curr = addDescription("RandomIntParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to generate a random integer in a set range.  New random integers are created by calling 'randomize()' and their values retrieved by using the 'value' script property.");
	curr->addProp("Value","The value stored in this object before any randomization occurs.");
	curr->addProp("UseSeed","Set this to true if you would like the order of randomized values to be identical for every experimental run.");
	curr->addProp("Seed","When 'UseSeed' is true, this is the seed that is used in the randomizer.  For each seed a different reproducable random sequence will be produced.");
	curr->addProp("Min","The minimum value in the range of values that will be randomized.");
	curr->addProp("Max","The maximum value in the range of values that will be randomized.");
	curr->addSProp("value","Sets/Gets the current value stored in this element.");
	curr->addSFunc("randomize()","Generates the next randomly selected value and makes it accesible from the 'value' script property.");

	curr = addDescription("RangeParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retreive an integer parameter value from the operator during run time.  The parameter value is confined to lie in a preset range and to be an integer number of set increment values from the bottom of this range.");
	curr->addProp("Value","The integer value.");
	curr->addProp("Min","The minimum allowable integer value. Operator will not be able to select a 'Value' below this.");
	curr->addProp("Max","The maximum allowable integer value. Operator will not be able to select a 'Value' above this.");
	curr->addProp("Increment","The acceptable increment at which integer values will be allowed.  For example, with Min=0,Max = 10 and Increment=2, 0,2,4,6,8,10 will be allowed and the operator will not be able to set any other values to this parameter.  If set to zero, this property is ignored and all integers in the Min->Max range are allowed.");
	curr->addSProp("value","Sets/Gets the current integer value.");

	curr = addDescription("SignalValueParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to retrieve that latest value of an experimental input signal (currently eye position or diameter).");
	curr->addProp("Signal","The signal channel who's values will be returned by this object.  Currently 'Position' (default) and 'Diameter' are acceptable.");
	curr->addSFunc("getValue(subChannel)","Returns the latest value from the input subChannel of the signal defined in 'Signal'.  Currently, 'x' and 'y' are acceptable.  For 'Position', values are in screen coordinates (x:0-800 from left to right, y:0-600 from top to bottom).  For 'Diameter' values are raw data.");

	curr = addDescription("TimerParameter");
	curr->setInherits(getAssetDescription("Parameter"));
	curr->setOverview("Used to track time.  The timer can be reset using restart() or set to restart from a given time by simply setting that time to the timer using the 'value' script property.");
	curr->addProp("TimeUnits","The time units (Sec,Ms,Us) of the time values returned from this element.");
	curr->addSProp("value","Gets the time of the first phosphor of the last frame that was displayed in units defined by 'TimeUnits'.  When used to set the value, this restarts the timer starting at the set time (Calls to value before the next frame will return the value that was set.  Calls after the next frame will return the value set plus a single frame period.)");
	curr->addSFunc("restart()","Restarts the timer from 0.  Equivalent to '(TimerParameter).value = 0;'");

	curr = addDescription("ImageFileParameter");
	curr->setInherits(getAssetDescription("FileParameter"));
	curr->setOverview("Used to embed image data from a file into an experimental design.  The image can be displayed by connecting an image graphic to this parameter.  Keep in mind that the selected file is embedded in the experiment such that modifications to a file after selecting it in the property window will not affect the experimental design unless the file is reselected.  Supported image types are: BMP, GIF, JPG, JPEG, PNG, PBM, PGM, PPM, SVG, XBM, XPM.");

	curr = addDescription("AudioFileParameter");
	curr->setInherits(getAssetDescription("FileParameter"));
	curr->setOverview("Used to embed audio data from a file into an experimental design.  The audio can be played by connecting an audio element to this parameter and calling its play() function.  Keep in mind that the selected file is embedded in the experiment such that modifications to a file after selecting it in the property window will not affect the experimental design unless the file is reselected.  Currently, only audio files in WAV format are supported.");

	//Logic Elements
	curr = addDescription("CircleTarget");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("Defines a circular target area for monitoring fixation in Control Elements.  When this target is in scope and used with a Target Controller or Choice Controller, it will be used to define the region of acceptable fixation and be set visible while it is being tracked.");
	curr->addProp("Radius","The radius of the circle target.");
	curr->addSProp("radius","Sets/Gets the circle target's current radius.");

	curr = addDescription("RectTarget");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("Defines a rectangular target area for monitoring fixation in Control Elements.  When this target is in scope and used with a Target Controller or Choice Controller, it will be used to define the region of acceptable fixation and be set visible while it is being tracked.");
	curr->addProp("Size","The size of the rectangle target.");
	curr->addSProp("width","Sets/Gets the target's current width.");
	curr->addSProp("height","Sets/Gets the target's current height.");

	curr = addDescription("ScriptFunction");
	curr->setInherits(getAssetDescription("ScriptableContainer"));
	curr->setOverview("This is a function that may be called by any script for which it is in scope.  Input variables can be defined and the function can return a value as well.  This is useful for situations when the same code would otherwise need to get copied multiple times in different scripts.  It can also serve to clean up scripts that would have otherwise been longer and less clear.");
	curr->addProp("UIEnabled","");
	curr->addProp("Inputs","A comma separated list of names for inputs that will be used by this script function.");
	curr->addProp("Script","A script that can be used to transfer data between elements.  This script may use any of the inputs from 'Inputs' and return values using the standard javascript return statement.");

	//Stimulus Elements
	curr = addDescription("BinaryDataOutput");
	curr->setInherits(getAssetDescription("OutputSignal"));
	curr->setOverview("This element is used to create an 8 bit binary output signal on data lines on a set port.  The data is set using the 'value' script property and making sure that the element is 'enabled'.");
	curr->addProp("Value","The integer value to be converted to binary and output on the 8 pins of the 'Port'.");
	curr->addSProp("value","Sets/Gets the current integer value to be output on the 8 pins of the 'Port'.");

	curr = addDescription("DigitalOutput");
	curr->setInherits(getAssetDescription("OutputSignal"));
	curr->setOverview("This element is used to generate a digital voltage value on a particular single line of a set port.  The data is set using the 'value' script property and making sure that the element is 'enabled'.");
	curr->addProp("Pin","The index of the pin (0-7) of the 'Port' that this element will use for voltage output.");
	curr->addProp("Value","The boolean value to be output on the 'Pin' of the 'Port'.");
	curr->addSProp("value","Sets/Gets the current boolean value to be output on the 'Pin' of the 'Port'.");
	curr->addSProp("pin","Sets/Gets the current index of the 'Pin' in the 'Port' to be used for output.");

	curr = addDescription("BoxGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to create a rectangular graphic on screen.  The center of the graphic will lie at the set 'Position'.");
	curr->addProp("Outline","If true, only the outline of this graphic will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the outline.");
	curr->addProp("Size","Sets the size of this graphic.");
	curr->addSProp("width","Sets/Gets the width of this graphic.");
	curr->addSProp("height","Sets/Gets the height of this graphic.");
	curr->addSProp("outline","Sets/Gets whether currently only the graphic's outline is being drawn.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of this graphic in a single function call.");

	curr = addDescription("CircleGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
curr->setOverview("This element is used to create a circular graphic on screen.  The center of the graphic will lie at the set 'Position'.");
	curr->addProp("Outline","If true, only the outline of this graphic will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the outline.");
	curr->addProp("Radius","The radius of the circle.");
	curr->addSProp("radius","Sets/Gets the circle's current radius.");
	curr->addSProp("outline","Sets/Gets whether currently only the circle outline is being drawn.");

	curr = addDescription("DiamondGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to create a diamond graphic on screen.  The center of the graphic will lie at the set 'Position'.");
	curr->addProp("Outline","If true, only the outline of this graphic will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the outline.");
	curr->addProp("Size","Sets the size of this graphic.");
	curr->addSProp("width","Sets/Gets the width of this graphic.");
	curr->addSProp("height","Sets/Gets the height of this graphic.");
	curr->addSProp("outline","Sets/Gets whether currently only the graphic's outline is being drawn.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of this graphic in a single function call.");

	curr = addDescription("EllipseGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to create an elliptical graphic on screen.  The center of the graphic will lie at the set 'Position'.");
	curr->addProp("Outline","If true, only the outline of this graphic will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the outline.");
	curr->addProp("Size","Sets the size of this graphic.");
	curr->addSProp("width","Sets/Gets the width of this graphic.");
	curr->addSProp("height","Sets/Gets the height of this graphic.");
	curr->addSProp("outline","Sets/Gets whether currently only the graphic's outline is being drawn.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of this graphic in a single function call.");

	curr = addDescription("GridGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to create an grid graphic on screen.  The center of the graphic will lie at the set 'Position' and the number of rows and columns can be set.");
	curr->addProp("Rows","The number of rows in this grid.");
	curr->addProp("Columns","The number of columns in this grid.");
	curr->addProp("Size","The size of the grid.");
	curr->addSProp("width","Sets/Gets the width of this grid.");
	curr->addSProp("height","Sets/Gets the height of this grid.");
	curr->addSProp("rows","Sets/Gets the number of rows in this grid.");
	curr->addSProp("columns","Sets/Gets the number of columns in this grid.");

	curr = addDescription("ImageGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to show an image from a file on screen.  The file is set by entering the name of an 'in scope' Image File parameter to the ImageFile property.  It can also be set programatically using the 'file' script property, in which case a string with the ImageFile parameter's name should be set to the script property. The image is scaled automatically by using the width and height properties.  The center of the image will lie at the set 'Position'.");
	curr->addProp("Color","");
	curr->addProp("ImageFile","The name of an Image File Parameter whose image file contents will be displayed when this graphic is visible.  The Image File Parameter must be in scope.");
	curr->addProp("Size","The size in pixels at which the image file should be displayed.  If the original image file is larger or smaller than this size it will be resized automatically.");
	curr->addSProp("alpha","");
	curr->addSProp("blue","");
	curr->addSProp("color","");
	curr->addSProp("file","Sets/Gets the name of an Image File Parameter whose image file contents will be displayed when this graphic is visible.  The name of the Image File Parameter should be in string form (ie. surrounded by quotes).  This allows an image graphic to have its displayed contents changed depending on the experimental state.");
	curr->addSProp("green","");
	curr->addSProp("height","Sets/Gets the height of this graphic.");
	curr->addSProp("red","");
	curr->addSProp("width","Sets/Gets the width of this graphic.");
	curr->addSFunc("setColor(r,g,b) or setColor(r,g,b,a)","");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of this graphic in a single function call.");

	curr = addDescription("OperatorInfoGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This element is used to output useful information for the experiment operator.  When the Analysis system is ready, it should more or less take the place of this graphic by allowing the operator to activate a particular analysis during a task run to overlay important information.  In any event, the graphic is used by setting field,value pairs using setData(field,value).  These pairs show up in the order that they were set on screen in the set location in a 'field1: value1, field2: value2' format.  Please note that on-screen values will not be updated even after calling setData(field,value) until updateValue() is called.");
	curr->addProp("Size","The size of the text area.");
	curr->addSFunc("setData(field,value)","Creates a new field,value pair, or updates the value in field if it already exists.  Note: The effects of this call are not implemented until 'updateValue()' is called.");
	curr->addSFunc("getDataAsInt(field)","Returns the data from the input field rounded down to an integer number.  This is equivalent to parseInt(getDataAsNum(field)), and ideally it could just go away along with getDataAsDouble(field), but old experiments are using them.");
	curr->addSFunc("getDataAsString(field)","Returns the data from the input field as a string.");
	curr->addSFunc("getDataAsDouble(field)","This is the same as getDataAsNum(field).  It is here for backwards compatibility.  Ideally, both this and getDataAsInt(field) could go away, but old experiments are still using them.");
	curr->addSFunc("getDataAsNum(field)","Returns the data from the input field as a floating point number.  This is equivalent to parseFloat(getDataAsString(field)).");
	curr->addSFunc("getNumEntries()","Returns the number of fields in this element.");
	curr->addSFunc("updateValue()","Causes the changes from calling 'setData(field,value)' to be implemented.  Only after this is called will the display on the screen change according to the contents of that function call.");
	curr->addSFunc("getFields()","Returns a javascript array of the field names currently used by this operator info graphic");
	curr->addSFunc("getValues()","Returns a javascript array of the values currently stored in this operator info graphic.  Values are in the order of the fields returned by getFields() and are in the string format in which they appear on screen.  To turn them into numbers, use parseFloat on the individual values.");

	curr = addDescription("RandomlyFilledGridGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This graphic is used to create a grid with two colors of boxes.  The position of the colored boxes is randomized but their proportion is precisely set.  The randomization can occur only once or at set frame intevals.");
	curr->addProp("Color2","The second color to be displayed in the graphic (Color is the first color).");
	curr->addProp("Size","The size of the graphic.");
	curr->addProp("HorizontalSquares","The number of horizontal squares to be displayed in the grid.");
	curr->addProp("VerticalSquares","The number of vertical squares to be displayed in the grid.");
	curr->addProp("Color2Squares","The number of sqaures with color 'Color2' to display in the grid.");
	curr->addProp("FramesPerUpdate","The number of frames that should pass before rerandomizing the square colors in the grid.  If set to zero, the grid square colors will be static.");

	curr = addDescription("ShapeShifterGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This graphic can be used when it is convenient for the same named graphic to be able to take on multiple shapes.  It is essentially a combination of the box, diamond, and elliptical shapes (where elliptical is a superset of the circle shape).");
	curr->addProp("Shape","The shape (Ellipse,Rectangle,Diamond) of this graphic.");
	curr->addProp("Outline","If true, only the outline of this graphic will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the outline.");
	curr->addProp("Size","Sets the size of this graphic.");
	curr->addSProp("width","Sets/Gets the width of this graphic.");
	curr->addSProp("height","Sets/Gets the height of this graphic.");
	curr->addSProp("shape","Sets/Gets the shape of this graphic.  Allowable values are 'Ellipse', 'Rectangle' and 'Diamond'.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of this graphic in a single function call.");

	curr = addDescription("TextGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("This graphic is used to display text on screen.  Since monkeys can't read, it is mostly useful for operator feedback and debugging during experimental design.  Don't forget to set the size of the text graphic or you won't see anything when you use this.");
	curr->addProp("Size","Sets the size of the text field in pixel units.");
	curr->addProp("Text","The text to be displayed.");
	curr->addSProp("width","Sets/Gets the width of the text field in pixel units.");
	curr->addSProp("height","Sets/Gets the height of the text field in pixel units.");
	curr->addSProp("text","Sets/Gets the displayed text.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of the text field in pixel units as a single function call.");

	curr = addDescription("TokenTrayGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("The Token Tray Graphic fills an often used role by presenting a tray of tokens on screen where the tokens have set shapes, they are arranged in a set shape, they each have set colors and sizes and appear in set positions within the tray.  This tray is quite robust and allows for a whole lot of different types of configurations.  In general, the design properties are used to set default values for the tokens and the various values of individual tokens can then be set and changed by scripts during the session run.  See the properties and functions for further details.");
	curr->addProp("Shape","The shape of the token tray (Ellipse only. Rectangle, Diamond are not yet implemented).  ie. The imaginary shape on which the tokens are placed.");
	curr->addProp("Size","Sets the size of the token tray.  ie. The size of the imaginary shape on which the tokens are placed.");
	curr->addProp("TokenShape","The shape (Ellipse,Rectangle,Diamond) of the tokens in the tray.");
	curr->addProp("TokenSize","The size of the tokens in the tray.");
	curr->addProp("NumTokens","The number of tokens in the tray.");
	curr->addProp("Phase","Phase in degrees at which the first token (Token0) sits in the tray.  The token will sit at the point where a line drawn outward from the tray center at the input phase intersects the tray shape.");
	curr->addProp("Outline","If true, only the outline of the tokens will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the token outlines.");
	curr->addSProp("tokenWidth","Sets/Gets default width of the tokens.");
	curr->addSProp("tokenHeight","Sets/Gets default height of the tokens.");
	curr->addSProp("tokenShape","Sets/Gets default shape of the tokens ('Ellipse','Rectangle','Diamond').");
	curr->addSProp("outline","Sets/Gets whether the tokens are being drawn with outline only by default.");
	curr->addSProp("outlineWIdth","Sets/Gets the default outline width in pixels of the tokens if outline is being used.");
	curr->addSProp("numTokens","Sets/Gets the number of tokens in the tray.");
	curr->addSProp("width","Sets/Gets the width of the token tray.");
	curr->addSProp("height","Sets/Gets the height of the token tray.");
	curr->addSProp("shape","Sets/Gets the shape of the token tray (Ellipse only. Rectangle, Diamond are not yet implemented).");
	curr->addSProp("phase","Sets/Gets the phase in degrees at which the first token (Token0) sits in the tray.  The token will sit at the point where a line drawn outward from the tray center at the set phase intersects the tray shape.");
	curr->addSFunc("setDimensions(w,h)","Sets the width and height of the tray in a single function call.");
	curr->addSFunc("setTokenDimensions(w,h)","Sets the default width and height of this tray's tokens in a single function call.");
	curr->addSFunc("setTokenSize(index,size)","Scales the size of the token at the input index by a factor of the input size, where size is a number between 0 and 1.");
	curr->addSFunc("setTokenColor(index,r,g,b,a) or setTokenColor(index,r,g,b)","Sets the color of the token at the input index according to the red, green, blue and optional alpha inputs.");
	curr->addSFunc("setTokenColor(index,color)","Sets the color of the token at the input index by using a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("setTokenShape(index,shape)","Sets the shape of the token at the input index according to the input shape ('Ellipse','Rectangle','Diamond').");
	curr->addSFunc("setTokenOutline(index,on)","Sets whether the token at the input index will be drawn as outline only (on=true).");
	curr->addSFunc("setTokenOutlineWidth(index,pixels)","Sets the width of the outline in pixels of the token at the input index if that token is being drawn outline only.");
	curr->addSFunc("getTokenX(index)","Gets the x component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSFunc("getTokenY(index)","Gets the y component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSFunc("getTokenLocalX(index)","Gets the x component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token tray.");
	curr->addSFunc("getTokenLocalY(index)","Gets the y component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token tray.");
	curr->addSFunc("getTokenSize(index)","Gets the scale factor from 0 to 1.0 of the token at the input index.");
	curr->addSFunc("getTokenRed(index)","Gets the red component of the color of the token at the input index.");
	curr->addSFunc("getTokenGreen(index)","Gets the green component of the color of the token at the input index.");
	curr->addSFunc("getTokenBlue(index)","Gets the blue component of the color of the token at the input index.");
	curr->addSFunc("getTokenAlpha(index)","Gets the alpha component of the color of the token at the input index.");
	curr->addSFunc("getTokenColor(index)","Gets the color value of the token at the input index as a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("getTokenOutline(index)","Returns whether the token at the input index is being drawn as outline only");
	curr->addSFunc("getTokenOutlineWidth(index)","Gets the width in pixels of the outline that is used on the token at the input index if it is set to be drawn outline only.");
	curr->addSFunc("getTokenShape(index)","Gets the shape of the token at the input index ('Ellipse','Rectangle','Diamond').");

	curr = addDescription("TokenFactoryGraphic");
	curr->setInherits(getAssetDescription("VisualElement"));
	curr->setOverview("The Token Factory Graphic is a set of individual 'token' graphics that have individual shapes, colors, sizes and positions that can be set during runtime by the designer.  This graphic is particularly useful if you have a widely variable number of graphics that will need to be used during experimentation.  It functions much like the Token Tray graphic with the main difference being that the token positions are setable.  This graphic is quite robust and allows for a whole lot of different types of configurations.  In general, the design properties are used to set default values for the tokens and the various values of individual tokens are then be set and changed by scripts during the session run.  See the properties and functions for further details.");
	curr->addProp("Shape","The shape of the token tray (Ellipse only. Rectangle, Diamond are not yet implemented).  ie. The imaginary shape on which the tokens are placed.");
	curr->addProp("Size","Sets the size of the token tray.  ie. The size of the imaginary shape on which the tokens are placed.");
	curr->addProp("TokenShape","The shape (Ellipse,Rectangle,Diamond) of the tokens in the tray.");
	curr->addProp("TokenSize","The size of the tokens in the tray.");
	curr->addProp("NumTokens","The number of tokens in the tray.");
	curr->addProp("Phase","Phase in degrees at which the first token (Token0) sits in the tray.  The token will sit at the point where a line drawn outward from the tray center at the input phase intersects the tray shape.");
	curr->addProp("Outline","If true, only the outline of the tokens will be drawn.");
	curr->addProp("OutlineThickness","If 'Outline' is true, this defines the thickness in pixels of the token outlines.");
	curr->addSProp("tokenWidth","Sets/Gets default width of the tokens.");
	curr->addSProp("tokenHeight","Sets/Gets default height of the tokens.");
	curr->addSProp("tokenShape","Sets/Gets default shape of the tokens ('Ellipse','Rectangle','Diamond').");
	curr->addSProp("outline","Sets/Gets whether the tokens are being drawn with outline only by default.");
	curr->addSProp("outlineWIdth","Sets/Gets the default outline width in pixels of the tokens if outline is being used.");
	curr->addSProp("numTokens","Sets/Gets the number of tokens in the tray.");
	curr->addSFunc("setTokenDimensions(w,h)","Sets the default width and height of this tray's tokens in a single function call.");
	curr->addSProp("setTokenPos(index,x,y)","Sets the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSProp("setTokenLocalPos(index,x,y)","Sets the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token factory.");
	curr->addSProp("setTokenX(index,x,y)","Sets the x component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSProp("setTokenY(index,x,y)","Sets the y component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSProp("setTokenLocalX(index,x,y)","Sets the x component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token factory.");
	curr->addSProp("setTokenLocalY(index,x,y)","Sets the y component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token factory.");
	curr->addSFunc("setTokenSize(index,size)","Scales the size of the token at the input index by a factor of the input size, where size is a number between 0 and 1.");
	curr->addSFunc("setTokenColor(index,r,g,b,a) or setTokenColor(index,r,g,b)","Sets the color of the token at the input index according to the red, green, blue and optional alpha inputs.");
	curr->addSFunc("setTokenColor(index,color)","Sets the color of the token at the input index by using a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("setTokenShape(index,shape)","Sets the shape of the token at the input index according to the input shape ('Ellipse','Rectangle','Diamond').");
	curr->addSFunc("setTokenOutline(index,on)","Sets whether the token at the input index will be drawn as outline only (on=true).");
	curr->addSFunc("setTokenOutlineWidth(index,pixels)","Sets the width of the outline in pixels of the token at the input index if that token is being drawn outline only.");
	curr->addSFunc("getTokenX(index)","Gets the x component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSFunc("getTokenY(index)","Gets the y component of the position of the token at the input index in global coordinates (ie. top left corner of screen is 0,0).");
	curr->addSFunc("getTokenLocalX(index)","Gets the x component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token factory.");
	curr->addSFunc("getTokenLocalY(index)","Gets the y component of the position of the token at the input index in local coordinates (ie. Token at (0,0) has the same position as the token factory.  Token at (10,10) has position 10 pixels to the right of and 10 pixels below the position set for the token factory.");
	curr->addSFunc("getTokenSize(index)","Gets the scale factor from 0 to 1.0 of the token at the input index.");
	curr->addSFunc("getTokenRed(index)","Gets the red component of the color of the token at the input index.");
	curr->addSFunc("getTokenGreen(index)","Gets the green component of the color of the token at the input index.");
	curr->addSFunc("getTokenBlue(index)","Gets the blue component of the color of the token at the input index.");
	curr->addSFunc("getTokenAlpha(index)","Gets the alpha component of the color of the token at the input index.");
	curr->addSFunc("getTokenColor(index)","Gets the color value of the token at the input index as a string containing hexadecimal r,g,b values (#rrggbb).");
	curr->addSFunc("getTokenOutline(index)","Returns whether the token at the input index is being drawn as outline only");
	curr->addSFunc("getTokenOutlineWidth(index)","Gets the width in pixels of the outline that is used on the token at the input index if it is set to be drawn outline only.");
	curr->addSFunc("getTokenShape(index)","Gets the shape of the token at the input index ('Ellipse','Rectangle','Diamond').");

	//Analysis
	curr = addDescription("AnalysisVariable");
	curr->setInherits(getAssetDescription("Parameter"));

	curr = addDescription("AnalysisFunction");
	curr->setInherits(getAssetDescription("ScriptFunction"));
	curr->setOverview("This is a function that may be called by any analysis script for which it is in scope and has access to any variables in its analysis or in the experimental design to which it is attached.  Input variables can be defined and the function can return a value as well.  This is useful for situations when the same code would otherwise need to get copied multiple times in different scripts.  It can also serve to clean up scripts that would have otherwise been longer and less clear.");
	curr->addProp("UIEnabled","");
	curr->addProp("Inputs","A comma separated list of names for inputs that will be used by this script function.");
	curr->addProp("Script","A script that can be used to transfer data between elements.  This script may use any of the inputs from 'Inputs' and return values using the standard javascript return statement.");

	curr = addDescription("AnalysisDataSource");
	curr->setInherits(getAssetDescription("AnalysisVariable"));

	curr = addDescription("AnalysisOutput");
	curr->setInherits(getAssetDescription("AnalysisVariable"));

	//Variables
	curr = addDescription("AnalysisNumberVariable");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("An Analysis Number Variable is used to hold numeric data used in an Analysis.  For simple operations in a script, a local javascript var is sufficient, but this value does not maintain its state from frame to frame, and global javascript variables may not be used in Picto.  To store data outside of the local script scope, use one of the Picto Analysis Variables.");
	curr->addProp("Value","The value of the variable");
	curr->addSProp("value","Sets/Gets the current value of the variable.");

	curr = addDescription("AnalysisStringVariable");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("An Analysis String Variable is used to hold experimentally significant string data.  For simple operations in a script, a local javascript var is sufficient, but this value does not maintain its state from frame to frame, and global javascript variables may not be used in Picto.  To store data outside of the local script scope, use one of the Picto Analysis Variables.");
	curr->addProp("Value","The value of the variable");
	curr->addSProp("value","Sets/Gets the current value of the variable.");

	curr = addDescription("AnalysisVariableList");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("An Analysis Variable List is used to hold analysis data in a list.  Data is entered in javascript var format and retreived as a number or string by using the appropriate functions.");
	curr->addSFunc("number length()","Returns the number of values in the array.");
	curr->addSFunc("append(value)","Appends the input value to the end of the array, increasing the array length by one.");
	curr->addSFunc("prepend(value)","Prepends the input value to the beginning of the array, increasing the array length by one.");
	curr->addSFunc("setValue(index,value)","Changes the current value of the array at index to value.  If index is less than zero or greater than length()-1, nothing happens.");
	curr->addSFunc("fromArray(array)","Resets the array from the input javascript array object.");
	curr->addSFunc("number getValueAsNum(index)","Returns the value at the input index as a number.  If an invalid index is supplied, an undefined value will be returned.");
	curr->addSFunc("number firstAsNum()","Returns the first value in the array as a number.  If the array is empty, and undefined value is returned.");
	curr->addSFunc("number lastAsNum()","Returns the last value in the array as a number.  If the array is empty, and undefined value is returned.");
	curr->addSFunc("number takeFirstAsNum()","Returns the first value in the array as a number and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("number takeLastAsNum()","Returns the last value in the array as a number and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("number takeAtAsNum(index)","Returns the value in the array at the input index as a number and deletes that value from the array, decreasing the array length by one.  If the index is invalid an undefined value is returned.");
	curr->addSFunc("string getValueAsString(index)","Returns the value at the input index as a string .  If an invalid index is supplied, an undefined value will be returned.");
	curr->addSFunc("string firstAsString()","Returns the first value in the array as a string .  If the array is empty, and undefined value is returned.");
	curr->addSFunc("string lastAsString()","Returns the last value in the array as a string .  If the array is empty, and undefined value is returned.");
	curr->addSFunc("string takeFirstAsString()","Returns the first value in the array as a string and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("string takeLastAsString()","Returns the last value in the array as a string and deletes that value from the array, decreasing the array length by one.  If the array is empty an undefined value is returned.");
	curr->addSFunc("string takeAtAsString(index)","Returns the value in the array at the input index as a string and deletes that value from the array, decreasing the array length by one.  If the index is invalid an undefined value is returned.");
	curr->addSFunc("removeFirst()","Removes the first value from the array.  If the array is empty, nothing happens.");
	curr->addSFunc("removeLast()","Removes the last value from the array.  If the array is empty nothing happens.");
	curr->addSFunc("removeAt(index)","Removes the value at the input index from the array.  If the input index is invalid, nothing happens.");

	curr = addDescription("AnalysisVariableMap");
	curr->setInherits(getAssetDescription("Variable"));
	curr->setOverview("An Analysis Variable Map is used to hold analysis data in a lookup table format.  Data is as a javascript var along with its lookup key.  Data is retrieved with that lookup key as either a number or string by using the appropriate functions.");
	curr->addSFunc("number length()","Returns the number of values in the map.");
	curr->addSFunc("setValue(key,value)","Sets/Changes the current value of the map at the input key to value.");
	curr->addSFunc("fromAssocArray(assocArray)","Resets the map from the input javascript Associative Array object.  Advanced Note: This cannot be used with toAssocArray as a way of storing Javascript objects as function components of the object are not usable after the transfer.");
	curr->addSFunc("number getValueAsNum(key)","Returns the value at the input key as a number.  If a key that is not in the map is supplied, an undefined value will be returned.");
	curr->addSFunc("string getValueAsString(key)","Returns the value at the input key as a string.  If a key that is not in the map is supplied, an undefined value will be returned.");
	curr->addSFunc("Array getKeys()","Returns a javascript array containing all of the keys currently in this map as strings.");
	curr->addSFunc("number takeAtAsNum(key)","Returns the value in the map at the input key as a number and deletes that value from the map.  If the key is invalid an undefined value is returned.");
	curr->addSFunc("string takeAtAsString(key)","Returns the value in the map at the input key as a string and deletes that value from the map.  If the key is invalid an undefined value is returned.");
	curr->addSFunc("removeAt(key)","Removes the value at the input key from the map.  If the input key is invalid, nothing happens.");

	//DataSources
	curr = addDescription("AnalysisFrameData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides frame timing data to be used in analysis.  The latest, previous, and next frame times can all be read out as single numerical values. Alternatively, frame times during a set time period can be read out as a javascript array of times.  All reported times are when the first phosphor appeared on the user display.  Time zero is the time of the frame preceding the point at which this element entered scope.  Alternatively, zeroLastFrame() can be called to set time zero to the time of the frame preceding that function call.  It should be noted that during tests in the workstation,  the system does not wait for first the phosphor before completing a frame.  For this reason it should not come as a surprise if frame times during testing are much higher than frame times during an experimental session.  High frame rates during testing; however, are a good indication that there will be no skipped frames during experimentation, assuming that the computer running the Picto director has a reasonably comparable processor.");
	curr->addSFunc("zeroLatestFrame()","Sets the first phophor time of the latest frame preceding this function call as time zero.  Note that all times reported from this element that precede the latest frame will be negative.  Also note that this function is called automatically as soon as this element comes into scope.");
	curr->addSFunc("getLatestTime()","Returns the time at which the first phosphor of the latest presented frame occured.");
	curr->addSFunc("getPrevTime()","Returns the time at which the first phosphor of one frame before the latest was presented.");
	curr->addSFunc("getNextTime()","Returns the time at which the first phosphor of the frame following this function call will occur.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getPrevTimes(number secsPreceding)","Returns a javascript array of first phosphor times that occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextTimes(number secsFollowing)","Returns a javascript array of first phosphor times that will occur starting from the next frame, up to and including the time at the input number of seconds following the latest frame time.  Note that the input quantity need not be an integer.  Also note that this function will only return data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getTimesSince(number beyondTime)","Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getTimesUntil(number upToTime)","Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");

	curr = addDescription("AnalysisLfpData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides local field potential data to be used in analysis.  The latest, and next lfp values/read times can be read out as numerical values. Latest, Next, Previous and Future ranges of lfp values and their read times can also be read out.  These can be selected according to the lfp channel number or returned in groups of n values in a javascript array where n is the number of lfp channels.  Time zero is the time of the frame preceding the point at which this element entered scope.  Alternatively, zeroLastFrame() can be called to set time zero to the time of the frame preceding that function call.  Active lfp channels can be read out using the getChannels() function.");
	curr->addSFunc("zeroLatestFrame()","Sets the first phophor time of the latest frame preceding this function call as time zero.  Note that all times reported from this element that precede the latest frame will be negative.  Also note that this function is called automatically as soon as this element comes into scope.");
	curr->addSFunc("getChannels()","Returns an array with a list of all active lfp channels in the current session.");
	curr->addSFunc("getSamplePeriod()","Returns the sample period at which lfp values were sampled in the neural data acquisition system used in this session.");
	curr->addSFunc("getLatestTime()","Returns the time at which the latest lfp values were read.");
	curr->addSFunc("getLatestValue(number channel)","Returns the latest value of the input lfp channel.");
	curr->addSFunc("getLatestValue()","Returns the latest values of all lfp channels as an array with size equal to the number of active channels and ordered like the result of getChannels().");
	curr->addSFunc("getNextTime()","Returns the time at which the lfp values will next be read.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextValue(number channel)","Returns the next value of the input lfp channel.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextValue()","Returns the next values of all lfp channels as an array with size equal to the number of active lfp channels and ordered like the result of getChannels().");
	curr->addSFunc("getPrevTimes(number secsPreceding)","Returns a list of times at which lfp values were read and that occured with times > the input # sec before the latest frame and &lt;= the latest frame time.  Indeces in the returned list correspond one to one with indices in the lists returned from the getPrevValues() functions.");
	curr->addSFunc("getNextTimes(number secsFollowing)","Returns a list of times at which lfp values will be read and with times > the latest frame time and &lt;= the input # sec after the latest frame.  Indeces in the returned list correspond one to one with indices in the list returned from the getNextValues() functions.");
	curr->addSFunc("getTimesSince(number beyondTime)","Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getTimesUntil(number upToTime)","Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getPrevValues(number channel,number secsPreceding)","Returns a list of lfp values for the input lfp channel that occured with times > the input # sec before the latest frame and &lt;= the latest frame time.  Indeces in the returned list correspond one to one with indices in the list returned from getPrevTimes().");
	curr->addSFunc("getNextValues(number channel,number secsFollowing)","Returns a list of lfp values for the input lfp channel that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  Indeces in the returned list correspond one to one with indices in the list returned from getNextTimes().");
	curr->addSFunc("getPrevValues(number secsPreceding)","Returns a list of lfp values for all lfp channels that will occur with times > the input # sec before the latest frame time and &lt;= the latest frame time. Returned value is a list of lists such that each index of the list contains a list of lfp channel values ordered like the result of getChannels().  Indeces in the returned list correspond one to one with indices in the list returned from getPrevTimes().");
	curr->addSFunc("getNextValues(number secsFollowing)","Returns a list of lfp values for all lfp channels that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame. Returned value is a list of lists such that each index of the list contains a list of lfp channel values ordered like the result of getChannels().  Indeces in the returned list correspond one to one with indices in the list returned from getNextTimes().");
	curr->addSFunc("getValuesSince(number channel,number beyondTime)","Functions like getPrevValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesUntil(number channel,number upToTime)","Functions like getNextValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesSince(number beyondTime)","Functions like getPrevValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesUntil(number upToTime)","Functions like getNextValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");


	curr = addDescription("AnalysisRewardData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides reward timing data to be used in analysis.  The latest, and next reward times/durations can be read out as single numerical values. Alternatively, reward times/durations during a set time period can be read out as a javascript array of times.  All reported times are when the reward solenoid opened.  Time zero is the time of the frame preceding the point at which this element entered scope.  Alternatively, zeroLastFrame() can be called to set time zero to the time of the frame preceding that function call.");
	curr->addSFunc("zeroLatestFrame()","Sets the first phophor time of the latest frame preceding this function call as time zero.  Note that all times reported from this element that precede the latest frame will be negative.  Also note that this function is called automatically as soon as this element comes into scope.");
	curr->addSFunc("getLatestTime()","Returns the time at which the latest reward was supplied.");
	curr->addSFunc("getNextTime()","Returns the time at which the next reward will be supplied.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getLatestDuration()","Returns the durtation of the latest reward was supplied in ms.");
	curr->addSFunc("getNextDuration()","Returns the duration of the next reward that will be supplied in ms.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getPrevTimes(number secsPreceding)","Returns a javascript array of reward supply times that occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextTimes(number secsFollowing)","Returns a javascript array of reward supply times that will occur starting from the next frame time, up to and including the time at the input number of seconds following the latest frame time.  Note that the input quantity need not be an integer.  Also note that this function will only return data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getTimesSince(number beyondTime)","Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getTimesUntil(number upToTime)","Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getPrevDurations(number secsPreceding)","Returns a javascript array of reward durations in ms that occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextDurations(number secsFollowing)","Returns a javascript array of reward durations in ms that will occur starting from the next frame time, up to and including the time at the input number of seconds following the latest frame time.  Note that the input quantity need not be an integer.  Also note that this function will only return data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getDurationsSince(number beyondTime)","Functions like getPrevDurations except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getDurationsUntil(number upToTime)","Functions like getNextDurations except that the input time is an absolute time with respect to this element's zero time instead of an offset.");

	curr = addDescription("AnalysisRunNotesData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides access to run notes and other run information.");
	curr->addSFunc("getRunName()","Returns the name of the current run.  This is the name entered by the operator during the session or the automatically generated name if no name was entered.");
	curr->addSFunc("getGlobalStartTime()","Returns the time in the session at which the run started.  This time is reported in seconds where time zero is the time at which the session began.");
	curr->addSFunc("getGlobalEndTime()","Returns the time in the session at which the run ended.  This time is reported in seconds where time zero is the time at which the session began.");
	curr->addSFunc("getRunNotes()","Returns the notes saved in the run info tab during the course of a session for the current run.");
	curr->addSFunc("getRunIndex()","Returns the index of this run in this sessions list of task runs, starting with zero.");

	curr = addDescription("AnalysisSignalData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides input signal data (eye position/diameter) to be used in analysis.  The latest, and next signal values/read times can be read out as numerical values. Latest, Next, Previous and Future ranges of signal values and their read times can also be read out.  These can be selected according to the name of an individual component of the signal or returned in groups of n values in a javascript array where n is the number of components of the signal.  Time zero is the time of the frame preceding the point at which this element entered scope.  Alternatively, zeroLastFrame() can be called to set time zero to the time of the frame preceding that function call.");
	curr->addProp("SignalName", "The name of the signal to be accessed using this element.  This value defaults to 'Position'");
	curr->addSFunc("zeroLatestFrame()","Sets the first phophor time of the latest frame preceding this function call as time zero.  Note that all times reported from this element that precede the latest frame will be negative.  Also note that this function is called automatically as soon as this element comes into scope.");
	curr->addSFunc("getComponentNames()","Returns an array with a list of all the components of the signal defined in the 'SignalName' property.");
	curr->addSFunc("getSamplePeriod()","Returns the sample period for this signal in samples per second.");
	curr->addSFunc("getLatestTime()","Returns the time at which the latest signal values were read.");
	curr->addSFunc("getLatestValue(string componentName)","Returns the latest value of the 'componentName' component of the signal.");
	curr->addSFunc("getLatestValue()","Returns the latest values of all components of the signal as an array with size equal to the number of the signal's components and ordered like the result of getComponentNames().");
	curr->addSFunc("getNextTime()","Returns the time at which the signal will next be read.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextValue(string componentName)","Returns the next value of the 'componentName' component of the signal.");
	curr->addSFunc("getNextValue()","Returns the next values of all components of the signal as an array with size equal to the number of the signal's components and ordered like the result of getComponentNames().");
	curr->addSFunc("getPrevTimes(number secsPreceding)","Returns a list of signal reading times that occured with times > the input # sec before the latest frame and &lt;= the latest frame time.  Indeces in the returned list correspond one to one with indices in the lists returned from the getPrevValues() functions.");
	curr->addSFunc("getNextTimes(number secsFollowing)","Returns a list of signal reading times that occured with times > the latest frame time and &lt;= the input # sec after the latest frame.  Indeces in the returned list correspond one to one with indices in the list returned from the getNextValues() functions.");
	curr->addSFunc("getTimesSince(number beyondTime)","Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getTimesUntil(number upToTime)","Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getPrevValues(string componentName,number secsPreceding)","Returns a list of signal values for the input componentName that occured with times > the input # sec before the latest frame and &lt;= the latest frame time.  Indeces in the returned list correspond one to one with indices in the list returned from getPrevTimes().");
	curr->addSFunc("getNextValues(string componentName,number secsFollowing)","Returns a list of signal values for the input sub channel that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  Indeces in the returned list correspond one to one with indices in the list returned from getNextTimes().");
	curr->addSFunc("getPrevValues(number secsPreceding)","Returns a list of signal values for all subcomponents that will occur with times > the input # sec before the latest frame time and &lt;= the latest frame time. Returned value is a list of lists such that each index of the list contains a list of signal subcomponents values ordered like the result of getComponentNames().  Indeces in the returned list correspond one to one with indices in the list returned from getPrevTimes().");
	curr->addSFunc("getNextValues(number secsFollowing)","Returns a list of signal values for all subcomponents that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame. Returned value is a list of lists such that each index of the list contains a list of signal subcomponent values ordered like the result of getComponentNames().  Indeces in the returned list correspond one to one with indices in the list returned from getNextTimes().");
	curr->addSFunc("getValuesSince(string componentName,number beyondTime)","Functions like getPrevValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesUntil(string componentName,number upToTime)","Functions like getNextValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesSince(number beyondTime)","Functions like getPrevValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getValuesUntil(number upToTime)","Functions like getNextValues except that the input time is an absolute time with respect to this element's zero time instead of an offset.");

	curr = addDescription("AnalysisSpikeData");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Provides spike data to be used in analysis.  The latest, and next spike times/channels/units/waveforms can be read out individually. Alternatively, spike times/channels/units/waveforms during a set time period can be read out as javascript arrays.  Time zero is the time of the frame preceding the point at which this element entered scope.  Alternatively, zeroLastFrame() can be called to set time zero to the time of the frame preceding that function call.");
	curr->addSFunc("zeroLatestFrame()","Sets the first phophor time of the latest frame preceding this function call as time zero.  Note that all times reported from this element that precede the latest frame will be negative.  Also note that this function is called automatically as soon as this element comes into scope.");
	curr->addSFunc("getSamplePeriod()","Returns the sample period in seconds used by the running sessions neural data acquisition system.");
	curr->addSFunc("getLatestTime()","Returns the time at which the latest spike occured.");
	curr->addSFunc("getLatestChannel()","Returns the channel of the latest spike.");
	curr->addSFunc("getLatestUnit()","Returns the unit of the latest spike.");
	curr->addSFunc("getLatestWaveform()","Returns an array of waveform values for the latest spike.");
	curr->addSFunc("getNextTime()","Returns the time at which the next spike will occur.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextChannel()","Returns the channel of the next spike to occur.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextUnit()","Returns the unit of the next spike to occur.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getNextWaveform()","Returns an array of waveform values for the next spike to occur.  Note that this only returns valid data when replaying an experiment.  Picto cannot predict the future.");
	curr->addSFunc("getPrevTimes(number secsPreceding)","Returns an array of spike times that occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  There is a one to one matchup between the arrays for all of the getPrevValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).   Note that the input quantity need not be an integer.");
	curr->addSFunc("getPrevChannels(number secsPreceding)","Returns an array of channels on which spikes occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  There is a one to one matchup between the arrays for all of the getPrevValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getPrevUnits(number secsPreceding)","Returns an array of the units (within a channel) on which spikes occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  There is a one to one matchup between the arrays for all of the getPrevValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getPrevWaveforms(number secsPreceding)","Returns an array of waveforms of spikes that occured after the input number of seconds preceding the latest frame time, up to and including the latest frame time.  Note that the returned quantity is an array of waveform data arrays, ie. To access the mth element of the nth waveform, use getPrevWaveforms(?)[n][m].  There is a one to one matchup between the arrays for all of the getPrevValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextTimes(number secsPreceding)","Returns an array of spike times that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  There is a one to one matchup between the arrays for all of the getNextValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).   Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextChannels(number secsPreceding)","Returns an array of channels of spikes that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  There is a one to one matchup between the arrays for all of the getNextValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextUnits(number secsPreceding)","Returns an array of units (within a channel) that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  There is a one to one matchup between the arrays for all of the getNextValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getNextWaveforms(number secsPreceding)","Returns an array of waveforms of spikes that will occur with times > the latest frame time and &lt;= the input # sec after the latest frame.  Note that the returned quantity is an array of waveform data arrays, ie. To access the mth element of the nth waveform, use getNextWaveforms(?)[n][m].  There is a one to one matchup between the arrays for all of the getNextValues() functions (ie. The same index in the array in different returned arrays provides data about the same spike).  Note that the input quantity need not be an integer.");
	curr->addSFunc("getTimesSince(number beyondTime)","Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getTimesUntil(number upToTime)","Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getChannelsSince(number beyondTime)","Functions like getPrevChannels except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getChannelsUntil(number upToTime)","Functions like getNextChannels except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getUnitsSince(number beyondTime)","Functions like getPrevUnits except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getUnitsUntil(number upToTime)","Functions like getNextUnits except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getWaveformsSince(number beyondTime)","Functions like getPrevWaveforms except that the input time is an absolute time with respect to this element's zero time instead of an offset.");
	curr->addSFunc("getWaveformsUntil(number upToTime)","Functions like getNextWaveforms except that the input time is an absolute time with respect to this element's zero time instead of an offset.");

	curr = addDescription("AnalysisTimer");
	curr->setInherits(getAssetDescription("AnalysisDataSource"));
	curr->setOverview("Used to track time.  The Analysis Timer can be reset using restart() or set to restart from a given time by simply setting that time to the timer using the 'value' script property.");
	curr->addProp("TimeUnits","The time units (Sec,Ms,Us) of the time values returned from this element.");
	curr->addSProp("value","Gets the time of the first phosphor of the last frame that was displayed in units defined by 'TimeUnits'.  When used to set the value, this restarts the timer starting at the set time (Calls to value before the next frame will return the value that was set.  Calls after the next frame will return the value set plus a single frame period.)");
	curr->addSFunc("restart()","Restarts the timer from 0.  Equivalent to '[AnalysisTimerName].value = 0;'");

	//Output
	curr = addDescription("AnalysisFileOutput");
	curr->setInherits(getAssetDescription("AnalysisOutput"));
	curr->setOverview("Used to write analysis output to a file.  The file can either be a text file or a binary file.  This object comes with a widget that appears in the Test Viewer and Replay Viewer to display the latest contents of the output file.  The output file will be saved to the location chosen by the operator after pressing save in the Test Viewer/Replay Viewer Analysis Output widget.");
	curr->addProp("FileSuffix","The suffix to be appended after the run name in creating the output filename ('.lfp.txt' would cause the output file to be named something like 'Session_2013_12_31__23_59_59.lfp.txt').");
	curr->addProp("FileType","The type of file to be created.  Text: A text file. BigEndian: A binary file with most significant bit on the first byte. LittleEndian: A binary file with most significant bit on the last byte.");
	curr->addSFunc("writeLine(text)","Writes the input text string to the end of the file appending a new line character afterwards.");
	curr->addSFunc("writeText(text)","Writes the input text string to the end of the file.");
	curr->addSFunc("writeBinary(csvData,csvTypes)","Writes data out to file as binary values (ie. short, int, double). csvData is a string with values separated by commas.  csvTypes is a string with value types separated by commas.  Valid types are short, int, long, float, double.  If more values appear in csvData than there are types in csvTypes, the last type will be used for all remaining values.  (ex. [AnalysisFileOutputName].writeBinary('1.23,726374,2,0','float,int,short,short');");




}
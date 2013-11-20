#include "TargetController.h"
#include "../engine/PictoEngine.h"
#include "../common/storage/ObsoleteAsset.h"

namespace Picto
{
TargetController::TargetController()
{
	setMaxOptionalResults(0);
	//AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty("SignalChannel","Position");
	//shapeList_ << "Rectangle" << "Oval";
	//AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	AddDefinableProperty("ControlTarget","");
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	AddDefinableProperty(QVariant::Int,"TotalTime",5);
	AddDefinableProperty(QVariant::Int,"MinInitialAcquisitionTime",0);
	AddDefinableProperty(QVariant::Int,"MaxInitialAcquisitionTime",1);
	AddDefinableProperty(QVariant::Int,"MaxReacquisitionTime",0);
	AddDefinableProperty(QVariant::Bool,"ReacquisitionAllowed",false);
	AddDefinableProperty(QVariant::Bool,"OnTarget",false);
	AddDefinableProperty(QVariant::Bool,"OnTargetChanged",false);

	//Target Entry and Exit Scripts are obsolete as of design syntax version "0.0.1".
	//This functionality should be handled in Frame Scripts by calling the 
	//newly exposed userOnTarget, userEnteredTarget, userExitedTarget functions.
	//AddDefinableProperty(QVariant::String,"TargetEntryScript","");
	//AddDefinableProperty(QVariant::String,"TargetExitScript","");
	AddDefinableObjectFactory("TargetEntryScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableObjectFactory("TargetExitScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	//Make sure to update the list of results...
	addRequiredResult("Success");
	addRequiredResult("Broke Fixation");
	addRequiredResult("Total Time Excceeded");
	addRequiredResult("Initial Aquistion Time Exceeded");
	addRequiredResult("Reaquistion Time Exceeded");
}

/*! \brief Constructs and returns the pointer to a new TargetController*/
ControlElement* TargetController::NewTargetController()
{
	return new TargetController;
}
/*! \brief Constructs and returns a shared pointer to a new TargetController*/
QSharedPointer<Asset> TargetController::Create()
{
	return QSharedPointer<Asset>(new TargetController());
}
/*! \brief Returns the name of this type of ControlElement: "Target Controller"*/
QString TargetController::ControllerType()
{
	return "Target Controller";
}


void TargetController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_->start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;
	waitingForReacquisition_ = false;
	initialAcquisitionOccurred_ = false;
	activateTargets();
	frameCtr_ = 0;

	//We call isDonePrivate to initialize everything
	isDonePrivate(engine);
}

void TargetController::stop(QSharedPointer<Engine::PictoEngine> engine)
{
	deactivateTargets();
}

void TargetController::upgradeVersion(QString deserializedVersion)
{
	ControlElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{	// In design syntax version "0.0.1", we added userOnTarget, userEnteredTarget, userExitedTarget functions.
		// In an effort to keep the allowed script locations uniform, we are making TargetEntry
		// and TargetExit scripts obsolete.
		// We upgrade these older experiments by automatically adding their entry and exit scripts
		// to the frame script and exit scripts in if(userEnteredTarget()) and if(userExitedTarget) blocks.

		//WARNING!!! userOnTarget(), userEnteredTarget() and userExitedTarget() 
		//functions cannot be used in AnalysisScripts of experiments that were saved with version below 0.0.1.  
		//This is because experiments with those versions used TargetEntry and TargetExit scripts and did not 
		//save OnTarget or OffTarget data in their session files.  Attempting to use these functions in Analysis 
		//Scripts would cause return of invalid data.
		QSharedPointer<Property> parentFrameScriptProp = getParentAsset().staticCast<DataStore>()->getGeneratedChildren("FrameScript").first().staticCast<Property>();
		QSharedPointer<Property> parentExitScriptProp = getParentAsset().staticCast<DataStore>()->getGeneratedChildren("ExitScript").first().staticCast<Property>();
		Q_ASSERT(parentFrameScriptProp);
		QString parentFrameScript = parentFrameScriptProp->value().toString();
		QString parentExitScript = parentExitScriptProp->value().toString();
		QSharedPointer<ObsoleteAsset> obsScript;
		if(getGeneratedChildren("TargetExitScript").size())
			obsScript = getGeneratedChildren("TargetExitScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put exit script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userExitedTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
			//Put exit script into parent's exit script.
			parentExitScript.prepend(QString("if(%1.userExitedTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		obsScript.clear();
		if(getGeneratedChildren("TargetEntryScript").size())
			obsScript = getGeneratedChildren("TargetEntryScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put entry script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userEnteredTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
			//Put entry script into parent's exit script.
			parentExitScript.prepend(QString("if(%1.userEnteredTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		parentFrameScriptProp->setValue(parentFrameScript);
		parentExitScriptProp->setValue(parentExitScript);
	}
}

void TargetController::activateTargets()
{
	if(controlTarget_)
		controlTarget_.toStrongRef()->setActive(true);
}

void TargetController::deactivateTargets()
{
	if(controlTarget_)
		controlTarget_.toStrongRef()->setActive(false);
}

bool TargetController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	//This property tells us if the user entered or exited the target this frame.
	//It may have been set during start(), so don't change it on the first frame
	//since no one has had a chance to read it yet.  Afterwards, set it back to false
	//every frame.
	if(frameCtr_ > 0)
		propertyContainer_->getProperty("OnTargetChanged")->setValue(false);
	frameCtr_++;
	return isDonePrivate(engine);
}

QSharedPointer<Result> TargetController::getResult()
{
	return ResultContainer::getResult(result_);
}
/*! \brief Returns true if the user is currently fixating on the attached target.  Returns false otherwise.  
 */
bool TargetController::userOnTarget()
{
	return propertyContainer_->getPropertyValue("OnTarget").toBool();
}
/*! \brief Returns true if the user started fixating on the attached target during the previous frame, false otherwise*/
bool TargetController::userEnteredTarget()
{
	return userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}
/*! \brief Returns true if the user stopped fixating on the attached target during the previous frame, false otherwise*/
bool TargetController::userExitedTarget()
{
	return !userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

/*! \brief Reconnects this object to its attached controlTarget_
 *	\details This function is called whenever something changed in the scripting system
 *	in this element's scope.  Since the change could be the name of the attached
 *	ControlTarget, whenever this function is called we reconnect to the ControlTarget
 *	based on its name.
 */
void TargetController::scriptableContainerWasReinitialized()
{
	ControlElement::scriptableContainerWasReinitialized();
	QList<QWeakPointer<Scriptable>> scriptables = getScriptableList();
	QString targetName = propertyContainer_->getPropertyValue("ControlTarget").toString();
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.isNull())
			continue;
		if(scriptable.toStrongRef()->getName() == targetName && scriptable.toStrongRef()->inherits("Picto::ControlTarget"))
		{
			controlTarget_ = scriptable.toStrongRef().staticCast<ControlTarget>();
			break;
		}
	}
}

bool TargetController::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(ControlElement::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Search my control target for the string
			QString controlTargetName = propertyContainer_->getPropertyValue("ControlTarget").toString();
			if(!controlTargetName.isNull() && controlTargetName.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	};
	return false;
}

/*! \brief Returns true if the TargetController has found a result, false if it is still looking
 *	\details This contains the meat of the logic that you would expect to find in the isDone()
 *	function.  In the case of this class, this logic needed to be used in multiple places so it
 *	was a good idea to move it into its own separate function.
 */
bool TargetController::isDonePrivate(QSharedPointer<Engine::PictoEngine> engine)
{
	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		timeUnits = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);


	//check to see if we've met or exceeded the total time
	int totalTime = propertyContainer_->getPropertyValue("TotalTime").toInt();
	int currTotalTime = cumulativeTimer_->elapsedTime(timeUnits);
	int currAcqTime = acquisitionTimer_->elapsedTime(timeUnits);
	int currReAcqTime = reacquisitionTimer_->elapsedTime(timeUnits);
	int fixTime = propertyContainer_->getPropertyValue("FixationTime").toInt();
	int remainingFixTime = targetAcquired_?fixTime-currAcqTime:fixTime;
	if(currTotalTime+remainingFixTime > totalTime)
	{
		isDone_ = true;
		result_ = "Total Time Excceeded";
		return true;
	}

	bool isInsideTarget = insideTarget(engine);

	//just entered target
	if(!targetAcquired_ && isInsideTarget)
	{
		//if we're past the Min Initial Acquisition Time mark the target as acquired
		//and start running the timer
		int minAcqTime = propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toInt();
		if(currTotalTime >= minAcqTime)
		{
			acquisitionTimer_->start();
			targetAcquired_ = true;
			initialAcquisitionOccurred_ = true;
			if(!engine->slaveMode())
			{
				propertyContainer_->getProperty("OnTarget")->setValue(true);
				propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
				//if(propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
				//	runScript(getName().simplified().remove(' ').append("_TargetEntry"));
			}
			//If fixation time is zero, we're done
			if(fixTime <= 0)
			{
				isDone_ = true;
				result_ = "Success";
				return true;
			}
		}

	}
	//just left target
	else if(targetAcquired_ && !isInsideTarget)
	{
		targetAcquired_ = false;
		if(!engine->slaveMode())
		{
			propertyContainer_->getProperty("OnTarget")->setValue(false);
			propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
			//if(propertyContainer_->getPropertyValue("TargetExitScript").toString() != "")
			//	runScript(getName().simplified().remove(' ').append("_TargetExit"));
		}

		//If reacquisition isn't allowed, then we're done with a failure value.
		if(!propertyContainer_->getPropertyValue("ReacquisitionAllowed").toBool())
		{
			isDone_ = true;
			result_ = "Broke Fixation";
			return true;
		}
		else
		{
			if(currTotalTime+fixTime > totalTime)
			{	//There's not enough time left for a complete fixation.
				isDone_ = true;
				result_ = "Total Time Excceeded";
				return true;
			}
			waitingForReacquisition_ = true;
			reacquisitionTimer_->start();
		}
	}
	//staying inside target
	else if(targetAcquired_ && isInsideTarget)
	{
		//check to see if we've met or exceeded the fixation time
		if(currAcqTime >= fixTime)
		{
			isDone_ = true;
			result_ = "Success";
			return true;
		}

	}
	//staying outside target
	else if(!targetAcquired_ && !isInsideTarget)
	{
		//check to see if we've met or exceeded the total time
		int maxAcqTime = propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toInt();

		//check to see if we've met or exceeded the max acquisition time (if relevant)
		if(!initialAcquisitionOccurred_)
		{
			if(currTotalTime >= maxAcqTime)
			{
				isDone_ = true;
				result_ = "Initial Aquistion Time Exceeded";
				return true;
			}
		}

		//check to see if we've met or exceeded the reacquisition time
		if(waitingForReacquisition_)
		{
			int reAcqTime = propertyContainer_->getPropertyValue("MaxReacquisitionTime").toInt();
			if(currReAcqTime >= reAcqTime)
			{
				isDone_ = true;
				result_ = "Reaquistion Time Exceeded";
				return true;
			}
		}

	}

	return false;
}

/*!	\brief Returns true if the signalChannel coordinates are within the attached ControlTarget bounds.
 */
bool TargetController::insideTarget(QSharedPointer<Engine::PictoEngine> engine)
{
	if(engine.isNull())
		return false;
	if(signal_.isNull())
	{
		//grab the signal channel
		signal_ = engine->getSignalChannel(propertyContainer_->getPropertyValue("SignalChannel").toString());
		if(signal_.isNull())
			return false;

		//confirm that the signal channel contains "x" and "y" subchannels
		Q_ASSERT(signal_->getSubchannels().contains("x"));
		Q_ASSERT(signal_->getSubchannels().contains("y"));
	}

	int x = signal_->peekValue("x");
	int y = signal_->peekValue("y");
	if(controlTarget_ && controlTarget_.toStrongRef()->contains(x,y))
		return true;
	return false;
}

void TargetController::postDeserialize()
{
	ControlElement::postDeserialize();

	cumulativeTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	acquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	reacquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();

	//Don't let user see OnTarget/OnTargetChanged, they are for internal use only
	propertyContainer_->getProperty("OnTarget")->setVisible(false);
	propertyContainer_->getProperty("OnTargetChanged")->setVisible(false);

	//Whenever the ControlTarget name changes, we need to reset the ControlTarget pointer.  To accomplish this, we connect its edited() signal to our controlTargetNameEdited() slot
	connect(propertyContainer_->getProperty("ControlTarget").data(),SIGNAL(edited()),this,SLOT(controlTargetNameEdited()));

	setPropertyRuntimeEditable("FixationTime");
	setPropertyRuntimeEditable("TotalTime");
	setPropertyRuntimeEditable("MinInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxReacquisitionTime");
	setPropertyRuntimeEditable("ReacquisitionAllowed");

}

bool TargetController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("ControlTarget").toString().isEmpty())
	{
		addError("Target Controller cannot have an empty 'ControlTarget' property.");
		return false;
	}
	return true;
}

/*! \brief Reattaches to the ControlTarget by calling scriptableContainerWasReinitialized()
 *	\details This function is called whenever the designer changes the value in the ControlTarget
 *	field.  When that value changes, it indicates that we need to update the attached ControlTarget
 *	which is why we must reattach.
 */
void TargetController::controlTargetNameEdited()
{
	scriptableContainerWasReinitialized();
}

} //namespace Picto
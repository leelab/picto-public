#include "FixationController.h"
#include "../engine/PictoEngine.h"

namespace Picto
{
	FixationController::FixationController()
{
	setMaxOptionalResults(0);
	AddDefinableProperty("SignalChannel","Position");
	AddDefinableProperty("ControlTarget","");
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	AddDefinableProperty(QVariant::Int,"MinInitialAcquisitionTime",0);
	AddDefinableProperty(QVariant::Int,"MaxInitialAcquisitionTime",1);
	AddDefinableProperty(QVariant::Int,"MaxReacquisitionTime",0);
	AddDefinableProperty(QVariant::Bool,"OnTarget",false);
	AddDefinableProperty(QVariant::Bool,"OnTargetChanged",false);

	//The list of results
	addRequiredResult("Success");
	addRequiredResult("Broke Fixation");
	addRequiredResult("Initial Acquisition Time Exceeded");
}

/*! \brief Constructs and returns the pointer to a new FixationController
 */
ControlElement* FixationController::NewFixationController()
{
	return new FixationController;
}
/*! \brief Constructs and returns a shared pointer to a new FixationController
 */
QSharedPointer<Asset> FixationController::Create()
{
	return QSharedPointer<Asset>(new FixationController());
}
/*! \brief Returns the name of this type of ControlElement: "Fixation Controller"
 */
QString FixationController::ControllerType()
{
	return "Fixation Controller";
}


void FixationController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	acquisitionTimer_->start();
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

void FixationController::stop(QSharedPointer<Engine::PictoEngine> engine)
{
	deactivateTargets();
}

void FixationController::activateTargets()
{
	if(controlTarget_)
		controlTarget_.toStrongRef()->setActive(true);
}

void FixationController::deactivateTargets()
{
	if(controlTarget_)
		controlTarget_.toStrongRef()->setActive(false);
}

bool FixationController::isDone(QSharedPointer<Engine::PictoEngine> engine)
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

QSharedPointer<Result> FixationController::getResult()
{
	return ResultContainer::getResult(result_);
}
/*! \brief Returns true if the user is currently fixating on the attached target.  Returns false otherwise.  
 */
bool FixationController::userOnTarget()
{
	return propertyContainer_->getPropertyValue("OnTarget").toBool();
}
/*! \brief Returns true if the user started fixating on the attached target during the previous frame, false otherwise
 */
bool FixationController::userEnteredTarget()
{
	return userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}
/*! \brief Returns true if the user stopped fixating on the attached target during the previous frame, false otherwise
 */
bool FixationController::userExitedTarget()
{
	return !userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

/*! \brief Reconnects this object to its attached controlTarget_
 *	\details This function is called whenever something changed in the scripting system in this element's scope.  Since the
 *	change could be the name of the attached ControlTarget, whenever this function is called we reconnect to the
 *	ControlTarget based on its name.
 */
void FixationController::scriptableContainerWasReinitialized()
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

bool FixationController::executeSearchAlgorithm(SearchRequest searchRequest)
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

/*! \brief Returns true if the FixationController has found a result, false if it is still looking
 *	\details This contains the meat of the logic that you would expect to find in the isDone() function.  In the case of
 *	this class, this logic needed to be used in multiple places so it was a good idea to move it into its own separate
 *	function.
 */
bool FixationController::isDonePrivate(QSharedPointer<Engine::PictoEngine> engine)
{
	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		timeUnits = Controller::TimerUnits::us;
	else
	{
		timeUnits = Controller::TimerUnits::ms;
		Q_ASSERT(false);
	}

	int fixTime = getFixationTime();

	int currAcqTime = acquisitionTimer_->elapsedTime(timeUnits);
	int currReAcqTime = reacquisitionTimer_->elapsedTime(timeUnits);

	bool isInsideTarget = insideTarget(engine);

	//just entered target
	if(!targetAcquired_ && isInsideTarget)
	{
		//if we're past the Min Initial Acquisition Time mark the target as acquired and start running the timer
		if (initialAcquisitionOccurred_ || currAcqTime >= getMinAcqTime())
		{
			if (!initialAcquisitionOccurred_)
			{
				acquisitionTimer_->start();
				currAcqTime = 0;
			}
			initialAcquisitionOccurred_ = true;

			targetAcquired_ = true;

			if(!engine->slaveMode())
			{
				propertyContainer_->getProperty("OnTarget")->setValue(true);
				propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
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
		}

		//If reacquisition isn't allowed, then we're done with a failure value.
		if (getMaxReacqTime() <= 0)
		{
			isDone_ = true;
			result_ = "Broke Fixation";
			return true;
		}
		else
		{
			waitingForReacquisition_ = true;
			reacquisitionTimer_->start();
		}
	}
	//staying inside target
	else if(targetAcquired_ && isInsideTarget)
	{
		//We have no particular tasks to perform when staying in the target.
	}
	//staying outside target
	else if(!targetAcquired_ && !isInsideTarget)
	{
		//check to see if we've met or exceeded the max acquisition time (if relevant)
		if(!initialAcquisitionOccurred_)
		{
			if (currAcqTime >= getMaxAcqTime())
			{
				isDone_ = true;
				result_ = "Initial Acquisition Time Exceeded";
				return true;
			}
		}

		//check to see if we've met or exceeded the reacquisition time
		if(waitingForReacquisition_)
		{
			if (currReAcqTime >= getMaxReacqTime())
			{
				isDone_ = true;
				result_ = "Broke Fixation";
				return true;
			}
		}

	}

	//If the user has initially fixated, and they've made it to the end of the fixation window without failure, then
	//they have, by definition, succeeded.
	if (initialAcquisitionOccurred_ && currAcqTime >= fixTime)
	{
		isDone_ = true;
		result_ = "Success";
		return true;
	}

	return false;
}

/*!	\brief Returns true if the signalChannel coordinates are within the attached ControlTarget bounds.
 */
bool FixationController::insideTarget(QSharedPointer<Engine::PictoEngine> engine)
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

void FixationController::postDeserialize()
{
	ControlElement::postDeserialize();

	acquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	reacquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();

	//Don't let user see OnTarget/OnTargetChanged, they are for internal use only
	propertyContainer_->getProperty("OnTarget")->setVisible(false);
	propertyContainer_->getProperty("OnTargetChanged")->setVisible(false);

	//Whenever the ControlTarget name changes, we need to reset the ControlTarget pointer.  To accomplish this, we connect
	//  its edited() signal to our controlTargetNameEdited() slot
	connect(propertyContainer_->getProperty("ControlTarget").data(),SIGNAL(edited()),this,SLOT(controlTargetNameEdited()));

	setPropertyRuntimeEditable("FixationTime");
	setPropertyRuntimeEditable("MinInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxReacquisitionTime");

}

bool FixationController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("ControlTarget").toString().isEmpty())
	{
		addError("Fixation Controller cannot have an empty 'ControlTarget' property.");
		return false;
	}
	return true;
}

/*! \brief Reattaches to the ControlTarget by calling scriptableContainerWasReinitialized()
 *	\details This function is called whenever the designer changes the value in the ControlTarget field.  When that value
 *	changes, it indicates that we need to update the attached ControlTarget which is why we must reattach.
 */
void FixationController::controlTargetNameEdited()
{
	scriptableContainerWasReinitialized();
}

} //namespace Picto
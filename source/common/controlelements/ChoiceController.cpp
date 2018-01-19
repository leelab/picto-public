#include "ChoiceController.h"
#include "../engine/PictoEngine.h"
#include "../storage/PropertyFactory.h"
#include "../common/storage/ObsoleteAsset.h"

using namespace Picto;

ChoiceController::ChoiceController()
{
	//Properties
	//propertyContainer_->setPropertyValue("Type",ControllerType());
	
	AddDefinableProperty(QVariant::String,"SignalChannel","Position");
	//AddDefinableProperty(QVariant::String,"Shape","");

	//shapeList_ << "Rectangle" << "Ellipse";
	//AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);

	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);

	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	timeUnits_ = 0;

	AddDefinableProperty(QVariant::Int,"TotalTime",5);

	AddDefinableProperty(QVariant::Bool,"AllowReentries",false);
	AddDefinableProperty(QVariant::Bool,"OnTarget",false);
	AddDefinableProperty(QVariant::Bool,"OnTargetChanged",false);

	//Target Entry and Exit Scripts are obsolete as of design syntax version "0.0.1".
	//This functionality should be handled in Frame Scripts by calling the 
	//newly exposed userOnTarget, userEnteredTarget, userExitedTarget functions.
	//AddDefinableProperty(QVariant::String,"TargetEntryScript","");
	//AddDefinableProperty(QVariant::String,"TargetExitScript","");
	AddDefinableObjectFactory("TargetEntryScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableObjectFactory("TargetExitScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	defineResultFactoryType("Target",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ControlTargetResult::Create))));

	//resultFactory_->addAssetType("Target",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ControlTargetResult::Create))));
	//setMaxUntypedResults(2);
	//Make sure to update the list of results...
	setMaxOptionalResults(0,"");
	addRequiredResult("Broke Fixation");
	addRequiredResult("Total Time Exceeded");
}

/*! \brief Constructs and returns the pointer to a new ChoiceController*/
ControlElement* ChoiceController::NewChoiceController()
{
	return new ChoiceController;
}
/*! \brief Constructs and returns a shared pointer to a new ChoiceController*/
QSharedPointer<Asset> ChoiceController::Create()
{
	return QSharedPointer<Asset>(new ChoiceController());
}
/*! \brief Returns the name of this type of ControlElement: "Choice Controller"*/
QString ChoiceController::ControllerType()
{
	return "Choice Controller";
}

void ChoiceController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_->start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;
	activateTargets();
	frameCtr_ = 0;

	//We call isDonePrivate to initialize everything
	isDonePrivate(engine);

}
void ChoiceController::stop(QSharedPointer<Engine::PictoEngine> engine)
{
	deactivateTargets();
}

void ChoiceController::activateTargets()
{
	foreach(QSharedPointer<ControlTargetResult> target, targets_)
		target->setActive(true);
}

void ChoiceController::deactivateTargets()
{
	foreach(QSharedPointer<ControlTargetResult> target, targets_)
		target->setActive(false);
}

void ChoiceController::upgradeVersion(QString deserializedVersion)
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

bool ChoiceController::isDone(QSharedPointer<Engine::PictoEngine> engine)
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
/*! \brief Returns true if the ChoiceController has found a result, false if it is still looking
 *	\details This contains the meat of the logic that you would expect to find in the isDone()
 *	function.  In the case of this class, this logic needed to be used in multiple places so it
 *	was a good idea to move it into its own separate function.
 */
bool ChoiceController::isDonePrivate(QSharedPointer<Engine::PictoEngine> engine)
{
	//This property tells us if the user entered or exited the target this frame.  Initialize it to false.
	//propertyContainer_->getProperty("OnTargetChanged")->setValue(false);

	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(timeUnits_,"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(timeUnits_,"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(timeUnits_,"") == "Us")
		timeUnits = Controller::TimerUnits::us;
	else
	{
		timeUnits = Controller::TimerUnits::ms;
		Q_ASSERT(false);
	}


	//check to see if we've met or exceeded the total time
	int totalTime = propertyContainer_->getPropertyValue("TotalTime").toInt();
	int currTotalTime = cumulativeTimer_->elapsedTime(timeUnits);
	int fixTime = propertyContainer_->getPropertyValue("FixationTime").toInt();
	int currAcqTime = acquisitionTimer_->elapsedTime(timeUnits);
	int remainingFixTime = targetAcquired_?fixTime-currAcqTime:fixTime;
	if(currTotalTime+remainingFixTime > totalTime)
	{
		isDone_ = true;
		result_ = "Total Time Exceeded";
		return true;
	}

	QString currTarget = insideTarget(engine);

	//just left a target
	if(currTarget != lastTarget_ && targetAcquired_)
	{
		if(!engine->slaveMode())
		{
			propertyContainer_->getProperty("OnTarget")->setValue(false);
			propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
			//if(propertyContainer_->getPropertyValue("TargetExitScript").toString() != "")
			//	runScript(getName().simplified().remove(' ').append("_TargetExit"));
		}
		//Are reentries allowed?
		if(!propertyContainer_->getPropertyValue("AllowReentries").toBool())
		{
			isDone_ = true;
			result_ = "Broke Fixation";
			return true;
		}
		if(currTarget == "NotATarget")
			targetAcquired_ = false;
		else
		{
			acquisitionTimer_->start();
			targetAcquired_ = true;
		}
		
		//Recheck if fixation is possible in the timeframe since a target was left
		if(currTotalTime+fixTime > totalTime)
		{
			isDone_ = true;
			result_ = "Total Time Exceeded";
			return true;
		}
	}
	//just entered a target
	else if(currTarget != "NotATarget" && !targetAcquired_)
	{
		if(!engine->slaveMode())
		{
			propertyContainer_->getProperty("OnTarget")->setValue(true);
			propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
			//if(propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
			//	runScript(getName().simplified().remove(' ').append("_TargetEntry"));
		}
		targetAcquired_ = true;
		acquisitionTimer_->start();
		//If fixation time is zero, we're done
		if(fixTime <= 0)
		{
			isDone_ = true;
			result_ = currTarget;
			return true;
		}
	}
	//staying inside a target
	else if(currTarget == lastTarget_ && targetAcquired_)
	{
		if(currAcqTime > fixTime)
		{
			isDone_ = true;
			result_ = currTarget;
			return true;
		}
	}
	//staying outside a target
	else if(currTarget == "NotATarget" && !targetAcquired_)
	{
		//do nothing
	}
	lastTarget_ = currTarget;

	return false;
}

/*! \brief Returns the name of the ControlTarget that the subject is currently inside.  
 *
 *	If the subject isn't inside a target, this returns NotATarget (which is a reserved name).
 */
QString ChoiceController::insideTarget(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_ASSERT(!engine.isNull());
	if(signal_.isNull())
	{
		//grab the signal channel
		signal_ = engine->getSignalChannel(propertyContainer_->getPropertyValue("SignalChannel").toString());
		Q_ASSERT_X(!signal_.isNull(),"ChoiceController::insideTarget","There is no signal channel called: " + propertyContainer_->getPropertyValue("SignalChannel").toString().toLatin1());

		//confirm that the signal channel contains "x" and "y" subchannels
		Q_ASSERT(signal_->getSubchannels().contains("x"));
		Q_ASSERT(signal_->getSubchannels().contains("y"));
	}

	//Run through the targets checking them
	foreach(QSharedPointer<ControlTargetResult> target, targets_)
	{
		//QRect targetRect = target->getBounds();
		if(target->contains(signal_->peekValue("x"),signal_->peekValue("y"))/*checkSingleTarget(targetRect)*/)
		{
			QString targetName = target->getName();
			return targetName;
		}
	}

	return "NotATarget";

}

QSharedPointer<Result> ChoiceController::getResult()
{
	return ResultContainer::getResult(result_);
}
/*! \brief Returns true if the subject is currently fixating on a target, false otherwise*/
bool ChoiceController::userOnTarget()
{
	return propertyContainer_->getPropertyValue("OnTarget").toBool();
}
/*! \brief Returns true if the user started fixating on a target during the previous frame, false otherwise*/
bool ChoiceController::userEnteredTarget()
{
	return userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}
/*! \brief Returns true if the user stopped fixating on a target during the previous frame, false otherwise*/
bool ChoiceController::userExitedTarget()
{
	return !userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

void ChoiceController::postDeserialize()
{
	ControlElement::postDeserialize();
	
	cumulativeTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	acquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	reacquisitionTimer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();

	//Don't let user see OnTarget/OnTargetChanged, they are for internal use only
	propertyContainer_->getProperty("OnTarget")->setVisible(false);
	propertyContainer_->getProperty("OnTargetChanged")->setVisible(false);

	//shapeIndex_ = propertyContainer_->getPropertyValue("Shape").toInt();
	timeUnits_ = propertyContainer_->getPropertyValue("TimeUnits").toInt();

	QString targetName;
	QList<QSharedPointer<Asset>> targetChildren = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(target->inherits("Picto::ControlTargetResult"))
		{
			targets_.push_back(target.staticCast<ControlTargetResult>());
			//addChildScriptableContainer(target.staticCast<ScriptableContainer>());
		}
	}
	setPropertyRuntimeEditable("FixationTime");
	setPropertyRuntimeEditable("TotalTime");
}

bool ChoiceController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	
	bool hasValidChild = false;
	QList<QSharedPointer<Asset>> targetChildren = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(target->inherits("Picto::ControlTargetResult"))
		{
			hasValidChild = true;
		}
	}
	if(!hasValidChild)
	{
		addError("At least one Control Result must be defined inside this ChoiceController.");
		return false;
	}
	if(timeUnits_<0)
	{
		addError("Unrecognized units for " + getName().toLatin1());
		return false;
	}

	QString targetName;
	QMap<QString,bool> targetNameMap;
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(!target->inherits("Picto::ControlTargetResult"))
		{
			if((target->getName() != "Broke Fixation")
				&& (target->getName() != "Total Time Exceeded"))
			{
				addError("Only Target Results can be used in a ChoiceController.");
				return false;
			}
		}
		targetName = target->getName();
		if(targetName == "NotATarget")
		{
			addError("Target name cannot be 'NotATarget'");
			return false;
		}
		//confirm that the new target name doesn't conflict
		if(targetNameMap.contains(targetName))
		{
			addError("No two Targets can have the same name.");
			return false;
		}
		targetNameMap[targetName] = true;
	}
	return true;
}
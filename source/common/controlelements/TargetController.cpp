#include "TargetController.h"
#include "../engine/PictoEngine.h"
#include "../common/storage/ObsoleteAsset.h"

namespace Picto
{
TargetController::TargetController()
{
	AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	
	AddDefinableProperty("SignalChannel","");
	//shapeList_ << "Rectangle" << "Oval";
	//AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	//AddDefinableProperty(QVariant::Rect,"Target",QRect());
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

	//Target Entry and Exit Scripts are obsolete as of design syntax version "0.0.3".
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


ControlElement* TargetController::NewTargetController()
{
	return new TargetController;
}

QSharedPointer<Asset> TargetController::Create()
{
	return QSharedPointer<Asset>(new TargetController());
}

QString TargetController::ControllerType()
{
	return "Target Controller";
}


void TargetController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_.start();
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
	if(deserializedVersion < "0.0.3")
	{	// In design syntax version "0.0.3", we added userOnTarget, userEnteredTarget, userExitedTarget functions.
		// In an effort to keep the allowed script locations uniform, we are making TargetEntry
		// and TargetExit scripts obsolete.
		// We upgrade these older experiments by automatically adding their entry and exit scripts
		// to the frame script in if(userEnteredTarget()) and if(userExitedTarget) blocks.

		//WARNING!!! userOnTarget(), userEnteredTarget() and userExitedTarget() 
		//functions cannot be used in AnalysisScripts of experiments that were saved with version below 0.0.3.  
		//This is because experiments with those versions used TargetEntry and TargetExit scripts and did not 
		//save OnTarget or OffTarget data in their session files.  Attempting to use these functions in Analysis 
		//Scripts would cause return of invalid data.
		QSharedPointer<Property> parentFrameScriptProp = getParentAsset().staticCast<DataStore>()->getGeneratedChildren("FrameScript").first().staticCast<Property>();
		Q_ASSERT(parentFrameScriptProp);
		QString parentFrameScript = parentFrameScriptProp->value().toString();
		QSharedPointer<ObsoleteAsset> obsScript;
		if(getGeneratedChildren("TargetExitScript").size())
			obsScript = getGeneratedChildren("TargetExitScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put exit script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userExitedTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		obsScript.clear();
		if(getGeneratedChildren("TargetEntryScript").size())
			obsScript = getGeneratedChildren("TargetEntryScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put entry script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userEnteredTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		parentFrameScriptProp->setValue(parentFrameScript);
	}
}

void TargetController::activateTargets()
{
	Q_ASSERT(!controlTarget_.isNull());
	controlTarget_.toStrongRef()->setActive(true);
}

void TargetController::deactivateTargets()
{
	Q_ASSERT(!controlTarget_.isNull());
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

QString TargetController::getResult()
{
	return result_;
}

bool TargetController::userOnTarget()
{
	return propertyContainer_->getPropertyValue("OnTarget").toBool();
}

bool TargetController::userEnteredTarget()
{
	return userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

bool TargetController::userExitedTarget()
{
	return !userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

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
	int currTotalTime = cumulativeTimer_.elapsedTime(timeUnits);
	int currAcqTime = acquisitionTimer_.elapsedTime(timeUnits);
	int currReAcqTime = reacquisitionTimer_.elapsedTime(timeUnits);
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
			acquisitionTimer_.start();
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
			reacquisitionTimer_.start();
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

/*!	\brief Returns true if the signalChannel coordinates are within the target area
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
	Q_ASSERT(!controlTarget_.isNull());
	if(controlTarget_.toStrongRef()->contains(x,y))
		return true;
	return false;
	//if("Rectangle" == shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt()))
	//{
	//	QRect targetRect = QRect(controlTarget_->getX(),controlTarget_->getY(),controlTarget_->getBoundingRect().width(),controlTarget_->getBoundingRect().height());//propertyContainer_->getPropertyValue("Target").toRect();
	//	if(targetRect.contains(x,y))
	//		return true;
	//	else
	//		return false;
	//}
	//else if("Ellipse" == shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt()))
	//{
	//	/*We're going to test this equation:
	//		(x-x0)^2     (y-0)^2
	//		--------  +  --------  <= 1
	//		 width^2     height^2
	//	*/
	//	QRect targetRect = QRect(controlTarget_->getX(),controlTarget_->getY(),controlTarget_->getBoundingRect().width(),controlTarget_->getBoundingRect().height());//propertyContainer_->getPropertyValue("Target").toRect();
	//	double x0 = targetRect.x() + targetRect.width()/2;
	//	double y0 = targetRect.y() + targetRect.height()/2;
	//	double width = targetRect.width();
	//	double height = targetRect.height();
	//	double term1 = (x-x0)*(x-x0)/(width*width);
	//	double term2 = (y-y0)*(y-y0)/(height*height);
	//	if(term1 + term2 <= 1.0)
	//		return true;
	//	else
	//		return false;
	//}
	//else
	//{
	//	return false;
	//}
}

void TargetController::postDeserialize()
{
	ControlElement::postDeserialize();
	//Don't let user see OnTarget/OnTargetChanged, they are for internal use only
	propertyContainer_->getProperty("OnTarget")->setVisible(false);
	propertyContainer_->getProperty("OnTargetChanged")->setVisible(false);

	setPropertyRuntimeEditable("FixationTime");
	setPropertyRuntimeEditable("TotalTime");
	setPropertyRuntimeEditable("MinInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxInitialAcquisitionTime");
	setPropertyRuntimeEditable("MaxReacquisitionTime");
	setPropertyRuntimeEditable("ReacquisitionAllowed");
}


/*! \brief Turns the ControlElement into an XML fragment
 *
 * A serialized TargetController will look something like this:
 *	<ControlElement type="TargetController" operatorVisible="true subjectVisible="false">
 *		<Name>MyControlElement</Name>
 *		<SignalChannel>ChannelName</SignalChannel>
 *		<Shape>Rectangle</Shape>
 *		<Target x="400" y="800" width="100" height="100">
 *		<TotalTime units="Ms">10000</TotalTime>
 *		<FixationTime units="Ms">1000</FixationTime>
 *		<MinInitialAcquisitionTime units="Ms">200</MinInitialAcquisitionTime>
 *		<MaxInitialAcquisitionTime units="Ms">2000</MaxInitialAcquisitionTime>
 *		<MaxReacquisitionTime units="Ms">500</MaxReacquisitionTime>
 *		<ReacquisitionAllowed>Yes</ReacquisitionAllowed>
 *	</ControlElement>
 */
////


bool TargetController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	//! \todo Add script verification once validate runs after full deserialization
	return true;
}

} //namespace Picto
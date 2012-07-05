#include "TargetController.h"
#include "../engine/PictoEngine.h"

namespace Picto
{
TargetController::TargetController()
{
	AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	
	AddDefinableProperty("SignalChannel","");
	//shapeList_ << "Rectangle" << "Oval";
	//AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	//AddDefinableProperty(QVariant::Rect,"Target",QRect());
	AddDefinableProperty("ControlTarget","");
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	AddDefinableProperty(QVariant::Int,"TotalTime",5);
	AddDefinableProperty(QVariant::Int,"MinInitialAcquisitionTime",0);
	AddDefinableProperty(QVariant::Int,"MaxInitialAcquisitionTime",1);
	AddDefinableProperty(QVariant::Int,"MaxReacquisitionTime",0);
	AddDefinableProperty(QVariant::Bool,"ReacquisitionAllowed",false);
	AddDefinableProperty(QVariant::String,"TargetEntryScript","");
	AddDefinableProperty(QVariant::String,"TargetExitScript","");

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
	Q_ASSERT(!controlTarget_.isNull());
	controlTarget_.toStrongRef()->setActive(true);

	//We call isDone to initialize everything
	isDone(engine);
}

void TargetController::stop(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_ASSERT(!controlTarget_.isNull());
	controlTarget_.toStrongRef()->setActive(false);
}

bool TargetController::isDone(QSharedPointer<Engine::PictoEngine> engine)
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
			if(!engine->slaveMode() && propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
				runScript(getName().simplified().remove(' ').append("_TargetEntry"));
		}

	}
	//just left target
	else if(targetAcquired_ && !isInsideTarget)
	{
		targetAcquired_ = false;
		if(!engine->slaveMode() && propertyContainer_->getPropertyValue("TargetExitScript").toString() != "")
			runScript(getName().simplified().remove(' ').append("_TargetExit"));

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

QString TargetController::getResult()
{
	return result_;
}


bool TargetController::hasScripts()
{
	return (propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
		|| (propertyContainer_->getPropertyValue("TargetExitScript").toString() != "");
}

QMap<QString,QPair<QString,QString>>  TargetController::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts;
	if(!hasScripts())
		return scripts;
	if(propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
		scripts[getName().simplified().remove(' ').append("_TargetEntry")] = QPair<QString,QString>(QString(),propertyContainer_->getPropertyValue("TargetEntryScript").toString());
	if(propertyContainer_->getPropertyValue("TargetExitScript").toString() != "")
		scripts[getName().simplified().remove(' ').append("_TargetExit")] = QPair<QString,QString>(QString(),propertyContainer_->getPropertyValue("TargetExitScript").toString());
	return scripts;
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
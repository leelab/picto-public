#include "StopwatchController.h"

namespace Picto
{
StopwatchController::StopwatchController()
{
	
	AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"Time",5);
	
	//propertyContainer_->setPropertyValue("Type",ControllerType());

	//QSharedPointer<Property> unitsEnumProperty = propertyContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"Units",0);
 //   unitList_ << "Sec" << "Ms" << "Us";
	//unitsEnumProperty->setAttribute("enumNames", unitList_);

	//propertyContainer_->addProperty(QVariant::Int,"Time",0);

	//Make sure to update the list of results...
	addRequiredResult("Success");
}


ControlElement* StopwatchController::NewStopwatchController()
{
	return new StopwatchController;
}

QString StopwatchController::ControllerType()
{
	return "Stopwatch Controller";
}

void StopwatchController::setTime(int time, Controller::TimerUnits::TimerUnits units)
{
	int newUnit;

	if(units == Controller::TimerUnits::ms)
		newUnit = unitList_.indexOf("Ms");
	else if(units == Controller::TimerUnits::us)
		newUnit = unitList_.indexOf("Us");
	else if(units == Controller::TimerUnits::sec)
		newUnit = unitList_.indexOf("Sec");
	else 
		return;

	propertyContainer_->setPropertyValue("Units",newUnit);
	propertyContainer_->setPropertyValue("Time",time);

}


void StopwatchController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);

	isDone_ = false;
	timer_.start();
}

bool StopwatchController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);

	Controller::TimerUnits::TimerUnits units;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		units = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		units = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		units = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);
	int time = timer_.elapsedTime(units);
	if(timer_.elapsedTime(units) >= propertyContainer_->getPropertyValue("Time").toInt())
	{
		isDone_ = true;
		return true;
	}
	else
	{
		return false;
	}
}

QString StopwatchController::getResult()
{
	if(isDone_)
		return "Success";
	else
		return "";
}

void StopwatchController::postSerialize()
{
	ControlElement::postSerialize();
}



///*! \brief Turns the ControlElement into an XML fragment
// *
// * A serialized StopwatchController will look something like this:
// *	<ControlElement type="StopwatchController" operatorVisible="true subjectVisible="false">
// *		<Name>MyControlElement</Name>
// *		<Time units="Ms">500</Time>
// *	</ControlElement>
// */
////

bool StopwatchController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto
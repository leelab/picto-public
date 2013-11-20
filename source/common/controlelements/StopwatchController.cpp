#include "StopwatchController.h"

namespace Picto
{
StopwatchController::StopwatchController()
{
	setMaxOptionalResults(0);
	//AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"Time",5);

	//Make sure to update the list of results...
	addRequiredResult("Success");
}

/*! \brief Creates and returns a pointer to a new StopwatchController*/
ControlElement* StopwatchController::NewStopwatchController()
{
	return new StopwatchController;
}

/*! \brief Creates and returns a shared pointer to a new StopwatchController*/
QSharedPointer<Asset> StopwatchController::Create()
{
	return QSharedPointer<Asset>(new StopwatchController());
}

/*! \brief Returns the name of this type of ControlElement.  Namely, "Stopwatch Controller".
*/
QString StopwatchController::ControllerType()
{
	return "Stopwatch Controller";
}

/*! \brief Sets values for the Time property along with the current time units
 *	\sa setTime(int)
*/
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
	timer_->start();
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
	if(timer_->elapsedTime(units) >= propertyContainer_->getPropertyValue("Time").toInt())
	{
		isDone_ = true;
		return true;
	}
	else
	{
		return false;
	}
}

QSharedPointer<Result> StopwatchController::getResult()
{
	if(isDone_)
		return ResultContainer::getResult("Success");
	else
		return QSharedPointer<Result>();
}

void StopwatchController::postDeserialize()
{
	ControlElement::postDeserialize();

	timer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();

	setPropertyRuntimeEditable("Time");
}

bool StopwatchController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto
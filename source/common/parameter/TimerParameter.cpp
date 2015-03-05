#include "TimerParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a new TimerParameter.
 *	\details Adds a TimeUnits Property to define whether values returned from this timer will
 *	 be in Seconds ("Sec"), Milliseconds ("Ms"), or Microseconds ("Us")
 */
TimerParameter::TimerParameter()
: 
  Parameter()
{
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	//AddDefinableProperty(QVariant::Int,"Value",-1);
}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* TimerParameter::NewParameter()
{
	return new TimerParameter;
}

/*! \brief Creates a new TimerParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> TimerParameter::Create()
{
	return QSharedPointer<Asset>(new TimerParameter());
}

/*! \brief Restarts the timer such that the time of the latest presented frame is time zero.
*/
void TimerParameter::restart()
{
	time_ = 0;
	timer_->restart();
}

/*! \brief Extends Parameter::enteredScope() to also restart this timer.
*/
void TimerParameter::enteredScope()
{
	Parameter::enteredScope();
	restart();
}

/*! \brief Returns the latest timer value in the units set in the "TimeUnits" parameter.
 *	\note Since the timer functions in frame resolution, the actual returned time will
 *	be the time of the latest presented frame minus the time of the latest frame when 
 *	restart() was last called or the TimerParameter entered scope.
 */
int TimerParameter::getValue()
{
	Controller::TimerUnits::TimerUnits units;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		units = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		units = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		units = Controller::TimerUnits::us;
	else
	{
		units = Controller::TimerUnits::ms;
		Q_ASSERT(false);
	}
	return time_ + timer_->elapsedTime(units);
	//qDebug(QString("Timer Value: %1").arg(propertyContainer_->getPropertyValue("Value").toInt()).toLatin1());
}

void TimerParameter::postDeserialize()
{
	Parameter::postDeserialize();
	timer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	restart();
}

bool TimerParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}
} //namespace Picto
#include "TimerParameter.h"

namespace Picto
{

TimerParameter::TimerParameter()
: 
  Parameter()
{
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	AddDefinableProperty(QVariant::Int,"Value",-1);
}

Parameter* TimerParameter::NewParameter()
{
	return new TimerParameter;
}

void TimerParameter::restart()
{
	timer_.restart();
}

void TimerParameter::updateTimeValue()
{
	Controller::TimerUnits::TimerUnits units;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		units = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		units = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		units = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);
	propertyContainer_->setPropertyValue("Value",timer_.elapsedTime(units));
	//qDebug(QString("Timer Value: %1").arg(propertyContainer_->getPropertyValue("Value").toInt()).toAscii());
}

void TimerParameter::postDeserialize()
{
	Parameter::postDeserialize();
}

bool TimerParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}
} //namespace Picto
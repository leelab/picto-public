#include "TimerParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

TimerParameter::TimerParameter()
: 
  Parameter()
{
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	restart();
	//AddDefinableProperty(QVariant::Int,"Value",-1);
}

Parameter* TimerParameter::NewParameter()
{
	return new TimerParameter;
}

QSharedPointer<Asset> TimerParameter::Create()
{
	return QSharedPointer<Asset>(new TimerParameter());
}

void TimerParameter::restart()
{
	time_ = 0;
	timer_.restart();
}

void TimerParameter::reset()
{
	Parameter::reset();
	restart();
}

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
		Q_ASSERT(false);
	return time_ + timer_.elapsedTime(units);
	//qDebug(QString("Timer Value: %1").arg(propertyContainer_->getPropertyValue("Value").toInt()).toLatin1());
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
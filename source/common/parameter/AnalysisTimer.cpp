#include "AnalysisTimer.h"
#include "../memleakdetect.h"

namespace Picto
{

AnalysisTimer::AnalysisTimer()
: AnalysisDataSource()
{
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	restart();
	//AddDefinableProperty(QVariant::Int,"Value",-1);
}

QSharedPointer<Asset> AnalysisTimer::Create()
{
	return QSharedPointer<Asset>(new AnalysisTimer());
}

void AnalysisTimer::restart()
{
	time_ = 0;
	timer_.restart();
}

void AnalysisTimer::reset()
{
	Parameter::reset();
	restart();
}

int AnalysisTimer::getValue()
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

void AnalysisTimer::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisTimer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
} //namespace Picto
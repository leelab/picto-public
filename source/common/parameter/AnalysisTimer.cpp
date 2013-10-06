#include "AnalysisTimer.h"
#include "../memleakdetect.h"

namespace Picto
{

AnalysisTimer::AnalysisTimer()
: AnalysisDataSource()
{
	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);
	//AddDefinableProperty(QVariant::Int,"Value",-1);
}

QSharedPointer<Asset> AnalysisTimer::Create()
{
	return QSharedPointer<Asset>(new AnalysisTimer());
}

void AnalysisTimer::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	AnalysisDataSource::setDesignConfig(designConfig);
	//Whenever the design config changes we need to change our change our frame timer so that 
	//we can be sure that it will be updated according to presented frames in the  
	//experiment to which this analysis is attached
	timer_ = getDesignConfig()->getFrameTimerFactory()->createTimer();
	restart();
}

void AnalysisTimer::restart()
{
	time_ = 0;
	Q_ASSERT(timer_);
	timer_->restart();
}

void AnalysisTimer::reset()
{
	AnalysisDataSource::reset();
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
	Q_ASSERT(timer_);
	int returnVal = time_ + timer_->elapsedTime(units);
	return returnVal;
	//qDebug(QString("Timer Value: %1").arg(propertyContainer_->getPropertyValue("Value").toInt()).toLatin1());
}

void AnalysisTimer::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
	restart();
}

bool AnalysisTimer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
} //namespace Picto
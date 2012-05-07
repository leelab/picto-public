#include <QSqlQuery>
#include "TimeTrigger.h"
using namespace Picto;

TimeTrigger::TimeTrigger()
{
	AddDefinableProperty(QVariant::Double,"TriggerPeriod",.01);
	restart();
}

TimeTrigger::~TimeTrigger()
{

}

QSharedPointer<Asset> TimeTrigger::Create()
{
	return QSharedPointer<Asset>(new TimeTrigger());
}

EventOrderIndex TimeTrigger::getNextTriggerTime()
{
	if(totalSessionTime_ < 0)
		recheckSessionData();
	currTime_.time_ += propertyContainer_->getPropertyValue("TriggerPeriod").toDouble();
	if(currTime_ > totalSessionTime_)
	{
		return EventOrderIndex();
	}
	return currTime_;
}

void TimeTrigger::restart()
{
	currTime_ = EventOrderIndex(0.0);
	totalSessionTime_ = -1;
}

float TimeTrigger::fractionTriggersRemaining()
{
	Q_ASSERT(totalSessionTime_ >= 0);
	return (totalSessionTime_-currTime_.time_)/propertyContainer_->getPropertyValue("TriggerPeriod").toDouble();
}

void TimeTrigger::recheckSessionData()
{
	totalSessionTime_ = 0;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);

	//Get dataid of last frame in session
	query.prepare("SELECT time FROM frames ORDER BY time DESC LIMIT 1");
	bool success = query.exec();
	if(success && query.next())
	{
		totalSessionTime_ = query.value(0).toDouble();
	}
}

void TimeTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool TimeTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("TriggerPeriod").toDouble() <= 0)
	{
		QString errMsg = QString("TimeTrigger: Trigger Period must be greater than zero.");
		addError("TimeTrigger", errMsg,xmlStreamReader);
		return false;
	}
	return true;
}
#include <QSqlQuery>
#include "TimeDataIterator.h"
#include "TimeTrigger.h"
using namespace Picto;

TimeTrigger::TimeTrigger()
{
	AddDefinableProperty(QVariant::Double,"TriggerPeriod",.01);
}

TimeTrigger::~TimeTrigger()
{

}

QSharedPointer<Asset> TimeTrigger::Create()
{
	return QSharedPointer<Asset>(new TimeTrigger());
}

QSharedPointer<AnalysisDataIterator> TimeTrigger::createDataIterator()
{
	return QSharedPointer<TimeDataIterator>(
							new TimeDataIterator(qsEngine_,session_,propertyContainer_->getPropertyValue("TriggerPeriod").toDouble())
							);
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
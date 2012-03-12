#include <QSqlQuery>
#include "TimeTrigger.h"
using namespace Picto;

TimeTrigger::TimeTrigger()
{
	AddDefinableProperty("PropertyPath","");
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
	if(!propIterator_)
	{
		propIterator_ = QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
		Q_ASSERT(propIterator_->isValid());
	}
	return propIterator_->getNextPropertyChange().index;
}

void TimeTrigger::restart()
{
	propIterator_.clear();
}

unsigned int TimeTrigger::totalKnownTriggers()
{
	return propIterator_->totalValues();
}

unsigned int TimeTrigger::remainingKnownTriggers()
{
	return propIterator_->remainingValues();
}

void TimeTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool TimeTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
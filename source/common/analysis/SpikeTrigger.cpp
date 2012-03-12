#include <QSqlQuery>
#include "SpikeTrigger.h"
using namespace Picto;

SpikeTrigger::SpikeTrigger()
{
	AddDefinableProperty("PropertyPath","");
}

SpikeTrigger::~SpikeTrigger()
{

}

QSharedPointer<Asset> SpikeTrigger::Create()
{
	return QSharedPointer<Asset>(new SpikeTrigger());
}

EventOrderIndex SpikeTrigger::getNextTriggerTime()
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

void SpikeTrigger::restart()
{
	propIterator_.clear();
}

unsigned int SpikeTrigger::totalKnownTriggers()
{
	return propIterator_->totalValues();
}

unsigned int SpikeTrigger::remainingKnownTriggers()
{
	return propIterator_->remainingValues();
}

void SpikeTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool SpikeTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
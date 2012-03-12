#include <QSqlQuery>
#include "LFPTrigger.h"
using namespace Picto;

LFPTrigger::LFPTrigger()
{
	AddDefinableProperty("PropertyPath","");
}

LFPTrigger::~LFPTrigger()
{

}

QSharedPointer<Asset> LFPTrigger::Create()
{
	return QSharedPointer<Asset>(new LFPTrigger());
}

EventOrderIndex LFPTrigger::getNextTriggerTime()
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

void LFPTrigger::restart()
{
	propIterator_.clear();
}

unsigned int LFPTrigger::totalKnownTriggers()
{
	return propIterator_->totalValues();
}

unsigned int LFPTrigger::remainingKnownTriggers()
{
	return propIterator_->remainingValues();
}

void LFPTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool LFPTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
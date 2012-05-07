#include <QSqlQuery>
#include "TransitionTrigger.h"
using namespace Picto;

TransitionTrigger::TransitionTrigger()
{
	AddDefinableProperty("PropertyPath","");
}

TransitionTrigger::~TransitionTrigger()
{

}

QSharedPointer<Asset> TransitionTrigger::Create()
{
	return QSharedPointer<Asset>(new TransitionTrigger());
}

EventOrderIndex TransitionTrigger::getNextTriggerTime()
{
	if(!propIterator_)
	{
		propIterator_ = QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
	}
	return propIterator_->getNextValue()->index;
}

void TransitionTrigger::restart()
{
	propIterator_.clear();
}

float TransitionTrigger::fractionTriggersRemaining()
{
	if(!propIterator_) return 1.0;
	return propIterator_->fractionRemaining();
}

void TransitionTrigger::recheckSessionData()
{
	if(!propIterator_)
		return;
	propIterator_->recheckSessionData();
}

void TransitionTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool TransitionTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
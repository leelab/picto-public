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
							new PropertyDataIterator(session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
	}
	return propIterator_->getNextPropertyChange()->index;
}

void TransitionTrigger::restart()
{
	propIterator_.clear();
}

unsigned int TransitionTrigger::totalKnownTriggers()
{
	return propIterator_->totalValues();
}

unsigned int TransitionTrigger::remainingKnownTriggers()
{
	return propIterator_->remainingValues();
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
#include <QSqlQuery>
#include "PropertyTrigger.h"
using namespace Picto;

PropertyTrigger::PropertyTrigger()
{
	AddDefinableProperty("PropertyPath","");
}

PropertyTrigger::~PropertyTrigger()
{

}

QSharedPointer<Asset> PropertyTrigger::Create()
{
	return QSharedPointer<Asset>(new PropertyTrigger());
}

EventOrderIndex PropertyTrigger::getNextTriggerTime()
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

void PropertyTrigger::restart()
{
	propIterator_.clear();
}

unsigned int PropertyTrigger::totalKnownTriggers()
{
	return propIterator_->totalValues();
}

unsigned int PropertyTrigger::remainingKnownTriggers()
{
	return propIterator_->remainingValues();
}

void PropertyTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool PropertyTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
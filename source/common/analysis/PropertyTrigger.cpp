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
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
	}
	return propIterator_->getNextValue()->index;
}

void PropertyTrigger::restart()
{
	propIterator_.clear();
}

float PropertyTrigger::fractionTriggersRemaining()
{
	if(!propIterator_) return 1.0;
	return propIterator_->fractionRemaining();
}

void PropertyTrigger::recheckSessionData()
{
	if(!propIterator_)
		return;
	propIterator_->recheckSessionData();
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
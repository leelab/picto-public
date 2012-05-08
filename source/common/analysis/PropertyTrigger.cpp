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

QSharedPointer<AnalysisDataIterator> PropertyTrigger::createDataIterator()
{
	return QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
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
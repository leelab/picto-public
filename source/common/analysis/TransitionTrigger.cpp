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

QSharedPointer<AnalysisDataIterator> TransitionTrigger::createDataIterator()
{
	return QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
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
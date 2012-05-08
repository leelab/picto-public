#include <QSqlQuery>
#include "SignalTrigger.h"
using namespace Picto;

SignalTrigger::SignalTrigger()
{
	AddDefinableProperty("SignalName","");
}

SignalTrigger::~SignalTrigger()
{

}

QSharedPointer<Asset> SignalTrigger::Create()
{
	return QSharedPointer<Asset>(new SignalTrigger());
}

QSharedPointer<AnalysisDataIterator> SignalTrigger::createDataIterator()
{
	return QSharedPointer<SignalDataIterator>(
							new SignalDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("SignalName").toString())
							);
}

void SignalTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool SignalTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
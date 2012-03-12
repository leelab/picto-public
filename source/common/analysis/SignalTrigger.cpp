#include <QSqlQuery>
#include "SignalTrigger.h"
using namespace Picto;

SignalTrigger::SignalTrigger()
{
	AddDefinableProperty("SignalName","");
	AddDefinableProperty("SubChannelName","");
}

SignalTrigger::~SignalTrigger()
{

}

QSharedPointer<Asset> SignalTrigger::Create()
{
	return QSharedPointer<Asset>(new SignalTrigger());
}

EventOrderIndex SignalTrigger::getNextTriggerTime()
{
	if(!signalIterator_)
	{
		signalIterator_ = QSharedPointer<SignalDataIterator>(
							new SignalDataIterator(session_,
								propertyContainer_->getPropertyValue("SignalName").toString(),
								propertyContainer_->getPropertyValue("SubChannelName").toString())
							);
		Q_ASSERT(signalIterator_->isValid());
	}
	return signalIterator_->getNextSignalVals().index;
}

void SignalTrigger::restart()
{
	signalIterator_.clear();
}

unsigned int SignalTrigger::totalKnownTriggers()
{
	return signalIterator_->totalValues();
}

unsigned int SignalTrigger::remainingKnownTriggers()
{
	return signalIterator_->remainingValues();
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
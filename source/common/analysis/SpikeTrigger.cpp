#include <QSqlQuery>
#include "SpikeTrigger.h"
using namespace Picto;

SpikeTrigger::SpikeTrigger()
{
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
	if(!spikeIterator_)
	{
		spikeIterator_ = QSharedPointer<SpikeDataIterator>(
							new SpikeDataIterator(qsEngine_,session_)
							);
	}
	latestData_ = spikeIterator_->getNextValue();
	return latestData_->index;
}

void SpikeTrigger::restart()
{
	spikeIterator_.clear();
}

float SpikeTrigger::fractionTriggersRemaining()
{
	if(!spikeIterator_) return 1.0;
	return spikeIterator_->fractionRemaining();
}


QSharedPointer<AnalysisValue> SpikeTrigger::getLatestValue()
{
	return latestData_;
}

void SpikeTrigger::recheckSessionData()
{
	if(!spikeIterator_)
		return;
	spikeIterator_->recheckSessionData();
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
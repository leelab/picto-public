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
							new SpikeDataIterator(session_)
							);
	}
	latestData_ = spikeIterator_->getNextSpikeVals();
	return latestData_->index;
}

void SpikeTrigger::restart()
{
	spikeIterator_.clear();
}

unsigned int SpikeTrigger::totalKnownTriggers()
{
	return spikeIterator_->totalValues();
}

unsigned int SpikeTrigger::remainingKnownTriggers()
{
	return spikeIterator_->remainingValues();
}


QSharedPointer<SpikeData> SpikeTrigger::getLatestValue()
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
#include <QSqlQuery>
#include "LFPTrigger.h"
using namespace Picto;

LFPTrigger::LFPTrigger()
{
}

LFPTrigger::~LFPTrigger()
{

}

QSharedPointer<Asset> LFPTrigger::Create()
{
	return QSharedPointer<Asset>(new LFPTrigger());
}

EventOrderIndex LFPTrigger::getNextTriggerTime()
{
	if(!lfpIterator_)
	{
		lfpIterator_ = QSharedPointer<LFPDataIterator>(
							new LFPDataIterator(qsEngine_,session_)
							);
	}
	latestValue_ = lfpIterator_->getNextLFPVals();
	return latestValue_->index;
}

void LFPTrigger::restart()
{
	lfpIterator_.clear();
}

unsigned int LFPTrigger::totalKnownTriggers()
{
	return lfpIterator_->totalValues();
}

unsigned int LFPTrigger::remainingKnownTriggers()
{
	return lfpIterator_->remainingValues();
}

QSharedPointer<LFPData> LFPTrigger::getLatestValue()
{
	return latestValue_;
}

void LFPTrigger::recheckSessionData()
{
	if(!lfpIterator_)
		return;
	lfpIterator_->recheckSessionData();
}

void LFPTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool LFPTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
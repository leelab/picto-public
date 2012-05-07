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
	latestValue_ = lfpIterator_->getNextValue();
	return latestValue_->index;
}

void LFPTrigger::restart()
{
	lfpIterator_.clear();
}

float LFPTrigger::fractionTriggersRemaining()
{
	if(!lfpIterator_) return 1.0;
	return lfpIterator_->fractionRemaining();
}

QSharedPointer<AnalysisValue> LFPTrigger::getLatestValue()
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
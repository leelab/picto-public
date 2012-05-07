#include <QSqlQuery>
#include "FrameTrigger.h"
using namespace Picto;

FrameTrigger::FrameTrigger()
{
}

FrameTrigger::~FrameTrigger()
{

}

QSharedPointer<Asset> FrameTrigger::Create()
{
	return QSharedPointer<Asset>(new FrameTrigger());
}

EventOrderIndex FrameTrigger::getNextTriggerTime()
{
	if(!frameIterator_)
	{
		frameIterator_ = QSharedPointer<FrameDataIterator>(
							new FrameDataIterator(qsEngine_,session_)
							);
	}
	return frameIterator_->getNextValue()->index;
}

void FrameTrigger::restart()
{
	frameIterator_.clear();
}

float FrameTrigger::fractionTriggersRemaining()
{
	if(!frameIterator_) return 1.0;
	return frameIterator_->fractionRemaining();
}

void FrameTrigger::recheckSessionData()
{
	if(!frameIterator_)
		return;
	frameIterator_->recheckSessionData();
}

void FrameTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool FrameTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
#include "FrameDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

FrameDataSource::FrameDataSource()
{
}

FrameDataSource::~FrameDataSource()
{
}

QSharedPointer<Asset> FrameDataSource::Create()
{
	return QSharedPointer<Asset>(new FrameDataSource());
}

void FrameDataSource::restart()
{
	frameIterator_.clear();
}

QSharedPointer<AnalysisValue> FrameDataSource::getValue(const EventOrderIndex& index)
{
	QString name = getName();
	if(!frameIterator_)
	{
		frameIterator_ = QSharedPointer<FrameDataIterator>(
							new FrameDataIterator(qsEngine_,session_)
							);
	}
	QSharedPointer<AnalysisValue> prev = lastDataUnit_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	//Get new frame values until the newest one is beyond the input
	//time or non-existant, then return the prior frame value.
	QSharedPointer<AnalysisValue> curr = frameIterator_->getNextValue();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = frameIterator_->getNextValue();
	}
	if(!curr || !curr->index.isValid())
	{
		curr = prev;
	}
	//Store the last read frame change (which should be beyond the input time or empty)
	lastDataUnit_ = curr;
	//Store the latest value of our frame
	latestValue_ = prev;
	return latestValue_;
}

void FrameDataSource::recheckSessionData()
{
	if(!frameIterator_)
		return;
	frameIterator_->recheckSessionData();
}

void FrameDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool FrameDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
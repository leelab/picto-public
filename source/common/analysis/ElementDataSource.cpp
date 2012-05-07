#include "ElementDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ElementDataSource::ElementDataSource()
{
}

ElementDataSource::~ElementDataSource()
{
}

QSharedPointer<Asset> ElementDataSource::Create()
{
	return QSharedPointer<Asset>(new ElementDataSource());
}

void ElementDataSource::restart()
{
	elemIterator_.clear();
}

QSharedPointer<AnalysisValue> ElementDataSource::getValue(const EventOrderIndex& index)
{
	if(!elemIterator_)
	{
		elemIterator_ = QSharedPointer<ElementDataIterator>(
							new ElementDataIterator(qsEngine_,session_)
							);
	}
	QSharedPointer<AnalysisValue> prev = lastDataUnit_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	//Get new element values until the newest one is beyond the input
	//time or non-existant, then return the prior element value.
	QSharedPointer<AnalysisValue> curr = elemIterator_->getNextValue();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = elemIterator_->getNextValue();
	}
	if(!curr || !curr->index.isValid())
	{
		curr = prev;
	}
	//Store the last read element change (which should be beyond the input time or empty)
	lastDataUnit_ = curr;
	//Store the latest value of our property
	latestValue_ = prev;
	return latestValue_;
}

void ElementDataSource::recheckSessionData()
{
	if(!elemIterator_)
		return;
	elemIterator_->recheckSessionData();
}

void ElementDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool ElementDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
#include "SignalDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

SignalDataSource::SignalDataSource()
{
	AddDefinableProperty("SignalName","");
	AddDefinableProperty("SubChannelName","");
	latestValue_ = "";
}

SignalDataSource::~SignalDataSource()
{
}

QSharedPointer<Asset> SignalDataSource::Create()
{
	return QSharedPointer<Asset>(new SignalDataSource());
}

void SignalDataSource::restart()
{
	signalIterator_.clear();
}

QString SignalDataSource::getValue(const EventOrderIndex& index)
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
	SignalData prev = lastDataUnit_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev.index > index)
		return latestValue_;
	
	//Get new property values until the newest one is beyond the input
	//time or non-existant, then return the prior property value.
	SignalData curr = signalIterator_->getNextSignalVals();
	while((curr.index <= index) && (curr.index.time_ >= 0))
	{
		prev = curr;
		curr = signalIterator_->getNextSignalVals();
	}
	//Store the last read property change (which should be beyond the input time or empty)
	lastDataUnit_ = curr;
	//Store the latest value of our property
	latestValue_ = prev.value;
	return latestValue_;
}

void SignalDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool SignalDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
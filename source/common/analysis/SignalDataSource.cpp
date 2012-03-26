#include "SignalDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

SignalDataSource::SignalDataSource()
{
	AddDefinableProperty("SignalName","");
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

QSharedPointer<AnalysisValue> SignalDataSource::getValue(const EventOrderIndex& index)
{
	if(!signalIterator_)
	{
		signalIterator_ = QSharedPointer<SignalDataIterator>(
							new SignalDataIterator(session_,
								propertyContainer_->getPropertyValue("SignalName").toString())
							);
	}
	QSharedPointer<SignalData> prev = nextValue_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	
	//Get new property values until the newest one is beyond the input
	//time or non-existant, then return the prior property value.
	QSharedPointer<SignalData> curr = signalIterator_->getNextSignalVals();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = signalIterator_->getNextSignalVals();
	}
	if(!curr || !curr->index.isValid())
	{
		curr = prev;
	}
	//Store the last read property change (which should be beyond the input time or empty)
	nextValue_ = curr;
	//Store the latest value of our property
	latestValue_ = prev;
	return latestValue_;
}

unsigned int SignalDataSource::numSubChannels()
{
	if(!signalIterator_)
		return 0;
	return signalIterator_->numSubChannels();
}

QString SignalDataSource::subChannelName(int subChanIndex)
{
	if(!signalIterator_)
		return "";
	return signalIterator_->subChanName(subChanIndex);
}

float SignalDataSource::value(int subChanIndex, int triggerIndex)
{
	QSharedPointer<SignalData> data = getScriptValue(triggerIndex).staticCast<SignalData>();
	if(data && (subChanIndex < data->values.size()))
		return data->values[subChanIndex];
	return 0.0;
}

void SignalDataSource::recheckSessionData()
{
	if(!signalIterator_)
		return;
	signalIterator_->recheckSessionData();
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
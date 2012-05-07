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
							new SignalDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("SignalName").toString())
							);
		int numSubChans = signalIterator_->numSubChannels();
		setScriptInfo("numSubChannels",numSubChans);
		QScriptValue subChanNames = qsEngine_->newArray(numSubChans);
		for(int i=0;i<numSubChans;i++)
			subChanNames.setProperty(i,signalIterator_->subChanName(i));
		setScriptInfo("subChannelName",subChanNames);
	}
	QSharedPointer<AnalysisValue> prev = nextValue_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	
	//Get new property values until the newest one is beyond the input
	//time or non-existant, then return the prior property value.
	QSharedPointer<AnalysisValue> curr = signalIterator_->getNextValue();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = signalIterator_->getNextValue();
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
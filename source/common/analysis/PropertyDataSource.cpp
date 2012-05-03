#include "PropertyDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

PropertyDataSource::PropertyDataSource()
{
	AddDefinableProperty("PropertyPath","");
}

PropertyDataSource::~PropertyDataSource()
{
}

QSharedPointer<Asset> PropertyDataSource::Create()
{
	return QSharedPointer<Asset>(new PropertyDataSource());
}

void PropertyDataSource::restart()
{
	propIterator_.clear();
}

QSharedPointer<AnalysisValue> PropertyDataSource::getValue(const EventOrderIndex& index)
{
	QString name = getName();
	if(!propIterator_)
	{
		propIterator_ = QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
	}
	QSharedPointer<PropData> prev = lastDataUnit_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	//Get new property values until the newest one is beyond the input
	//time or non-existant, then return the prior property value.
	QSharedPointer<PropData> curr = propIterator_->getNextPropertyChange();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = propIterator_->getNextPropertyChange();
	}
	if(!curr || !curr->index.isValid())
	{
		curr = prev;
	}
	//Store the last read property change (which should be beyond the input time or empty)
	lastDataUnit_ = curr;
	//Store the latest value of our property
	latestValue_ = prev;
	return latestValue_;
}

void PropertyDataSource::recheckSessionData()
{
	if(!propIterator_)
		return;
	propIterator_->recheckSessionData();
}

void PropertyDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool PropertyDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
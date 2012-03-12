#include "LFPDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

LFPDataSource::LFPDataSource()
{
	AddDefinableProperty("PropertyPath","");
	latestValue_ = "";
}

LFPDataSource::~LFPDataSource()
{
}

QSharedPointer<Asset> LFPDataSource::Create()
{
	return QSharedPointer<Asset>(new LFPDataSource());
}

void LFPDataSource::restart()
{
	propIterator_.clear();
}

QString LFPDataSource::getValue(const EventOrderIndex& index)
{
	if(!propIterator_)
	{
		propIterator_ = QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
		Q_ASSERT(propIterator_->isValid());
	}
	PropData prev = lastDataUnit_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev.index > index)
		return latestValue_;
	
	//Get new property values until the newest one is beyond the input
	//time or non-existant, then return the prior property value.
	PropData curr = propIterator_->getNextPropertyChange();
	while((curr.index <= index) && (curr.index.time_ >= 0))
	{
		prev = curr;
		curr = propIterator_->getNextPropertyChange();
	}
	//Store the last read property change (which should be beyond the input time or empty)
	lastDataUnit_ = curr;
	//Store the latest value of our property
	latestValue_ = prev.value;
	return latestValue_;
}

void LFPDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool LFPDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
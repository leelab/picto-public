#include "SpikeTrigger.h"
#include "SpikeDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

SpikeDataSource::SpikeDataSource()
{
	AddDefinableProperty(QVariant::Double,"ScaleFactor",1.0);
	AddDefinableProperty(QVariant::Int,"DecimalPlaces",6);
}

SpikeDataSource::~SpikeDataSource()
{
}

QSharedPointer<Asset> SpikeDataSource::Create()
{
	return QSharedPointer<Asset>(new SpikeDataSource());
}

void SpikeDataSource::restart()
{
}

//Note: This data source is only useful if the trigger is a spikeTrigger.  If it isn't
//there will be an error.
QSharedPointer<AnalysisValue> SpikeDataSource::getValue(const EventOrderIndex& index)
{
	return getParentAsset().staticCast<SpikeTrigger>()->getLatestValue();
}

void SpikeDataSource::recheckSessionData()
{
		return;
}

void SpikeDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool SpikeDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	if(!getParentAsset()->inherits("Picto::SpikeTrigger"))
	{
		addError("SpikeDataSource", "Spike data sources must be defined within a SpikeTrigger.");
		return false;
	}
	return true;
}
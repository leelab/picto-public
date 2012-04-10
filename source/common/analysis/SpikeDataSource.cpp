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

unsigned int SpikeDataSource::channel(int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(data)
		return data->channel;
	return -1;
}

unsigned int SpikeDataSource::unit(int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(data)
		return data->unit;
	return -1;
}

double SpikeDataSource::samplePeriod(int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(data)
		return data->samplePeriod;
	return 0;
}

unsigned int SpikeDataSource::waveSize(int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(data)
		return data->waveSize;
	return 0;
}

QString SpikeDataSource::wave(int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(data)
		return data->scaleWave(propertyContainer_->getPropertyValue("ScaleFactor").toDouble(),propertyContainer_->getPropertyValue("DecimalPlaces").toInt());
	return "";
}

double SpikeDataSource::waveValue(int waveValueIndex, int triggerIndex)
{
	QSharedPointer<SpikeData> data = getScriptValue(triggerIndex).staticCast<SpikeData>();
	if(!data)
		return 0.0;
	if(waveValueIndex >= data->waveSize)
		return 0.0;
	return data->wave.split(",",QString::SkipEmptyParts)[waveValueIndex].toDouble();
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
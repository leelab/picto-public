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

QSharedPointer<AnalysisDataIterator> SpikeDataSource::createDataIterator()
{
	return QSharedPointer<SpikeDataIterator>(
							new SpikeDataIterator(qsEngine_,session_)
							);
}

void SpikeDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool SpikeDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	if(!getParentAsset()->inherits("Picto::SpikeTrigger"))
	{
		addError("Spike data sources must be defined within a SpikeTrigger.");
		return false;
	}
	return true;
}
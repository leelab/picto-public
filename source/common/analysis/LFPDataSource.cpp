#include "LFPDataSource.h"
#include "LFPTrigger.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

LFPDataSource::LFPDataSource()
{
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
}

QSharedPointer<AnalysisValue> LFPDataSource::getValue(const EventOrderIndex& index)
{
	return getParentAsset().staticCast<LFPTrigger>()->getLatestValue();
}

unsigned int LFPDataSource::channel(int triggerIndex)
{
	QSharedPointer<LFPData> data = getScriptValue(triggerIndex).staticCast<LFPData>();
	if(data)
		return data->channel;
	return -1;
}

float LFPDataSource::value(int triggerIndex)
{
	QSharedPointer<LFPData> data = getScriptValue(triggerIndex).staticCast<LFPData>();
	if(data)
		return data->value;
	return 0.0;
}

void LFPDataSource::recheckSessionData()
{
	return;
}

void LFPDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool LFPDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	if(!getParentAsset()->inherits("Picto::LFPTrigger"))
	{
		addError("LFPDataSource", "LFP data sources must be defined within LFPTriggers.");
		return false;
	}
	return true;
}
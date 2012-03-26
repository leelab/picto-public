#include "TimeDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

TimeDataSource::TimeDataSource()
{
}

TimeDataSource::~TimeDataSource()
{
}

QSharedPointer<Asset> TimeDataSource::Create()
{
	return QSharedPointer<Asset>(new TimeDataSource());
}

void TimeDataSource::restart()
{
}

QSharedPointer<AnalysisValue> TimeDataSource::getValue(const EventOrderIndex& index)
{
	return QSharedPointer<TimeData>(new TimeData(index.time_));
}

double TimeDataSource::value(int triggerIndex)
{
	QSharedPointer<TimeData> data = getScriptValue(triggerIndex).staticCast<TimeData>();
	if(data)
		return data->time;
	return -1;
}

void TimeDataSource::recheckSessionData()
{
	return;
}

void TimeDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool TimeDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
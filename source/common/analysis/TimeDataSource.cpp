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

void TimeDataSource::fillOutScriptValue(QSharedPointer<AnalysisValue> val)
{
	val->scriptVal.setProperty("value",val->index.time_);
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
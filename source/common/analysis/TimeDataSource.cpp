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

QString TimeDataSource::getValue(const EventOrderIndex& index)
{
	return QString::number(index.time_);
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
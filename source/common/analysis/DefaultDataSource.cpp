#include "DefaultDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DefaultDataSource::DefaultDataSource()
{
}

DefaultDataSource::~DefaultDataSource()
{
}

QSharedPointer<Asset> DefaultDataSource::Create()
{
	return QSharedPointer<Asset>(new DefaultDataSource());
}

void DefaultDataSource::restart()
{
}

QSharedPointer<AnalysisValue> DefaultDataSource::getValue(const EventOrderIndex& index)
{
	return QSharedPointer<AnalysisValue>(new AnalysisValue(qsEngine_));
}

void DefaultDataSource::recheckSessionData()
{
	return;
}

void DefaultDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool DefaultDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
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

void DefaultDataSource::fillOutScriptValue(QSharedPointer<AnalysisValue> val)
{
}

void DefaultDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool DefaultDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	return true;
}
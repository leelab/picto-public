#include "AnalysisDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDataSource::AnalysisDataSource()
{
}

AnalysisDataSource::~AnalysisDataSource()
{
}

void AnalysisDataSource::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool AnalysisDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}
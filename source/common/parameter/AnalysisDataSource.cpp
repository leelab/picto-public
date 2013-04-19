#include "AnalysisDataSource.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisDataSource::AnalysisDataSource()
: AnalysisVariable()
{
}

QSharedPointer<Asset> AnalysisDataSource::Create()
{
	return QSharedPointer<Asset>(new AnalysisDataSource());
}


void AnalysisDataSource::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

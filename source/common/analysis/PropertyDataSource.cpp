#include "PropertyDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

PropertyDataSource::PropertyDataSource()
{
	AddDefinableProperty("PropertyPath","");
}

PropertyDataSource::~PropertyDataSource()
{
}

QSharedPointer<Asset> PropertyDataSource::Create()
{
	return QSharedPointer<Asset>(new PropertyDataSource());
}

QSharedPointer<AnalysisDataIterator> PropertyDataSource::createDataIterator()
{
	return QSharedPointer<PropertyDataIterator>(
							new PropertyDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("PropertyPath").toString())
							);
}

void PropertyDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool PropertyDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	return true;
}
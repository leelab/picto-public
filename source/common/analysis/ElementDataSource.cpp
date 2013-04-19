#include "ElementDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ElementDataSource::ElementDataSource()
{
}

ElementDataSource::~ElementDataSource()
{
}

QSharedPointer<Asset> ElementDataSource::Create()
{
	return QSharedPointer<Asset>(new ElementDataSource());
}

QSharedPointer<AnalysisDataIterator> ElementDataSource::createDataIterator()
{
	return QSharedPointer<ElementDataIterator>(
							new ElementDataIterator(qsEngine_,session_)
							);
}

void ElementDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool ElementDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	return true;
}
#include "FrameDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

FrameDataSource::FrameDataSource()
{
}

FrameDataSource::~FrameDataSource()
{
}

QSharedPointer<Asset> FrameDataSource::Create()
{
	return QSharedPointer<Asset>(new FrameDataSource());
}

QSharedPointer<AnalysisDataIterator> FrameDataSource::createDataIterator()
{
	return QSharedPointer<FrameDataIterator>(
							new FrameDataIterator(qsEngine_,session_)
							);
}

void FrameDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool FrameDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	return true;
}
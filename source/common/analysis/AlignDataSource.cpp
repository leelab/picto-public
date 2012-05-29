#include "AlignDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AlignDataSource::AlignDataSource()
{
}

AlignDataSource::~AlignDataSource()
{
}

QSharedPointer<Asset> AlignDataSource::Create()
{
	return QSharedPointer<Asset>(new AlignDataSource());
}

QSharedPointer<AnalysisDataIterator> AlignDataSource::createDataIterator()
{
	return QSharedPointer<AlignDataIterator>(
							new AlignDataIterator(qsEngine_,session_)
							);
}

void AlignDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AlignDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}
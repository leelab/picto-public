#include "LFPDataSource.h"
#include "LFPTrigger.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

LFPDataSource::LFPDataSource()
{
}

LFPDataSource::~LFPDataSource()
{
}

QSharedPointer<Asset> LFPDataSource::Create()
{
	return QSharedPointer<Asset>(new LFPDataSource());
}

QSharedPointer<AnalysisDataIterator> LFPDataSource::createDataIterator()
{
	return QSharedPointer<LFPDataIterator>(
							new LFPDataIterator(qsEngine_,session_)
							);
}

void LFPDataSource::postDeserialize()
{
	AnalysisDataSourceDep::postDeserialize();
}

bool LFPDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSourceDep::validateObject(xmlStreamReader))
		return false;
	if(!getParentAsset()->inherits("Picto::LFPTrigger"))
	{
		addError("LFP data sources must be defined within LFPTriggers.");
		return false;
	}
	return true;
}
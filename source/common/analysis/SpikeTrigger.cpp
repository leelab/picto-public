#include <QSqlQuery>
#include "SpikeTrigger.h"
using namespace Picto;

SpikeTrigger::SpikeTrigger()
{
}

SpikeTrigger::~SpikeTrigger()
{

}

QSharedPointer<Asset> SpikeTrigger::Create()
{
	return QSharedPointer<Asset>(new SpikeTrigger());
}

QSharedPointer<AnalysisDataIterator> SpikeTrigger::createDataIterator()
{
	return QSharedPointer<SpikeDataIterator>(
							new SpikeDataIterator(qsEngine_,session_)
							);
}

void SpikeTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool SpikeTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
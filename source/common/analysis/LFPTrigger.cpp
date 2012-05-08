#include <QSqlQuery>
#include "LFPTrigger.h"
using namespace Picto;

LFPTrigger::LFPTrigger()
{
}

LFPTrigger::~LFPTrigger()
{

}

QSharedPointer<Asset> LFPTrigger::Create()
{
	return QSharedPointer<Asset>(new LFPTrigger());
}

QSharedPointer<AnalysisDataIterator> LFPTrigger::createDataIterator()
{
	return QSharedPointer<LFPDataIterator>(
							new LFPDataIterator(qsEngine_,session_)
							);
}

void LFPTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool LFPTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
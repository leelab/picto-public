#include <QSqlQuery>
#include "AlignTrigger.h"
using namespace Picto;

AlignTrigger::AlignTrigger()
{
}

AlignTrigger::~AlignTrigger()
{

}

QSharedPointer<Asset> AlignTrigger::Create()
{
	return QSharedPointer<Asset>(new AlignTrigger());
}

QSharedPointer<AnalysisDataIterator> AlignTrigger::createDataIterator()
{
	return QSharedPointer<AlignDataIterator>(
							new AlignDataIterator(qsEngine_,session_)
							);
}

void AlignTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool AlignTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
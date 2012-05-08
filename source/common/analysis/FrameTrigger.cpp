#include <QSqlQuery>
#include "FrameTrigger.h"
using namespace Picto;

FrameTrigger::FrameTrigger()
{
}

FrameTrigger::~FrameTrigger()
{

}

QSharedPointer<Asset> FrameTrigger::Create()
{
	return QSharedPointer<Asset>(new FrameTrigger());
}

QSharedPointer<AnalysisDataIterator> FrameTrigger::createDataIterator()
{
	return QSharedPointer<FrameDataIterator>(
							new FrameDataIterator(qsEngine_,session_)
							);
}

void FrameTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool FrameTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}
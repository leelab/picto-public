#include "AnalysisFunction.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisFunction::AnalysisFunction()
{
	EXP_LINK_FACTORY_CREATION	
}

QSharedPointer<Asset> AnalysisFunction::Create()
{
	return QSharedPointer<Asset>(new AnalysisFunction());
}


void AnalysisFunction::postDeserialize()
{
	ScriptFunction::postDeserialize();
}

bool AnalysisFunction::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptFunction::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

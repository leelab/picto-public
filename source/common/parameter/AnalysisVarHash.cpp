#include "AnalysisVarHash.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVarHash::AnalysisVarHash()
: AnalysisVariable()
{
}

QSharedPointer<Asset> AnalysisVarHash::Create()
{
	return QSharedPointer<Asset>(new AnalysisVarHash());
}


void AnalysisVarHash::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVarHash::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

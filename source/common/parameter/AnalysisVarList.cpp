#include "AnalysisVarList.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVarList::AnalysisVarList()
: AnalysisVariable()
{
}

QSharedPointer<Asset> AnalysisVarList::Create()
{
	return QSharedPointer<Asset>(new AnalysisVarList());
}


void AnalysisVarList::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVarList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

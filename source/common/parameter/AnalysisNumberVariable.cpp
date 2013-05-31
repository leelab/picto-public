#include "AnalysisNumberVariable.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisNumberVariable::AnalysisNumberVariable()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisNumberVariable::Create()
{
	return QSharedPointer<Asset>(new AnalysisNumberVariable());
}


void AnalysisNumberVariable::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisNumberVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

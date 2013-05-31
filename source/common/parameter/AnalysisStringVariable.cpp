#include "AnalysisStringVariable.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisStringVariable::AnalysisStringVariable()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisStringVariable::Create()
{
	return QSharedPointer<Asset>(new AnalysisStringVariable());
}


void AnalysisStringVariable::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisStringVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

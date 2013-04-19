#include "AnalysisVar.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVar::AnalysisVar()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisVar::Create()
{
	return QSharedPointer<Asset>(new AnalysisVar());
}

void AnalysisVar::reset()
{
	AnalysisVariable::reset();
	val_ = propertyContainer_->getPropertyValue("Value");
}

void AnalysisVar::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVar::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

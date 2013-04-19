#include "AnalysisOutput.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisOutput::AnalysisOutput()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisOutput::Create()
{
	return QSharedPointer<Asset>(new AnalysisOutput());
}

void AnalysisOutput::reset()
{
	AnalysisVariable::reset();
	val_ = propertyContainer_->getPropertyValue("Value");
}

void AnalysisOutput::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

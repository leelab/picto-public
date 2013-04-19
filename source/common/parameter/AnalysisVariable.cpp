#include "AnalysisVariable.h"
#include "AnalysisExpLink.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariable::AnalysisVariable()
: Parameter()
{
	EXP_LINK_FACTORY_CREATION
}

void AnalysisVariable::reset()
{
	Parameter::reset();
}

void AnalysisVariable::postDeserialize()
{
	Parameter::postDeserialize();
}

bool AnalysisVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

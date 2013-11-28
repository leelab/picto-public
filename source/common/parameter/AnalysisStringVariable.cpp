#include "AnalysisStringVariable.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an AnalysisStringVariable.
 *	\details Adds a "Value" property.
 */
AnalysisStringVariable::AnalysisStringVariable()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}
/*! \brief Creates a new AnalysisStringVariable and returns a shared Asset pointer to it.*/
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

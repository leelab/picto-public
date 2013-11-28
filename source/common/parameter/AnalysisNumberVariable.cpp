#include "AnalysisNumberVariable.h"
#include "../memleakdetect.h"

namespace Picto {
/*! \brief Constructs an AnalysisNumberVariable.
 *	\details Adds a "Value" property.
 */
AnalysisNumberVariable::AnalysisNumberVariable()
: AnalysisVariable()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant(0));
}
/*! \brief Creates a new AnalysisNumberVariable and returns a shared Asset pointer to it.*/
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

#include "NumberVariable.h"
#include "../memleakdetect.h"

namespace Picto {
/*! \brief Constructs an NumberVariable.
 *	\details Adds a "Value" property.
 */
NumberVariable::NumberVariable()
: Variable()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant(0));
}
/*! \brief Creates a new NumberVariable and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> NumberVariable::Create()
{
	return QSharedPointer<Asset>(new NumberVariable());
}


void NumberVariable::postDeserialize()
{
	Variable::postDeserialize();
}

bool NumberVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

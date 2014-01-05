#include "StringVariable.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an StringVariable.
 *	\details Adds a "Value" property.
 */
StringVariable::StringVariable()
: Variable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}
/*! \brief Creates a new StringVariable and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> StringVariable::Create()
{
	return QSharedPointer<Asset>(new StringVariable());
}


void StringVariable::postDeserialize()
{
	Variable::postDeserialize();
}

bool StringVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

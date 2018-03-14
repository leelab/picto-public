#include "StringVariable.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an StringVariable.
 *	\details Adds a "Value" property.
 */
StringVariable::StringVariable()
: Parameter()
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
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool StringVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}
bool StringVariable::valuesAreValid(QString& warning)
{
	return true;
}

void StringVariable::fixValues()
{	
}
}; //namespace Picto

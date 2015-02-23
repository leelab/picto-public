#include <QScriptValue>

#include "InputSignal.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an InputSignal object.
*	\details Adds a Port Property to define the Port on which data will be input (Currently values are BNC0 - default, and PAR0 for BNC and Parallel ports respectively).
*	Adds an Enabled Property to define whether the current input value will be read from the input pins.  This is analogous to the Visible Property in a VisualElement.
*/
InputSignal::InputSignal()
{
}

InputSignal::~InputSignal()
{
}

void InputSignal::postDeserialize()
{
	OutputElement::postDeserialize();
}

bool InputSignal::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!OutputElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto


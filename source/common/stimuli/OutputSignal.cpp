#include <QScriptValue>

#include "OutputSignal.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs an OutputSignal object.
 *	\details Adds a Port Property to define the Port on which data will be output (Currently values are BNC0 - default, and PAR0 for BNC and Parallel ports respectively).
 *	Adds an Enabled Property to define whether the current output value will appears on the output pins.  This is analogous to the Visible Property in a VisualElement.
 */
OutputSignal::OutputSignal()
{
	AddDefinableProperty(QVariant::String,"Port","BNC0");
	AddDefinableProperty(QVariant::Bool,"Enabled",true);
}

OutputSignal::~OutputSignal()
{
}

void OutputSignal::postDeserialize()
{
	OutputElement::postDeserialize();
}

bool OutputSignal::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto


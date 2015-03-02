#include "DigitalOutput.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString DigitalOutput::type = "DigitalOutput";

/*! \brief Constructs a BinaryDataOutput object.
 *	\details Adds a Value Property to store a boolean value that will be output to the output pin.
 *	Adds a Pin Property to define the pin that is handled by this object within the Port defined
 *	in the parent OutputSignal class.
 */
DigitalOutput::DigitalOutput()
: OutputSignal()
{
	AddDefinableProperty(QVariant::Int,"Pin",0);
	AddDefinableProperty(QVariant::Bool,"Value",false);
}

/*! \brief Creates a new DigitalOutput object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> DigitalOutput::Create()
{
	return QSharedPointer<Asset>(new DigitalOutput());
}

void DigitalOutput::postDeserialize()
{
	OutputSignal::postDeserialize();
}

/*! \brief Extends OutputSignal::validateObject() to verify that the stored Pin value
 *	is not negative.
 */
bool DigitalOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputSignal::validateObject(xmlStreamReader))
		return false;
	if(getPin() < 0)
	{
		addError("Pin number cannot be less than zero");
		return false;
	}
	return true;
}

}; //namespace Picto

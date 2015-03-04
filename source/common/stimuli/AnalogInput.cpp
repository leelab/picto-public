#include "AnalogInput.h"
#include "../memleakdetect.h"

namespace Picto {
	const QString AnalogInput::type = "AnalogInput";

	/*! \brief Constructs a AnalogInput object.
	*	\details Adds a Value Property to store a boolean value that will be output to the output pin.
	*	Adds a Pin Property to define the pin that is handled by this object within the Port defined
	*	in the parent InputSignal class.
	*/
	AnalogInput::AnalogInput()
		: InputSignal()
	{
		AddDefinableProperty(QVariant::Int, "Pin", 0);
	}

	/*! \brief Creates a new AnalogInput object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> AnalogInput::Create()
	{
		return QSharedPointer<Asset>(new AnalogInput());
	}

	void AnalogInput::postDeserialize()
	{
		InputSignal::postDeserialize();
	}

	/*! \brief Extends InputSignal::validateObject() to verify that the stored Pin value
	*	is not negative.
	*/
	bool AnalogInput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!InputSignal::validateObject(xmlStreamReader))
			return false;
		if (getPin() < 0)
		{
			addError("Pin number cannot be less than zero");
			return false;
		}
		return true;
	}

}; //namespace Picto

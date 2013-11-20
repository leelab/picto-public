#include "OutputSignalController.h"
#include "../memleakdetect.h"

namespace Picto
{
/*! \brief Constructs an OutputSignalController with lowest pin number, minPin and highest pin number maxPin
 * \details We assume here that all pins between minPin and maxPin are valid
 */
OutputSignalController::OutputSignalController(int minPin, int maxPin)
:	maxPin_(maxPin),
	minPin_(minPin)
{
	Q_ASSERT(maxPin_ >= minPin_);
	pins_.resize(1+maxPin_-minPin_);
	for(int i=0;i<pins_.size();i++)
	{
		pins_[i].id = minPin_+i;
	}
}

OutputSignalController::~OutputSignalController()
{
}

/*! \brief Enables the pin with id pinId.  Disabled pins have zero voltage.  Enabled pins have whatever voltage is stored
 * for their value.
 *	\details to enable/disable all pins at once, use pinId of -1.  Returns false if the input
 *	pinId value is invalid.  Returns true otherwise.
 */
bool OutputSignalController::enablePin(bool enable, int pinId)
{
	if(pinId == -1)
	{//ALL PINS AFFECTED
		if(!pins_.size())
			return false;
		for(int i=0;i<pins_.size();i++)
		{
			pins_[i].enabled = enable;
			pins_[i].changed = true;
		}
		return true;
	}

	//Individual pin affected
	int index = pinId - minPin_;
	if(		index >= pins_.size()
		||	index < 0)
		return false;
	if(pins_[index].enabled == enable)
		return true;
	pins_[index].enabled = enable;
	pins_[index].changed = true;
	return true;
}

/*! \brief Sets the input value to the pin with id pinId.  
 *	\details If the pin is -1, the input is interpreted as an int and the binary value is applied
 *	to the group of pins in this port. ie. 0x5 would be interpreted as 0101 on a four pin port.
 *  Returns false if the input pinId value is invalid.  Returns true otherwise.
 *  \note Voltage isn't actually applied to pin until apply voltages is
 *	called.
 */
bool OutputSignalController::setValue(int pinId, QVariant value)
{
	if(pinId == -1)
	{	//All pins affected
		if(!pins_.size())
			return false;
		for(int i=0;i<pins_.size();i++)
		{
			pins_[i].value = 0x01 & (value.toInt() >> i);
			pins_[i].changed = true;
		}
		return true;
	}

	//Individual pin affected
	int index = pinId - minPin_;
	if(		index >= pins_.size()
		||	index < 0)
		return false;
	if(pins_[index].value == value)
		return true;
	pins_[index].value = value.toBool();
	pins_[index].changed = true;
	return true;
}

/*! \brief Applies the voltages set in setValue() to all pins.
 */
void OutputSignalController::updateValues()
{
	applyVoltages();
	for(int i=0;i<pins_.size();i++)
	{
		if(pins_[i].enabled || pins_[i].value)
		{	//After each frame, set all pins_ values to disabled
			//and voltages back to zero so that they will be 
			//disabled and reset if they are not set in
			//the coming frame
			pins_[i].changed = true;
			pins_[i].enabled = false;
			pins_[i].value = false;
		}
		else
			pins_[i].changed = false;
	}
}

} //namespace Picto
#include "VirtualOutputSignalController.h"
#include "../../common/memleakdetect.h"


namespace Picto
{

/*! \brief Sets up the reward controller for the input port name
 *  \details Currently the Picto system supports 'BNC0' and 'PAR0' port names.
 *	\sa OutputSignalWidget
 */
VirtualOutputSignalController::VirtualOutputSignalController(QString port)
: 	hasEnabledPins_(false),
	OutputSignalController(0,7),
	port_(port)
{
}

VirtualOutputSignalController::~VirtualOutputSignalController()
{
}

/*! \brief Returns true if the port represented by this OutputSignalController has any enabled pins.
 */
bool VirtualOutputSignalController::hasEnabledPins()
{
	return hasEnabledPins_;
}

/*! \brief Returns the number of pins on the port represented by this OutputSignalController.
 *	\details Currently all ports must have 8 pins, so this always returns 8.
 */
int VirtualOutputSignalController::numPins()
{
	return 8;
}

/*! \copydoc OutputSignalController
 *	\details In the case of the VirtualOutputSignalController, we control a virtual output port
 *	by simply emitting a pinChanged signal with pin ID, enabled value and voltage value, whenever
 *	that data changes.  The OutputSignalWidget connects to this to display it in the PictoWorkstation.
 */
void VirtualOutputSignalController::applyVoltages()
{
	bool hasEnabledPins = false;
	foreach(PinData pinData, pins_)
	{
		if(pinData.enabled)
			hasEnabledPins = true;
	}
	hasEnabledPins_ = hasEnabledPins;
	foreach(PinData pinData, pins_)
	{
		if(!pinData.changed)
			continue;
		emit pinChanged(pinData.id,pinData.enabled,pinData.value);
	}
}

}; //namespace Picto
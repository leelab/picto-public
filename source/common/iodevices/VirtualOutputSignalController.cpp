#include "VirtualOutputSignalController.h"
#include "../../common/memleakdetect.h"


namespace Picto
{

//! Sets up the reward controller
VirtualOutputSignalController::VirtualOutputSignalController(QString port)
: 	hasEnabledPins_(false),
	OutputSignalController(0,7),
	port_(port)
{
}

VirtualOutputSignalController::~VirtualOutputSignalController()
{
}

bool VirtualOutputSignalController::hasEnabledPins()
{
	return hasEnabledPins_;
}

int VirtualOutputSignalController::numPins()
{
	return 8;
}

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
#include "VirtualOutputSignalController.h"
#include "../../common/memleakdetect.h"


namespace Picto
{

//! Sets up the reward controller
VirtualOutputSignalController::VirtualOutputSignalController()
: 	hasEnabledPorts_(false),
	OutputSignalController(0,7)
{
}

VirtualOutputSignalController::~VirtualOutputSignalController()
{
}

double VirtualOutputSignalController::getVoltage(int portId)
{
	if(portId > 7 || portId < 0)
		return -100000000;
	if(ports_[portId].enabled)
	{
		return ports_[portId].level;
	}
	return 0.0;
}

bool VirtualOutputSignalController::hasEnabledPorts()
{
	return hasEnabledPorts_;
}

int VirtualOutputSignalController::numPorts()
{
	return 8;
}

void VirtualOutputSignalController::applyVoltages()
{
	bool hasEnabledPorts = false;
	foreach(PortData portData, ports_)
	{
		if(portData.enabled)
			hasEnabledPorts = true;
	}
	hasEnabledPorts_ = hasEnabledPorts;
	foreach(PortData portData, ports_)
	{
		if(!portData.changed)
			continue;
		emit portChanged(portData.id,portData.enabled,portData.level);
	}
}

}; //namespace Picto
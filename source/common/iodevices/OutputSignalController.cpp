#include "OutputSignalController.h"
#include "../memleakdetect.h"

namespace Picto
{
OutputSignalController::OutputSignalController(int minPort, int maxPort)
:	maxPort_(maxPort),
	minPort_(minPort)
{
	Q_ASSERT(maxPort_ >= minPort_);
	ports_.resize(1+maxPort_-minPort_);
	for(int i=0;i<ports_.size();i++)
	{
		ports_[i].id = minPort_+i;
	}
}

OutputSignalController::~OutputSignalController()
{
}

bool OutputSignalController::enablePort(bool enable, int portId)
{
	int index = portId - minPort_;
	if(		index >= ports_.size()
		||	index < 0)
		return false;
	if(ports_[index].enabled == enable)
		return true;
	ports_[index].enabled = enable;
	ports_[index].changed = true;
	return true;
}

bool OutputSignalController::setVoltage(int portId, double level)
{
	int index = portId - minPort_;
	if(		index >= ports_.size()
		||	index < 0)
		return false;
	if(ports_[index].level == level)
		return true;
	ports_[index].level = level;
	ports_[index].changed = true;
	return true;
}

void OutputSignalController::updateVoltages()
{
	applyVoltages();
	for(int i=0;i<ports_.size();i++)
	{
		ports_[i].changed = false;
	}
}

} //namespace Picto
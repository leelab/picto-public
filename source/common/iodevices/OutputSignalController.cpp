#include "OutputSignalController.h"
#include "../memleakdetect.h"

namespace Picto
{
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
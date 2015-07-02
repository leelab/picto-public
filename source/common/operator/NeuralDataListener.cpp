#include "NeuralDataListener.h"

#include "../memleakdetect.h"

namespace Picto {

NeuralDataListener::NeuralDataListener()
	: behavTime_(0.0)
{

}

void NeuralDataListener::setBehavioralTime(double time)
{
	behavTime_ = time;
}

}; //namespace Picto

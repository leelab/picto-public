#include "virtualeventsource.h"

bool VirtualEventSource::start(double time)
{
	startTime_ = time;
	return true;
}

bool VirtualEventSource::stop()
{
	return true;
}
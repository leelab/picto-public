#include "virtualeventsource.h"
#include "../../common/memleakdetect.h"
/*! \brief Starts this VirtualEventSource sampling data.
 *	\details Internally, this is just recording the start time so that we know we
 *	shouldn't return virtual neural events from before that time.
 */
bool VirtualEventSource::start(double time)
{
	startTime_ = time;
	return true;
}

/*! \brief Stops this VirtualEventSource from sampling data.
 *	\details Currently this function does not do anything.
 */
bool VirtualEventSource::stop()
{
	return true;
}
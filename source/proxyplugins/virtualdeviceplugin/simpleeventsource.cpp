#include "simpleeventsource.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a SimpleEventSource object.
 *	@param secPerEvent The approximate number of seconds per virtual neural event that will
 *	be returned by this source.
 *	@param secPerSample The number of seconds per sample for this virtual event source.  This is used
 *		in samplingRate().
 *	@param randRange The random variation in time when a new neural event will be generate about the 
 *		exact time at which it should occur according to the secPerEvent entry.
 */
SimpleEventSource::SimpleEventSource(double secPerEvent, double secPerSample, double randRange)
{
	secPerEvent_ = secPerEvent;
	secPerSample_ = secPerSample;
	randRange_ = randRange;
	nextRandOffset_ = 2*randRange_*(-.5+double(rand())/double(RAND_MAX));
}

/*! \brief Defines the time at which the next neural event should be generated
 *	and passes it into buildEvent to get a neural event for that time.
 *	@param time The current time.
 *	\details If the next neural event time is past the input time, an empty
 *	pointer will be returned.
 *	\note This function defines the time of the next virtual event as a random
 *	range about the precise time when it would occur in a perfect system.  See
 *	SimpleEventSource() for more details.
*/
QSharedPointer<Picto::DataUnit> SimpleEventSource::getNextEvent(double time)
{
	if((secPerEvent_ <= 0) || (secPerSample_ <= 0))
	{
		return QSharedPointer<Picto::DataUnit>();
	}

	if((startTime_ + secPerEvent_ + nextRandOffset_) < time)
	{
		startTime_ += secPerEvent_ + nextRandOffset_;
		nextRandOffset_ = 2*randRange_*(-.5+double(rand())/double(RAND_MAX));
		return buildEvent(startTime_);
	}
	return QSharedPointer<Picto::DataUnit>();
}

/*! \brief Returns the samplingRate() for this virtual event source based on the secPerSample value
 *	input into the constructor.  Units are samples per seconds.
 */
float SimpleEventSource::samplingRate()
{
	if(secPerSample_ == 0)
		return 0;
	return 1.0f/secPerSample_;
}
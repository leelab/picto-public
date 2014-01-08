#ifndef SIMPLEEVENTSOURCE_H
#define SIMPLEEVENTSOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "virtualeventsource.h"

/*! \brief A virtual event source that builds new events one by one at secPerEvent intervals
 *
 *	This interface doesn't implement the BuildEvent(time) function that is used by child interfaces
 *  to build the specific event type desired.  It simply handles the timing of event building and leaves
 *  the actual creation of the event to its child class.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SimpleEventSource : public VirtualEventSource
{
public:
	SimpleEventSource(double secPerEvent, double secPerSample, double randRange);
	virtual ~SimpleEventSource(){};
	virtual QSharedPointer<Picto::DataUnit> getNextEvent(double time);
	virtual float samplingRate();
	virtual QSharedPointer<Picto::DataUnit> buildEvent(double time) = 0;
protected:
	double secPerEvent_;	//!< The approximate number of seconds per virtual neural event that wil be returned by this source.
	double secPerSample_;	//!< secPerSample The number of seconds per sample for this virtual event source.  This is used in samplingRate().
	double randRange_;		//!< The range of random variations in time when a new neural event will be generate about the exact time at which it should occur according to secPerEvent_.
	double nextRandOffset_;	//!< The random variation in time when the next new neural event will be generate about the exact time at which it should occur according to secPerEvent_.
};
#endif
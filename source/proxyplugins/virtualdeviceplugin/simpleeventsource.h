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
	double secPerEvent_;
	double secPerSample_;
	double randRange_;
	double nextRandOffset_;
};
#endif
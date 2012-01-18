#ifndef SIMPLESPIKESOURCE_H
#define SIMPLESPIKESOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"

/*! \brief A Simple Event Source that builds simple Spike events.
 *
 *	
 */
class SimpleSpikeSource : public SimpleEventSource
{
public:
	SimpleSpikeSource(double secPerEvent, double secPerSample, int channel, int unit);
	virtual QSharedPointer<Picto::DataUnit> buildEvent(double time);
private:
	int channel_;
	int unit_;
};
#endif
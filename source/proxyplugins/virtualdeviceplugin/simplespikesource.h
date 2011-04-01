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
	SimpleSpikeSource(double secPerEvent, double secPerSample);
	virtual QSharedPointer<Picto::DataStore> buildEvent(double time);
};
#endif
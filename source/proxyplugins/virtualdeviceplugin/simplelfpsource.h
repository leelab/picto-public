#ifndef SIMPLELFPSOURCE_H
#define SIMPLELFPSOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"

/*! \brief A Simple Event Source that builds simple LFP data units.
 *
 *	
 */
class SimpleLFPSource : public SimpleEventSource
{
public:
	SimpleLFPSource(double secPerEvent, double secPerSample);
	virtual QSharedPointer<Picto::DataStore> buildEvent(double time);
private:
	double latestTime_;
};
#endif
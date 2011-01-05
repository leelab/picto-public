#ifndef SIMPLEMARKSOURCE_H
#define SIMPLEMARKSOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"
/*! \brief A Simple Event Source that builds simple mark events.
 *
 */
class SimpleMarkSource : public SimpleEventSource
{
public:
	SimpleMarkSource(double secPerMark, double secPerSample);
	virtual QSharedPointer<VirtualEvent> buildEvent(double time);
private:
	bool lastWasTrialEnd;
	short currEventCode;
};

#endif
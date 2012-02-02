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
	SimpleLFPSource(double secPerEvent, double secPerSample, int channel);
	virtual ~SimpleLFPSource();
	virtual QSharedPointer<Picto::DataUnit> buildEvent(double time);
private:
	int channel_;
	float* vals_;
	int sampsPerUnit_;
};
#endif
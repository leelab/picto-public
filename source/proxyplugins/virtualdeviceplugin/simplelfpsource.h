#ifndef SIMPLELFPSOURCE_H
#define SIMPLELFPSOURCE_H

#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"

/*! \brief A Simple Event Source that builds simple LFPDataUnitPackage objects.
 *	\details The frequency at which the LFPDataUnitPackage objects will be created
 *	along with their sample rates, and the channel represented by this SimpleLfpSource
 *	are set in the constructor.  Apart from that, objects of this class are used
 *	entirely for their buildEvent() function.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
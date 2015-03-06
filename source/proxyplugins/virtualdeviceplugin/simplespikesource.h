#ifndef SIMPLESPIKESOURCE_H
#define SIMPLESPIKESOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"

/*! \brief A Simple Event Source that builds NeuralDataUnit objects.
 *	\details The frequency at which the NeuralDataUnit objects will be created along with
 *	the range over which their timespans are randomized is set as the secPerEvent value in
 *	the constructor.  channel and unit for this source are also set there.
 *	Apart from that, objects of this class are used entirely for their buildEvent() 
 *	function which just creates NeuralDataUnit objects one after the other when SimpleEventSource
 *	says to do so.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
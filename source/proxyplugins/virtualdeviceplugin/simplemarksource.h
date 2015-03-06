#ifndef SIMPLEMARKSOURCE_H
#define SIMPLEMARKSOURCE_H


#include <QObject>
#include <QSharedPointer>

#include "simpleeventsource.h"
/*! \brief A Simple Event Source that builds AlignmentDataUnit objects.
 *	\details The frequency at which the AlignmentDataUnit objects will be created
 *	is set in the constructor.  Apart from that, objects of this class are used
 *	entirely for their buildEvent() function which just creates AlignmentDataUnit objects
 *	one after the other with increasing Alignment codes that wrap to 1 at 128.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class SimpleMarkSource : public SimpleEventSource
{
public:
	SimpleMarkSource(double secPerMark, double secPerSample);
	virtual QSharedPointer<Picto::DataUnit> buildEvent(double time);
private:
	short currEventCode;
};

#endif
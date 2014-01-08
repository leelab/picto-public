#ifndef VIRTUALEVENTSOURCE_H
#define VIRTUALEVENTSOURCE_H


#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include "..\..\common\storage\DataUnit.h"

/*! \brief This does not appear to be used anymore.  Verify this and then we should probably remove it.  It was
 *	a struct for holding virtual event data.
 */
struct VirtualEvent
{
	typedef enum{eSPIKE, eMARK} EventType;
	EventType type_;
	double timeStamp_;
	short eventCode_;
	int channel_;
	int unit_;
	QVector<short> waveform_;
	static bool LessThan(const QSharedPointer<VirtualEvent> e1, const QSharedPointer<VirtualEvent> e2){return (e1->timeStamp_ < e2->timeStamp_)?true:false;};
};

/*! \brief An interface for a VirtualEventSource that can be started and stopped, has a samplingRate and
 *	can provide sample events in the form of Picto::DataUnit objects.
 *  
 *	\details The key function here is getNextEvent(time) which returns Picto::DataUnit objects one by one, in time
 *  order until the input time arrives, at which point it should return a null pointer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class VirtualEventSource
{
public:
	virtual ~VirtualEventSource(){};
	virtual bool start(double time);
	virtual bool stop();
	/*! \brief Returns the sampling rate of this VirtualEventSource in samples per second.*/
	virtual float samplingRate() = 0;
	/*! \brief Returns Picto::DataUnit objects one by one, in time order until the input time 
	 *	arrives, at which point it returns an empty pointer.
	 */
	virtual QSharedPointer<Picto::DataUnit> getNextEvent(double time) = 0;
protected:
	double startTime_;	//!< The time at which sampling started.  No DataUnits should be returned for neural events occuring before this time.
};

#endif
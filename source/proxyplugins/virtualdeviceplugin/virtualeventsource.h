#ifndef VIRTUALEVENTSOURCE_H
#define VIRTUALEVENTSOURCE_H


#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include "..\..\common\storage\DataUnit.h"

/*! \brief A struct for holding virtual event data.
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

/*! \brief An interface for a Virtual Event Source.
 *  The key function here is getNextEvent(time) which returns event DataStores, one by one, in time
 *  order until the input time arrives, at which point it should return a null pointer.
 */
class VirtualEventSource
{
public:
	virtual bool start(double time);
	virtual bool stop();
	virtual float samplingRate() = 0;
	virtual QSharedPointer<Picto::DataUnit> getNextEvent(double time) = 0;
protected:
	double startTime_;
};

#endif
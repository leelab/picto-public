#ifndef _SlaveEventQueue_H_
#define _SlaveEventQueue_H_

#include <QVector>
#include "SlaveEvent.h"

namespace Picto {

/*! \brief A FIFO for storing ordered SlaveEvent objects.
 *	\details This class is used by SlaveExperimentDriver to hold all State changes to a master experiment in between new frame events.
 *	It is implemented as a circular buffered FIFO that expands as necessary to increasing storage needs.
 *	SlaveEvents are added using addPropChange(), addInitPropChange() and addTransActivation().  SlaveEvents are popped using takeFirstEvent().
 *	\sa SlaveEvent, SlaveExperimentDriver
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

class SlaveEventQueue
{
public:
	SlaveEventQueue();
	~SlaveEventQueue(){};

	void reset();
	void addPropChange(const int& assetId,const QString& val);
	void addInitPropChange(const int& assetId,const QString& val);
	void addTransActivation(const int& assetId);

	SlaveEvent takeFirstEvent();
private:
	int getNextAddLoc();
	int firstLoc_;
	int endLoc_;
	QVector<SlaveEvent> eventQueue_;
};


}; //namespace Picto

#endif

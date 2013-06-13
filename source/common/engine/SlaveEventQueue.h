#ifndef _SlaveEventQueue_H_
#define _SlaveEventQueue_H_

#include <QVector>
#include "SlaveEvent.h"

namespace Picto {

/*! SlaveEventQueue
 *
 *	Holds information about an experimental state change event.  Used by SlaveExperimentDriver.
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

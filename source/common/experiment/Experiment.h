#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "../common.h"

#include "../task/Task.h"
#include "../storage/DataStore.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"
#include "SessionData.h"
#include "MediaItem.h"
#include "Calibration.h"

#include <QList>
#include <QSharedPointer>

namespace Picto {

class Experiment : public DataStore
{
public:
	Experiment();

	void addTask(Picto::Task * task);


private:
	QUuid uuid_;
	QString formatID_;
	unsigned int revision_;
	QList<QSharedPointer<Task> > tasks_;
	QList<QSharedPointer<SessionData> > sessionDataElements_;
	QList<QSharedPointer<Calibration> > calibrations_;
	QList<QSharedPointer<MediaItem> > mediaItems_;

	PropertyContainer propertyContainer_;
};


}; //namespace Picto

#endif

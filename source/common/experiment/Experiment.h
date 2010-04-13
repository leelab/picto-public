/*!	\brief	The experiment object contains EVERYTHING that defines an experiment
 *
 *	An experiment contains the following objects:
 *		Tasks (which contain stat machines)
 *
 *	MATT'S NOTE:
 *	When I started to write this, there were a lot of contained objects that I 
 *	wasn't totally sure how to deal with.  Rather than ignoring them, I commented
 *	them out.  It is highly likely that we will need to add these as we go...
 */

#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "../common.h"

#include "../task/Task.h"
#include "../storage/DataStore.h"
#include "../property/PropertyContainer.h"
//#include "SessionData.h"
//#include "MediaItem.h"
//#include "Calibration.h"

#include <QList>
#include <QSharedPointer>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Experiment : public DataStore
#else
class Experiment : public DataStore
#endif
{
public:
	Experiment();

	void addTask(QSharedPointer<Task> task);
	bool runTask(QString taskName);

	QString name();
	void setName(QString name);

	QStringList getTaskNames();

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	//QUuid uuid_;
	//QString formatID_;
	//unsigned int revision_;
	QList<QSharedPointer<Task> > tasks_;
	//QList<QSharedPointer<SessionData> > sessionDataElements_;
	//QList<QSharedPointer<Calibration> > calibrations_;
	//QList<QSharedPointer<MediaItem> > mediaItems_;

	PropertyContainer propertyContainer_;
};


}; //namespace Picto

#endif

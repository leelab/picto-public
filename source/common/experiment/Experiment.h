
#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "../common.h"

#include "../task/Task.h"
#include "../statemachine/UIEnabled.h"
#include "../property/PropertyContainer.h"

//#include "SessionData.h"
//#include "MediaItem.h"
//#include "Calibration.h"

#include "../engine/PictoEngine.h"
#include "../engine/propertytable.h"


#include <QList>
#include <QSharedPointer>

namespace Picto {

/*!	\brief	The experiment object contains EVERYTHING that defines an experiment
 *
 *	An experiment contains the following objects:
 *		Tasks (which contain state machines)
 *
 *	MATT'S NOTE:
 *	When I started to write this, there were a lot of contained objects that I 
 *	wasn't totally sure how to deal with.  Rather than ignoring them, I commented
 *	them out.  It is highly likely that we will need to add these as we go...
 *
 *	NOTE: For some reason, doxygen thinks this is Picto::Picto::Experiment.  This totally
 *	screws up the alphebetization (for some reason).  I'm in a hurry to document everything,
 *	so I didn't have time to track down the source of this problem.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Experiment : public UIEnabled
#else
class Experiment : public UIEnabled
#endif
{
	Q_OBJECT
public:
	static QSharedPointer<Experiment> Create();
	void setEngine(QSharedPointer<Engine::PictoEngine> engine);
	void addTask(QSharedPointer<Task> task);
	bool runTask(QString taskName);
	bool jumpToState(QStringList path, QString state);

	QStringList getTaskNames();
	QSharedPointer<Task> getTaskByName(QString taskName);

	//DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual QString defaultTagName(){return "Experiment";};

	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	Experiment();
	//QUuid uuid_;
	//QString formatID_;
	//unsigned int revision_;
	QList<QSharedPointer<Task> > tasks_;
	const QString latestSyntaxVersion_;
	QSharedPointer<PropertyTable> propTable_;
	QSharedPointer<Engine::PictoEngine> engine_;
	//QList<QSharedPointer<SessionData> > sessionDataElements_;
	//QList<QSharedPointer<Calibration> > calibrations_;
	//QList<QSharedPointer<MediaItem> > mediaItems_;
};


}; //namespace Picto

#endif

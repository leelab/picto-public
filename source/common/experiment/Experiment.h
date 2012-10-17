
#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "../common.h"

#include "../task/Task.h"
#include "../statemachine/scriptablecontainer.h"
#include "../statemachine/UIEnabled.h"
#include "../property/PropertyContainer.h"

//#include "SessionData.h"
//#include "MediaItem.h"
//#include "Calibration.h"


#include <QList>
#include <QSharedPointer>

#include "../engine/PictoEngine.h"
#include "../engine/propertytable.h"
#include "../storage/propertylookup.h"

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
class PICTOLIB_API Experiment : public ScriptableContainer
#else
class Experiment : public ScriptableContainer
#endif
{
	Q_OBJECT
	Q_PROPERTY(double xOffset READ getXOffset WRITE setXOffset);
	Q_PROPERTY(double yOffset READ getYOffset WRITE setYOffset);
	Q_PROPERTY(double xGain READ getXGain WRITE setXGain);
	Q_PROPERTY(double yGain READ getYGain WRITE setYGain);
	Q_PROPERTY(double xySignalShear READ getXYSignalShear WRITE setXYSignalShear);
public:
	static QSharedPointer<Experiment> Create();
	virtual ~Experiment(){};
	virtual QString getName(){return "Experiment";};
	virtual void setName(QString newName){};
	void setEngine(QSharedPointer<Engine::PictoEngine> engine);
	void addTask(QSharedPointer<Task> task);
	bool runTask(QString taskName);
	bool jumpToState(QStringList path, QString state);

	QStringList getTaskNames();
	QSharedPointer<Task> getTaskByName(QString taskName);

	virtual QString assetType(){return "Experiment";};
	//An experiment doesn't use its InitPropertyContainer for its UI properties.
	virtual QSharedPointer<PropertyContainer> getUIPropertyContainer(){return getPropertyContainer();};
	//QString getPropLookupXml();
	//QString getTransLookupXml();

	//DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual QString defaultTagName(){return "Experiment";};

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	void setXOffset(double value) { propertyContainer_->setPropertyValue("XOffset",value); };
	double getXOffset(){return propertyContainer_->getPropertyValue("XOffset").toDouble(); };
	void setYOffset(double value) { propertyContainer_->setPropertyValue("YOffset",value); };
	double getYOffset(){return propertyContainer_->getPropertyValue("YOffset").toDouble(); };
	void setXGain(double value) { propertyContainer_->setPropertyValue("XGain",value); };
	double getXGain(){return propertyContainer_->getPropertyValue("XGain").toDouble(); };
	void setYGain(double value) { propertyContainer_->setPropertyValue("YGain",value); };
	double getYGain(){return propertyContainer_->getPropertyValue("YGain").toDouble(); };
	void setXYSignalShear(double value) { propertyContainer_->setPropertyValue("XYSignalShear",value); };
	double getXYSignalShear(){return propertyContainer_->getPropertyValue("XYSignalShear").toDouble(); };

	Experiment();
	//QUuid uuid_;
	//QString formatID_;
	//unsigned int revision_;
	QList<QSharedPointer<Task> > tasks_;
	const QString latestSyntaxVersion_;
	QSharedPointer<PropertyTable> propTable_;
	QSharedPointer<Engine::PictoEngine> engine_;
	bool signalCoeffInitialized_;
	//QList<QSharedPointer<SessionData> > sessionDataElements_;
	//QList<QSharedPointer<Calibration> > calibrations_;
	//QList<QSharedPointer<MediaItem> > mediaItems_;
private slots:
	void updateSignalCoefficients(QSharedPointer<Property> changedProp);
};


}; //namespace Picto

#endif

#ifndef _TARGETCONTROLLER_H_
#define _TARGETCONTROLLER_H_

#include "../common.h"
#include "Timer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"
#include "ControlTarget.h"

namespace Picto {

/*! \brief This controller watches for fixation on a single target
 *
 *	A TargetController watches a single target and returns true if the subject
 *	has fixated on it for a specified period of time.  It is possible to customize
 *	a TargetController in a lot of different ways
 *
 *	TargetControllers have the following properties:
 *	 SignalChannel: The name of the signal channel to use
 *	 Shape: The shape of the target area (Ellipse or Rectangle)
 *	 Target: The target area (as a rectangle)
 *	 TimeUnits: Units in which times are stored
 *	 TotalTime: The maximum time before a failure is returned
 *	 FixationTime: The required fixation time for success to be returned
 *	 MinInitialAcquisitionTime: Ignore all input that occurs before this time
 *	 MaxInitialAcquisitionTime: The target must be acquired by this time or a failure is returned
 *	 MaxReacquisitionTime: The amount of time allowed for reacquisition (assuming it is allowed)
 *	 Reacquisiton Allowed: Can the subject leave and re-enter the target?
 *
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API TargetController : public ControlElement
#else
class TargetController : public ControlElement
#endif
{
	Q_OBJECT
	Q_PROPERTY(int fixationTime READ getFixationTime WRITE setFixationTime)
	Q_PROPERTY(int totalTime READ getTotalTime WRITE setTotalTime)
	Q_PROPERTY(int minAcquisitionTime READ getMinAcqTime WRITE setMinAcqTime)
	Q_PROPERTY(int maxAcquisitionTime READ getMaxAcqTime WRITE setMaxAcqTime)
	Q_PROPERTY(int maxReacquisitionTime READ getMaxReacqTime WRITE setMaxReacqTime)
	Q_PROPERTY(bool reacquisitionAllowed READ getReacqAllowed WRITE setReacqAllowed)

public:
	TargetController();
	virtual ~TargetController(){};

	static ControlElement* NewTargetController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QString getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);
	void stop(QSharedPointer<Engine::PictoEngine> engine);

	int getFixationTime(){return propertyContainer_->getPropertyValue("FixationTime").toInt();}; 
	void setFixationTime(int time){propertyContainer_->setPropertyValue("FixationTime",time);};
	int getTotalTime(){return propertyContainer_->getPropertyValue("TotalTime").toInt();};
	void setTotalTime(int time){propertyContainer_->setPropertyValue("TotalTime",time);};
	int getMinAcqTime(){return propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toInt();};
	void setMinAcqTime(int time){propertyContainer_->setPropertyValue("MinInitialAcquisitionTime",time);};
	int getMaxAcqTime(){return propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toInt();};
	void setMaxAcqTime(int time){propertyContainer_->setPropertyValue("MaxInitialAcquisitionTime",time);};
	int getMaxReacqTime(){return propertyContainer_->getPropertyValue("MaxReacquisitionTime").toInt();};
	void setMaxReacqTime(int time){propertyContainer_->setPropertyValue("MaxReacquisitionTime",time);};
	bool getReacqAllowed(){return propertyContainer_->getPropertyValue("ReacquisitionAllowed").toInt();};
	void setReacqAllowed(bool allowed){propertyContainer_->setPropertyValue("ReacquisitionAllowed",allowed);};

	////DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QPair<QString,QString>>  getScripts();
	virtual void scriptableContainerWasReinitialized();

private:
	bool insideTarget(QSharedPointer<Engine::PictoEngine> engine);

	bool targetAcquired_;
	bool initialAcquisitionOccurred_;
	bool waitingForReacquisition_;
	QWeakPointer<ControlTarget> controlTarget_;

	Controller::Timer cumulativeTimer_;
	Controller::Timer acquisitionTimer_;	
	Controller::Timer reacquisitionTimer_;

	QStringList unitList_;
	//QStringList shapeList_;

	QString result_;

	QSharedPointer<SignalChannel> signal_;
};


}; //namespace Picto

#endif

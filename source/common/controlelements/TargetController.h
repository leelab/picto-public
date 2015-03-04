#ifndef _TARGETCONTROLLER_H_
#define _TARGETCONTROLLER_H_

#include "../common.h"
#include "FrameResolutionTimer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"
#include "ControlTarget.h"

namespace Picto {

/*! \brief Returns a result based on whether the subject fixated on a set target or failed to do so according to preset parameters.
 *	\details The Target Controller is used to check if the user has fixated on a ControlTarget.  
 *	The target is defined in the 'ControlTarget' property.  While the ControlTarget is active, the target being 
 *	considered will be visible to the operator.  The possible results from a TargetController include 
 *	Success, Broke Fixation, Total Time Excceeded, Initial Aquisition Time Exceeded, Reaquisition Time Exceeded.
 *	\note I am fully aware that "Total Time Excceeded" is spelled wrong.  Since Picto references element names
 *	for transitions, fixing this name would break many many things.  I'm not going to do it.  It should be possible
 *	to fix this by using the Automatic upgrade system (see Asset::upgradeVersion()) and creating some new system
 *	for upgrading old Session databases as well.  If you have a lot of time on your hands, go for it.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API TargetController : public ControlElement
#else
class TargetController : public ControlElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the current value of the necessary 'FixationTime' considered successful fixation.
	 * \details FixationTime is the amount of time that the user must continuously fixate on the target
	 *	defined in the 'ControlTarget' property before 'TotalTime' is reached in order for the 'Success'
	 *	result to be triggered.
	 */
	Q_PROPERTY(int fixationTime READ getFixationTime WRITE setFixationTime);
	/*! \brief Sets/Gets the current value of the 'TotalTime' that the user has in order to correctly fixate before 'Total Time Exceeded' is triggered.
	 * \details TotalTime is the amount of time that the user has to succesfully fixate on the target defined in 'ControlTarget'
	 *	for a continuous 'FixationTime'.
	 *	\note If the user is not fixating on the target at a time beyond 'TotalTime'-'FixationTime', 'Total Time Exceeded' will be
	 *	triggered.
	 */
	Q_PROPERTY(int totalTime READ getTotalTime WRITE setTotalTime);
	/*! \brief Sets/Gets the current value of 'MinInitialAcquisitionTime'.
	 *	\details MinInitialAcquisitionTime is the time before which any fixation on the target is not counted toward the
	 *	'FixationTime'.
	 */
	Q_PROPERTY(int minAcquisitionTime READ getMinAcqTime WRITE setMinAcqTime);
	/*! \brief Sets/Gets the current value of 'MaxInitialAcquisitionTime'
	 * \details The time before which the user must begin their initial fixation.
	 *	If the user does not fixate on the target for at least a single frame before
	 *	this time, 'Initial Aquistion Time Exceeded' will be triggered.
	 */
	Q_PROPERTY(int maxAcquisitionTime READ getMaxAcqTime WRITE setMaxAcqTime);
	/*! \brief Sets/Gets the current value of 'MaxReacquisitionTime'
	 * \details When 'ReaquisitionAllowed' is true, this is the amount of time that the user has to look outside the target
	 *	after having left it before 'Reacquisition Time Exceeded' is triggered.
	 */
	Q_PROPERTY(int maxReacquisitionTime READ getMaxReacqTime WRITE setMaxReacqTime);
	/*! \brief Sets/Gets the current value of 'ReacquisitionAllowed'
	 * \details If true, the user's eye position may enter and exit the target defined in 'ControlTarget' without triggering a
	 *	'Broke Fixation' result until they either fixate for 'FixationTime' or 'TotalTime' passes.
	 */
	Q_PROPERTY(bool reacquisitionAllowed READ getReacqAllowed WRITE setReacqAllowed);

public:
	TargetController();
	virtual ~TargetController(){};

	static ControlElement* NewTargetController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Result> getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);
	void stop(QSharedPointer<Engine::PictoEngine> engine);
	virtual void upgradeVersion(QString deserializedVersion);

	void activateTargets();
	void deactivateTargets();

	/*! \brief Gets the current value of the FixationTime property
	 *	\sa fixationTime
	 */
	int getFixationTime(){return propertyContainer_->getPropertyValue("FixationTime").toInt();};
	/*! \brief Gets the current value of the FixationTime property
	 *	\sa fixationTime
	 */
	void setFixationTime(int time){propertyContainer_->setPropertyValue("FixationTime",time);};
	/*! \brief Gets the current value of the TotalTime property
	 *	\sa totalTime
	 */
	int getTotalTime(){return propertyContainer_->getPropertyValue("TotalTime").toInt();};
	/*! \brief Gets the current value of the TotalTime property
	 *	\sa totalTime
	 */
	void setTotalTime(int time){propertyContainer_->setPropertyValue("TotalTime",time);};
	/*! \brief Gets the current value of the MinInitialAcquisitionTime property
	 *	\sa minAcquisitionTime
	 */
	int getMinAcqTime(){return propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toInt();};
	/*! \brief Gets the current value of the MinInitialAcquisitionTime property
	 *	\sa minAcquisitionTime
	 */
	void setMinAcqTime(int time){propertyContainer_->setPropertyValue("MinInitialAcquisitionTime",time);};
	/*! \brief Gets the current value of the MaxInitialAcquisitionTime property
	 *	\sa maxAcquisitionTime
	 */
	int getMaxAcqTime(){return propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toInt();};
	/*! \brief Gets the current value of the MaxInitialAcquisitionTime property
	 *	\sa maxAcquisitionTime
	 */
	void setMaxAcqTime(int time){propertyContainer_->setPropertyValue("MaxInitialAcquisitionTime",time);};
	/*! \brief Gets the current value of the MaxReacquisitionTime property
	 *	\sa maxReacquisitionTime
	 */
	int getMaxReacqTime(){return propertyContainer_->getPropertyValue("MaxReacquisitionTime").toInt();};
	/*! \brief Gets the current value of the MaxReacquisitionTime property
	 *	\sa maxReacquisitionTime
	 */
	void setMaxReacqTime(int time){propertyContainer_->setPropertyValue("MaxReacquisitionTime",time);};
	/*! \brief Gets the current value of the ReacquisitionAllowed property
	 *	\sa reacquisitionAllowed
	 */
	bool getReacqAllowed(){return propertyContainer_->getPropertyValue("ReacquisitionAllowed").toInt();};
	/*! \brief Gets the current value of the ReacquisitionAllowed property
	 *	\sa reacquisitionAllowed
	 */
	void setReacqAllowed(bool allowed){propertyContainer_->setPropertyValue("ReacquisitionAllowed",allowed);};

	virtual QString getUITemplate(){return "TargetController";};
	virtual QString friendlyTypeName(){return "Target Controller";};
public slots:
	bool userOnTarget();
	bool userEnteredTarget();
	bool userExitedTarget();
	////DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual void scriptableContainerWasReinitialized();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);

private:
	bool isDonePrivate(QSharedPointer<Engine::PictoEngine> engine);
	bool insideTarget(QSharedPointer<Engine::PictoEngine> engine);

	bool targetAcquired_;
	bool initialAcquisitionOccurred_;
	bool waitingForReacquisition_;
	QWeakPointer<ControlTarget> controlTarget_;
	int frameCtr_;

	QSharedPointer<Controller::FrameResolutionTimer> cumulativeTimer_;
	QSharedPointer<Controller::FrameResolutionTimer> acquisitionTimer_;	
	QSharedPointer<Controller::FrameResolutionTimer> reacquisitionTimer_;

	QStringList unitList_;
	//QStringList shapeList_;

	QString result_;

	QSharedPointer<SignalChannel> signal_;

private slots:
	void controlTargetNameEdited();
};


}; //namespace Picto

#endif

#ifndef _FIXATIONCONTROLLER_H_
#define _FIXATIONCONTROLLER_H_

#include "../common.h"
#include "FrameResolutionTimer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"
#include "ControlTarget.h"

namespace Picto {

/*! \brief Returns a result based on whether the subject fixated on a set target or failed to do so according to preset
 *	parameters.
 *	\details The Fixation Controller is used to check if the user has fixated on a ControlTarget.  
 *	The target is defined in the 'ControlTarget' property.  While the ControlTarget is active, the target being 
 *	considered will be visible to the operator.  The possible results from a FixationController include 
 *	Success, Broke Fixation, and Initial Acquisition Time Exceeded.
 *	\note This is an updated version of the TargetController.  It turns out that the TargetController had undesired
 *	and unintended behaviors, and this is meant to address those.  The FixationTime is how long the state will last in
 *	the case of success.  The way, the duration of the state can be strictly controlled.  Reacquisition Time does not count
 *	against success.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API FixationController : public ControlElement
#else
class FixationController : public ControlElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the current value of the necessary 'FixationTime' considered successful fixation.
	 *  \details FixationTime is the amount of time that the user must continuously fixate on the target defined in the
	 *	'ControlTarget' property in order for the 'Success' result to be triggered.
	 */
	Q_PROPERTY(int fixationTime READ getFixationTime WRITE setFixationTime);
	/*! \brief Sets/Gets the current value of 'MinInitialAcquisitionTime'.
	*	\details MinInitialAcquisitionTime is the time before which any fixation on the target is not counted toward the
	*	'FixationTime'.
	*/
	Q_PROPERTY(int minAcquisitionTime READ getMinAcqTime WRITE setMinAcqTime);
	/*! \brief Sets/Gets the current value of 'MaxInitialAcquisitionTime'
	 *  \details The time before which the user must begin their initial fixation.  If the user does not fixate on the
	 *	target for at least a single frame before this time, 'Initial Acquisition Time Exceeded' will be triggered.
	 */
	Q_PROPERTY(int maxAcquisitionTime READ getMaxAcqTime WRITE setMaxAcqTime);
	/*! \brief Sets/Gets the current value of 'MaxReacquisitionTime'
	 *  \details This is the amount of time that the user has to look outside the target after having left it before
	 *	'Reacquisition Time Exceeded' is triggered.
	 */
	Q_PROPERTY(int maxReacquisitionTime READ getMaxReacqTime WRITE setMaxReacqTime);

public:
	FixationController();
	virtual ~FixationController(){};

	static ControlElement* NewFixationController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Result> getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);
	void stop(QSharedPointer<Engine::PictoEngine> engine);

	void activateTargets();
	void deactivateTargets();

	/*! \brief Gets the current value of the FixationTime property
	 *	\sa fixationTime
	 */
	int getFixationTime(){return propertyContainer_->getPropertyValue("FixationTime").toInt();};
	/*! \brief Sets the current value of the FixationTime property
	 *	\sa fixationTime
	 */
	void setFixationTime(int time){propertyContainer_->setPropertyValue("FixationTime",time);};
	/*! \brief Gets the current value of the MinInitialAcquisitionTime property
	 *	\sa minAcquisitionTime
	 */
	int getMinAcqTime(){return propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toInt();};
	/*! \brief Sets the current value of the MinInitialAcquisitionTime property
	 *	\sa minAcquisitionTime
	 */
	void setMinAcqTime(int time){propertyContainer_->setPropertyValue("MinInitialAcquisitionTime",time);};
	/*! \brief Gets the current value of the MaxInitialAcquisitionTime property
	 *	\sa maxAcquisitionTime
	 */
	int getMaxAcqTime(){return propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toInt();};
	/*! \brief Sets the current value of the MaxInitialAcquisitionTime property
	 *	\sa maxAcquisitionTime
	 */
	void setMaxAcqTime(int time){propertyContainer_->setPropertyValue("MaxInitialAcquisitionTime",time);};
	/*! \brief Gets the current value of the MaxReacquisitionTime property
	 *	\sa maxReacquisitionTime
	 */
	int getMaxReacqTime(){return propertyContainer_->getPropertyValue("MaxReacquisitionTime").toInt();};
	/*! \brief Sets the current value of the MaxReacquisitionTime property
	 *	\sa maxReacquisitionTime
	 */
	void setMaxReacqTime(int time){propertyContainer_->setPropertyValue("MaxReacquisitionTime",time);};

	virtual QString getUITemplate(){return "FixationController";};
	virtual QString friendlyTypeName(){return "Fixation Controller";};
public slots:
	bool userOnTarget();
	bool userEnteredTarget();
	bool userExitedTarget();
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

	QSharedPointer<Controller::FrameResolutionTimer> acquisitionTimer_;	
	QSharedPointer<Controller::FrameResolutionTimer> reacquisitionTimer_;

	QStringList unitList_;

	QString result_;

	QSharedPointer<SignalChannel> signal_;

private slots:
	void controlTargetNameEdited();
};


}; //namespace Picto

#endif

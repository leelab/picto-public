#ifndef _CHOICECONTROLLER_H_
#define _CHOICECONTROLLER_H_

#include <QList>

#include "../common.h"
#include "FrameResolutionTimer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"
#include "ControlTargetResult.h"

namespace Picto {

/*!	\brief Returns a result based on the region selected by the subject
 *	\details The Choice Controller, like the TargetController is used to check if the 
 *	user has fixated on a ControlTarget.  In the case of the ChoiceController 
 *	however, multiple targets can be defined using internal ControlTargetResult
 *	elements and each one of these results creates a different possible result 
 *	outcome from the Choice Controller.  Like in the case of a Target Controller,
 *	any Control Target that is being actively considered will be marked to the 
 *	operator with a green outline while this ChoiceController is active.  The 
 *	possible results from a ChoiceController are Total Time Exceeded, Broke Fixation,
 *	and of course the manually added ControlTargetResults.
 *	\note The Choice Controller is essentially a convenience 
 *	element.  Its operation can be fairly precisely simulated by simply using 
 *	multiple Target Controllers.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ChoiceController : public ControlElement
#else
class ChoiceController : public ControlElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the current value of the necessary 'FixationTime' considered successful fixation.*/
	Q_PROPERTY(int fixationTime READ getFixationTime WRITE setFixationTime);
	/*! \brief Sets/Gets the current value of the 'TotalTime' that the user has in order to correctly fixate before 'Total Time Exceeded' is triggered.*/
	Q_PROPERTY(int totalTime READ getTotalTime WRITE setTotalTime);
public:
	ChoiceController();
	virtual ~ChoiceController(){};

	static ControlElement* NewChoiceController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Result> getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);
	void stop(QSharedPointer<Engine::PictoEngine> engine);

	void activateTargets();
	void deactivateTargets();
	virtual void upgradeVersion(QString deserializedVersion);

	/*! \brief Gets the current value of the FixationTime property
	*	\sa fixationTime
	*/
	int getFixationTime(){ return propertyContainer_->getPropertyValue("FixationTime").toInt(); };
	/*! \brief Sets the current value of the FixationTime property
	*	\sa fixationTime
	*/
	void setFixationTime(int time){ propertyContainer_->setPropertyValue("FixationTime", time); };
	/*! \brief Gets the current value of the TotalTime property
	*	\sa totalTime
	*/
	int getTotalTime(){ return propertyContainer_->getPropertyValue("TotalTime").toInt(); };
	/*! \brief Sets the current value of the TotalTime property
	*	\sa totalTime
	*/
	void setTotalTime(int time){propertyContainer_->setPropertyValue("TotalTime",time);};

	virtual QString getUITemplate(){return "ChoiceController";};
	virtual QString friendlyTypeName(){return "Choice Controller";};
public slots:
	bool userOnTarget();
	bool userEnteredTarget();
	bool userExitedTarget();
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	bool isDonePrivate(QSharedPointer<Engine::PictoEngine> engine);
	QString insideTarget(QSharedPointer<Engine::PictoEngine> engine);
	//bool checkSingleTarget(QRect targetRect);

	QSharedPointer<Controller::FrameResolutionTimer>  cumulativeTimer_;
	QSharedPointer<Controller::FrameResolutionTimer>  acquisitionTimer_;	
	QSharedPointer<Controller::FrameResolutionTimer>  reacquisitionTimer_;

	QStringList unitList_;
	//QStringList shapeList_;

	bool isDone_; //<! 
	bool targetAcquired_;
	QString result_;
	int timeUnits_;
	int reentriesAllowedListIndex_;

	QString lastTarget_;

	QSharedPointer<SignalChannel> signal_;
	QList<QSharedPointer<ControlTargetResult>> targets_;
	int frameCtr_;

};


}; //namespace Picto

#endif

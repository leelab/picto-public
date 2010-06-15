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

#ifndef _TARGETCONTROLLER_H_
#define _TARGETCONTROLLER_H_

#include "../common.h"
#include "Timer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API TargetController : public ControlElement
#else
class TargetController : public ControlElement
#endif
{
public:
	TargetController();

	static ControlElement* NewTargetController();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QString getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	bool insideTarget(QSharedPointer<Engine::PictoEngine> engine);

	bool targetAcquired_;
	bool initialAcquisitionOccurred_;
	bool waitingForReacquisition_;

	Controller::Timer cumulativeTimer_;
	Controller::Timer acquisitionTimer_;	
	Controller::Timer reacquisitionTimer_;

	QStringList unitList_;
	QStringList shapeList_;
	QStringList reacquisitionAllowedList_;

	QString result_;

	QSharedPointer<SignalChannel> signal_;
};


}; //namespace Picto

#endif

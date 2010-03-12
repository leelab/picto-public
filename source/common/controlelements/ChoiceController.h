/*!	\Brief Returns a result based on the region selected by the subject
 *
 *	A choice controller allows the experimenter to define multiple regions
 *	on the screen.  The controller will return when the subject has fixated
 *	on a single region for a fixed amount of time.
 *
 *	A choice controller has the following properties:
 *		SignalChannel: The name of the signal channel to use
 *		Shape: The shape of the target areas (Ellipse or Rectangle)
 *		TargetNameX: The name of target X
 *		TargetX: The area (as a rectangle) of Target X
 *		TimeUnits: Units in which times are stored
 *		TotalTime: The maximum time before a failure is returned
 *		FixationTime: The required fixation time for success to be returned
 *		AllowReentries: Is the subject allowed to leave a target without failing?
 */

#ifndef _CHOICECONTROLLER_H_
#define _CHOICECONTROLLER_H_

#include "../common.h"
#include "Timer.h"
#include "ControlElement.h"
#include "../engine/SignalChannel.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ChoiceController : public ControlElement
#else
class ChoiceController : public ControlElement
#endif
{
public:
	ChoiceController();

	static ControlElement* NewChoiceController();
	static QString ControllerType();

	bool addTarget(QString targetName, QRect target);

	bool isDone();
	QString getResult();
	void start();

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString insideTarget();
	bool checkSingleTarget(QRect targetRect);

	Controller::Timer cumulativeTimer_;
	Controller::Timer acquisitionTimer_;	
	Controller::Timer reacquisitionTimer_;

	QStringList unitList_;
	QStringList shapeList_;
	QStringList reentriesAllowedList_;

	bool isDone_;
	bool targetAcquired_;
	QString result_;

	QString lastTarget_;

	QSharedPointer<SignalChannel> signal_;

};


}; //namespace Picto

#endif

/*! \brief A time based controller
 *
 *	This controller ignores any and all inputs, and simply returns "Success"
 *	when a fixed amount of time has expired.  Since the time values are properties
 *	it is possible to change them at runtime through Javascript.
 */

#ifndef _STOPWATCH_CONTROLLER_H_
#define _STOPWATCH_CONTROLLER_H_

#include "../common.h"
#include "Timer.h"

#include "ControlElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API StopwatchController : public ControlElement
#else
class StopwatchController : public ControlElement
#endif
{
	Q_OBJECT

public:
	StopwatchController();

	static ControlElement* NewStopwatchController();
	static QString ControllerType();

	bool isDone();
	QString getResult();
	void start();

	void setTime(int time, Controller::TimerUnits::TimerUnits units);

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QStringList unitList_;
	Controller::Timer timer_;
};


}; //namespace Picto

#endif
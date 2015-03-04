#ifndef _STOPWATCH_CONTROLLER_H_
#define _STOPWATCH_CONTROLLER_H_

#include "../common.h"
#include "FrameResolutionTimer.h"

#include "ControlElement.h"

namespace Picto {

/*! \brief A time based controller
 *
 *	This is a simple timer controller that triggers its Success result as soon as its set timeout 
 *	runs out. Please note that since time is checked only once per frame the time that the next 
 *	frame from a state following this one will appear will be anywhere from one to two frames beyond 
 *	the set 'Time'.  This is true of all time based conditions in ControlElement objects.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API StopwatchController : public ControlElement
#else
class StopwatchController : public ControlElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the current value of the Time property which defines the time at which 'Success' will be triggered*/
	Q_PROPERTY(int timeout READ getTime() WRITE setTime());
public:
	StopwatchController();
	virtual ~StopwatchController(){};

	static ControlElement* NewStopwatchController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Result> getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);

	void setTime(int time, Controller::TimerUnits::TimerUnits units);
	/*! \brief Sets the current value of the Time property which defines the time at which 'Success' will be triggered*/
	void setTime(int time){propertyContainer_->setPropertyValue("Time",time);};
	/*! \brief Gets the current value of the Time property which defines the time at which 'Success' will be triggered*/
	int getTime(){return propertyContainer_->getPropertyValue("Time").toInt();};

	virtual QString getUITemplate(){return "StopwatchController";};
	virtual QString friendlyTypeName(){return "Stopwatch Controller";};
	////DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QStringList unitList_;
	QSharedPointer<Controller::FrameResolutionTimer> timer_;
};


}; //namespace Picto

#endif

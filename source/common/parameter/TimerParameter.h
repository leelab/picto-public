#ifndef _TIMERPARAMETER_H_
#define _TIMERPARAMETER_H_

#include "../common.h"

#include "parameter.h"
#include "../controlelements/FrameResolutionTimer.h"

namespace Picto {

/*!	\brief A TimerParameter for checking how much time has passed between different sections of the 
 *	StateMachine.
 *
 *	This Timer works according to Frame resolution.  This means that when you call restart() it will set
 *	the time of the last frame's first phosphor presentation as if it is time zero.  All calls to getValue()
 *	or uses of the "value" script parameter will return the time of the last frame's first phoshpor presentation
 *	with respect to the restart() time.  We do things this way because the only real meaningful times in
 *	a Picto experiment are the times at which frames are presented.  It is much more logical to return everything
 *	in terms of these times rather than in terms of the time at which a certain section of code happened to be
 *	executed.
 *
 *	The TimerParameter is essentially just an interface for a FrameResolutionTimer.  See that
 *	class for more detail.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API TimerParameter : public Parameter
#else
class TimerParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Returns the latest value on the timer according to the units in the TimerUnits Property.
	 *	\details Using this script property to set the value has the affect of restarting the timer and 
	 *	marking the restarted time, not as time zero but as the time that is input.
	 *	ie. \code
			TimerParameterName.value = 10;
			// A 16.6ms frame passes...
			var timerValue = timerParameterName.value;
			//timerValue will be equal to 26.6
		\endcode
		\note Don't forget that everything functions in frame resolution.  See getValue() for more details.
	 */
	Q_PROPERTY(int value READ getValue WRITE setValue)
public slots:
	void restart();

public:
	TimerParameter();
	virtual ~TimerParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	virtual void enteredScope();
	int getValue();
	/*! \brief Restarts the timer and sets the time of the latest presented frame as if it was
	 *	equal to the input value.
	 *	ie. \code
			TimerParameterName.setValue(10);
			// A 16.6ms frame passes...
			double timerValue = timerParameterName.getValue();
			//timerValue will be equal to 26.6
		\endcode
	*/
	void setValue(int val){restart();time_ = val;};

	virtual QString friendlyTypeName(){return "Timer";};
	virtual QString getUITemplate(){return "Timer";};
	virtual QString getUIGroup(){return "Sensors";};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Controller::FrameResolutionTimer> timer_;
	QStringList unitList_;
	int time_;


};


}; //namespace Picto

#endif

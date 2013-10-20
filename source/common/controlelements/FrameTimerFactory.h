#ifndef _FrameTimerFactory_H_
#define _FrameTimerFactory_H_
#include <QDateTime>
#include <QVector>
#include <QWeakPointer>
#include <QSharedPointer>

#include "FrameResolutionTimer.h"
#include "../common.h"

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {
namespace Controller {

/*! \brief A timer object used by ControlElements
 *
 *	The controlElements require precise timing aligned with frame intervals.  This object serves that
 *	purpose.  The values that the timer returns are according to the time at which the latest first
 *	phosphor occured.
 *
 *	NOTE: At the moment, this timer is part of the ControlElement namespace, to
 *	indicate that it is designed for use with ControlElements only.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameTimerFactory
#else
class FrameTimerFactory
#endif
{

public:
	FrameTimerFactory();
	~FrameTimerFactory();

	QSharedPointer<FrameResolutionTimer> createTimer();
	//Called by the rendering engine to mark the time at which the latest frame occured.
	//This is used by all timers created by this factory in calculating elapsedTime
	void setLastFrameTime(double frameTime);
	//In the case of analysis, we can set the time at which all upcoming property changes
	//will take effect (ie. The time of the frame following them).  While elapsed time needs
	//to go according to the previous frame.  Absolute time (for comparison to timed signals
	//like eye position and spike times) needs to go according to the frame after the property
	//change.
	//Example:  A State ends as soon as a monkey fixates on a target and a timer is started on entry into
	//the following state to see how long it takes the monkey to fixate on a new target in this
	//state.
	//In this case, Picto detects the monkey's fixation just after the first phosphor of the frame
	//on which he fixated.  The state then changes.  Now a new target appears on the first frame in the 
	//new state and a timer is started just after this first frame.  If the monkey fixates on it as soon as he sees it,
	//this will be detected just after the first phosphor of the second frame in the second state.  The
	//expected value from the timer would then be the time from the first phosphor of the first frame
	//in the second state to the time in the first phosphor of the second frame in the second state.  
	//Since the timer works according to the previous frame for elapsed time, the elapse time on the
	//timer will be correct.  If it worked according to the effective frame, the user would need to start
	//it on entry into the state and stop it before he even new that the monkey fixated. 
	//With all this said, it is still important to be able to read the absolute time for comparison
	//to timed signal values.  The analysis system calls setNextFrameTime with the time of the following
	//frame before property effects are implemented so that the operator can see when those changes
	//take effect.
	void setNextFrameTime(double frameTime);

	//See notes above.  Also note that in the case of the TestViewer, effectiveAbsoluteTime will
	//be estimated as 16.666ms past the previous frame.  This will
	//also occur if this function is called in an actual experiment, BUT DON'T LET ANYONE CALL IT IN AN
	//ACTUAL EXPERIMENT.
	int effectiveAbsoluteTime(TimerUnits::TimerUnits units);
	
	//Sets stored data back to its initial conditions and restarts all timers
	void resetAllTimers();

protected:
	QVector<QWeakPointer<FrameResolutionTimer>> timerList_;
private:
	QSharedPointer<FrameTracker> frameTracker_;

};

}; //namespace ControlElement
}; //namespace Picto

#endif
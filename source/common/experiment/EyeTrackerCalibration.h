#ifndef _EYETRACKERCALIBRATION_H
#define _EYETRACKERCALIBRATION_H

#include <QSharedPointer>
#include <QTime>


#include "../common.h"
#include "../compositor/CompositingSurface.h"
#include "../compositor/VisualTarget.h"
#include "../compositor/AuralTarget.h"
#include "../compositor/RenderingTarget.h"
#include "../stimuli/BoxGraphic.h"
#include "../iodevices/RewardController.h"
//#include "../engine/PictoBoxAnalogInputSignalChannel.h"


namespace Picto
{

/*! \brief Calibration tool for the eye tracker
 *
 *	The EyeTrackerCalibration object provides the framework for calibrating
 *	the eyetracker.  There are two different calibration modes provided.
 *	The automatic mode displays targets and gives rewards in a way that 
 *	automatically generates the calibration constants.  The manual mode displays
 *	targets in response to user input and allows the user to set the calibration
 *	constants by hand.
 *
 *	Additionally, there are automatic and manual test modes that allow the user to
 *	confirm that the calibration is accurate.
 *
 *	It should be noted that this object handles all of the hardware interaction, but
 *	of the user interface and GUI (apart from displaying graphics on the subject's
 *	screen).
 *
 *	WARNING: As of September 1, 2010 this code has never been tested and was all commented
 *	out.  We will want to have some sort of automated (or semi-automated) eye tracker
 *	calibration tool.  It is possible that we will want to build it as a state machine 
 *	instead of having it as a built-in Picto component.  We should also actually write the
 *	Calibration base class and use it here.
 */


#if defined WIN32 || defined WINCE
	class PICTOLIB_API EyeTrackerCalibration
#else
class EyeTrackerCalibration
#endif
{
public:
	EyeTrackerCalibration();
	~EyeTrackerCalibration();

	//void RunAutoCalibration(QSharedPointer<VisualTarget> visualTarget, 
	//						QSharedPointer<RewardController> rewardController,
	//						QSharedPointer<Picto::PictoBoxDaqBoard> daqBoard);
private:
};

};

#endif
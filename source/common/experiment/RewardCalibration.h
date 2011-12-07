
#ifndef _REWARDCALIBRATION_H
#define _REWARDCALIBRATION_H

#include <QTime>

#include "../common.h"
#include "../iodevices/RewardController.h"

namespace Picto
{

/*! \brief Calibration tool for the reward controller
 *
 *	The RewardCalibration object provides the framework for calibrating
 *	the reward controller.  The object allows for a fixed number of rewards
 *	to be given, so that the controller can be calibrated.  Additional functions
 *	are included to make applying the results of the calibration easy.
 *
 *	NOTE: We need to use the calibration base class, and this hasn't really been
 *	tested, so I have no idea if it actually works or not.
 *		--Matt Gay Sept 1, 2010
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardCalibration
#else
class RewardCalibration
#endif
{
public:
	RewardCalibration(RewardController *rewardController);
	virtual ~RewardCalibration();

	bool RunCalibration(unsigned int drops, unsigned int durationMs, unsigned int intervalMs = 250, unsigned int controllerNum=1); 
	double CalcDropVolumeUl(unsigned int drops, unsigned int totalVolumeMl);

private:
	RewardController *rewardController_;

};

}//namespace Picto

#endif
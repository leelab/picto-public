/*! \brief Calibration tool for the reward controller
 *
 *	The RewardCalibration object provides the framework for calibrating
 *	the reward controller.  The object allows for a fixed number of rewards
 *	to be given, so that the controller can be calibrated.  Additional functions
 *	are included to make applying the results of the calibration easy.
 */

#ifndef _REWARDCALIBRATION_H
#define _REWARDCALIBRATION_H

#include <QTime>

#include "../common.h"
#include "../iodevices/RewardController.h"

namespace Picto
{

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardCalibration
#else
class RewardCalibration
#endif
{
public:
	RewardCalibration(RewardController *rewardController);
	~RewardCalibration();

	bool RunCalibration(unsigned int drops, unsigned int durationMs, unsigned int intervalMs = 250, unsigned int controllerNum=1); 
	double CalcDropVolumeUl(unsigned int drops, unsigned int totalVolumeMl);

private:
	RewardController *rewardController_;

};

}//namespace Picto

#endif
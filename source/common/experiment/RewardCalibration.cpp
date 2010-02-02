#include "RewardCalibration.h"

namespace Picto
{

RewardCalibration::RewardCalibration(RewardController *rewardController)
: rewardController_(rewardController)
{
}

RewardCalibration::~RewardCalibration()
{
}

/*! \brief Runs a calibration consisting of delivering a fixed number of drops
 *
 *	Running a calibration delivers a fixed number of drops.  The experimenter then
 *	can measure the vlume of liquid produced and determine a good estimate for the 
 *	volume of each drop.
 *
 *	\param drops the number of drops to deliver in the calibration
 *	\param durationMs the duration of each drop
 *	\param intervalMs the interval between drops (in ms)
 *	\param controllerNum the controller number to perform the calibration on
 */
bool RewardCalibration::RunCalibration(unsigned int drops, 
									   unsigned int durationMs, 
									   unsigned int intervalMs, 
									   unsigned int controllerNum)
{
	//Note: using QTime is a really inaccurate way to generate a delay.
	//However, it is also the only easy way to generate a synchronous delay in 
	//Qt without using an event loop.  Since the reward calibration object
	//is used by PictoDirector (which doesn't have an event loop), and
	//we don't really care that much about the actual time, a Qtime loop
	//is the best solution.
	rewardController_->setRewardDuration(controllerNum,durationMs);
	QTime time;

	for(int i=0; i<drops; i++)
	{
		rewardController_->giveReward(controllerNum);
		time.start();
		while(time.elapsed() < intervalMs);
	}
	return true;
}

//!	\brief A convenience function for calculating the volume (is microliters) of each drop
double RewardCalibration::CalcDropVolumeUl(unsigned int drops, unsigned int totalVolumeMl)
{
	return (double)totalVolumeMl*1000.0/(double)drops;
}


} //namespace Picto
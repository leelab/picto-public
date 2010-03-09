/*! \brief This is the base class for the reward controller
 *
 *	Since there are multiple different types reward controllers, this
 *	base class exists to allow the creation of reward controllers
 *	for different platforms (and potentially different types of
 *	controllers)
 *
 *	Regardless of the control set up, all reward controllers are assumed
 *	to have the following features:
 *		- Access to one or more individual reward "channels" (numbered from 1 to N)
 *		- The ability to give rewards seperately on each channel
 *	Reward controllers may also have these features:
 *		- The ability to vary reward duration
 *		- The ability to vary reward volume
 *
 * \note reward controllers are numbered from 1 to n
 */

#ifndef _REWARDCONTROLLER_H_
#define _REWARDCONTROLLER_H_

#include <QList>
#include <QObject>

#include "../common.h"

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardController : public QObject
#else
class RewardController : public QObject
#endif
{
	Q_OBJECT
public:
	RewardController(unsigned int channelCount);
	~RewardController();

	virtual bool setRewardVolume(unsigned int channel, float volume) =0 ;
	virtual bool setRewardDurationMs(unsigned int channel, unsigned int duration) = 0;
	virtual int getRewardDurationMs(unsigned int channel) = 0;
	virtual bool setRewardResetTimeMs(unsigned int channel, unsigned int time) = 0;

	int getChannelCount() { return channelCount_; };

public slots:
	virtual void giveReward(unsigned int channel) = 0;
	virtual void flush(unsigned int channel,bool flush) = 0;

protected:
	int channelCount_;
	QList<float> rewardVolumes_;
	QList<int> rewardDurations_;
	QList<int> rewardResetTimes_;
};

} //namespace Picto

#endif
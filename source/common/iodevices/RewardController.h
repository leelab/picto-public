#ifndef _REWARDCONTROLLER_H_
#define _REWARDCONTROLLER_H_

#include <QList>
#include <QObject>
#include <QMutex>
#include <QReadWriteLock>
#include "../storage/RewardDataUnit.h"

#include "../common.h"

namespace Picto {

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
 *	It should be noted that hardware specific reward controllers won't be located
 *	in the common/iodevices folder, but will rather be in the folders of the source
 *	that actually uses them (e.g. Director).
 *
 * \note reward controllers are numbered from 1 to n
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardController : public QObject
#else
class RewardController : public QObject
#endif
{
	Q_OBJECT
public:
	RewardController(unsigned int channelCount);
	virtual ~RewardController();

	bool setRewardVolume(unsigned int channel, float volume);
	bool setRewardResetTimeMs(unsigned int channel, unsigned int time);

	int getChannelCount() { return channelCount_; };
	QList<QSharedPointer<RewardDataUnit>> getDeliveredRewards();


public slots:
	virtual void giveReward(unsigned int channel,int quantity,bool appendToList=false);
	virtual void flush(unsigned int channel,bool flush) = 0;

protected:
	virtual void doReward(unsigned int channel,int quantity) = 0;
	int channelCount_;
	QList<float> rewardVolumes_;
	QList<int> rewardResetTimes_;
private:
	void appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit);
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
	QMutex giveRewardMutex_;
	QReadWriteLock listLock_;

};

} //namespace Picto

#endif
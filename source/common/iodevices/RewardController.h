#ifndef _REWARDCONTROLLER_H_
#define _REWARDCONTROLLER_H_

#include <QList>
#include <QObject>
#include <QMutex>
#include <QReadWriteLock>
#include <QHash>
#include <QLinkedList>
#include "../storage/RewardDataUnit.h"
#include "../timing/Timestamper.h"
#include "../timing/Stopwatch.h"

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

	int getChannelCount() { return channelCount_; };
	QList<QSharedPointer<RewardDataUnit>> getDeliveredRewards();


public slots:
	// \brief Appends a reward to the list of startable rewards.
	void addReward(unsigned int channel,int quantity, int minRewardPeriod);
	// \brief Triggers any pending rewards to be given if possible.
	void triggerRewards(bool appendToList);
	// \brief Stops all currently active and pending rewards on the input channel
	void stopRewards(unsigned int channel);
	// \brief returns true if there are still rewards to send.
	bool hasPendingRewards();
	// \brief returns true if there are still rewards to send on the input channel.
	bool hasPendingRewards(unsigned int channel);
	virtual void flush(unsigned int channel,bool flush) = 0;

protected:
	// \brief Starts a reward without blocking during reward supply.
	// Reward time is marked just after this is called, so actual reward should be given as close as possible
	// to the end of the function.
	virtual void startReward(unsigned int channel,int quantity) = 0;
	// \brief Stops a reward  that is currently being supplied.
	// Note that when a reward is stopped early, this fact is not recorded
	// in the session database.
	virtual void stopReward(unsigned int channel) = 0;
	// \brief Returns true if the latest reward is no longer being supplied.  
	// startReward won't be called until this returns true.
	virtual bool rewardWasSupplied(unsigned int channel) = 0;
	int channelCount_;
	QList<float> rewardVolumes_;
private:
	struct RewardUnit
	{
		RewardUnit(int q, int m){quantity = q;minRewardPeriod = m;};
		int quantity;
		int minRewardPeriod;
	};
	struct RewardChannel
	{
		RewardChannel():lastRewardPeriod(0){};
		Stopwatch stopwatch;
		int lastRewardPeriod;
		QLinkedList<RewardUnit> pendingRewards;
	};
	QHash<int,RewardChannel> rewardChannels_;
	void appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit);
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
	Timestamper stamper_;


};

} //namespace Picto

#endif
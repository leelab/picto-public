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

/*! \brief This is the base class for classes that provide rewards to the subject of a Picto experiment.
 *
 *	Since there are multiple different types reward controllers, this
 *	base class exists to allow the creation of reward controllers
 *	for different platforms (and different types of controllers)
 *
 *	All reward controllers are assumed to have the following features:
 *		- Access to one or more individual reward "channels" (numbered from 1 to N)
 *		- The ability to give rewards seperately on each channel
 *	Reward controllers may also have the ability to vary reward duration.  If they do not have that
 *	ability, the reward duration resolution will be defined by the frequency of calls to triggerRewards().
 *	Since triggerRewards() is currently called once per frame by the Engine::PictoEngine, this means that
 *	reward controllers that don't have the ability to define their duration will have resolution of about
 *	16ms.
 *
 *	Hardware specific reward controllers won't be located
 *	in the common/iodevices folder, but will rather be in the folders of the source
 *	that actually uses them (e.g. Director).
 *
 *	\note reward controllers are numbered from 1 to n
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	void discardOverlapingRewards(bool doIt);

	int getChannelCount() { return channelCount_; };
	QList<QSharedPointer<RewardDataUnit>> getDeliveredRewards();


public slots:
	void addReward(unsigned int channel,int quantity, int minRewardPeriod);
	void triggerRewards(bool appendToList);
	bool rewardInProgress(unsigned int channel);
	bool hasPendingRewards();
	bool hasPendingRewards(unsigned int channel);
	void flush(unsigned int channel, int seconds);
	bool isFlushing(unsigned int channel);
	void abortFlush(unsigned int channel);

protected:
	/*!	\brief Starts a reward without blocking during reward supply.
	 *	\details Reward time is marked just after this is called, so actual reward should be given as close as possible
	 *	to the end of the function.
	 */
	virtual void startReward(unsigned int channel,int quantity) = 0;
	/*!	\brief Returns true if the latest reward is no longer being supplied on the input channel.  
	 *	\details startReward() won't be called until this returns true.
	 */
	virtual bool rewardWasSupplied(unsigned int channel) = 0;
	/*! \brief Starts a flush on the input channel.
	 *	\details The flush should continue until stopFlush() is called.
	 */
	virtual void startFlush(unsigned int channel) = 0;
	/*!	\brief Stops a flush on the input channel.
	 *	\details Stops the latest flush that was started with startFlush().
	 */
	virtual void stopFlush(unsigned int channel) = 0;
	int channelCount_;
	QList<float> rewardVolumes_;
private:
	/*! \brief Holds data about a particular reward to be supplied.
	 *	\details 
	 *	- quantity is the time length of the reward in ms.
	 *	- minRewardPeriod is the minimum time between rewards in ms.
	 *	- isFlush indicates if this reward is actually a flush.
	 */
	struct RewardUnit
	{
		RewardUnit(int q, int m, bool f=false){quantity = q;minRewardPeriod = m;isFlush = f;};
		int quantity;
		int minRewardPeriod;
		bool isFlush;
	};
	/*! \brief Contains data about a give Reward channel.
	 *	\details
	 *	- stopwatch is a timer used to detect how long ago the reward was triggered.
	 *	- lastRewardPeriod is the last set minimum time between rewards in ms.
	 *	- inFlush indicates if a flush is currently occuring on this channel.
	 *	- pendingRewards is a list of RewardUnit objects to be supplied on this channel.
	 */
	struct RewardChannel
	{
		RewardChannel():lastRewardPeriod(0),inFlush(0){};
		Stopwatch stopwatch;
		int lastRewardPeriod;
		bool inFlush;
		QLinkedList<RewardUnit> pendingRewards;
	};
	QHash<int,RewardChannel> rewardChannels_;
	void appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit);
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
	Timestamper stamper_;
	bool discardOverlap_;
};

} //namespace Picto

#endif
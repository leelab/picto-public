#ifndef _REWARDSTATE_H_
#define _REWARDSTATE_H_
#include "EventState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackRewardData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class RewardState : public RewardReader, public EventState<double>
{
	Q_OBJECT
public:
	bool setReward(double time,qulonglong dataId,int duration,int channel);
protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(double index);
	virtual void requestMoreDataByTime(double time);
signals:
	void rewardSupplied(int duration,int channel);
};

struct PlaybackRewardData
{
	PlaybackRewardData(){};
	PlaybackRewardData(int duration,int channel){duration_ = duration;channel_ = channel;};
	int duration_;
	int channel_;
};

}; //namespace Picto
#endif
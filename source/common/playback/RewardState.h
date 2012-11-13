#ifndef _REWARDSTATE_H_
#define _REWARDSTATE_H_
#include "EventState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackRewardData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class RewardState : public RewardReader, public EventState
{
	Q_OBJECT
public:
	bool setReward(double time,qulonglong dataId,int duration,int channel);
signals:
	void rewardSupplied(int duration,int channel);
	void needsData(PlaybackIndex currLast,PlaybackIndex to);
	void needsNextData(PlaybackIndex currLast,bool backward);

protected:
	virtual void triggerValueChange(bool reverse,bool last);
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
	virtual void requestNextData(PlaybackIndex currLast,bool backward);

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
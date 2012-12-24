#ifndef _REWARDSTATE_H_
#define _REWARDSTATE_H_
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackRewardData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class RewardState : public RewardReader, public DataState
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);
signals:
	void rewardSupplied(double time,int duration,int channel);
private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QVector<PlaybackRewardData> data_;

//public:
//	bool setReward(double time,qulonglong dataId,int duration,int channel);
//signals:
//	void rewardSupplied(double time,int duration,int channel);
//	void needsData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsNextData(PlaybackIndex currLast,bool backward);
//
//protected:
//	virtual void triggerValueChange(bool reverse,bool last);
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to);
//	virtual void requestNextData(PlaybackIndex currLast,bool backward);

};

struct PlaybackRewardData
{
	PlaybackRewardData(){};
	PlaybackRewardData(PlaybackIndex index,int duration,int channel){index_ = index;duration_ = duration;channel_ = channel;};
	PlaybackIndex index_;
	int duration_;
	int channel_;
};

}; //namespace Picto
#endif
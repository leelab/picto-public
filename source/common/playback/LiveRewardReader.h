#ifndef _LiveRewardReader_H_
#define _LiveRewardReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveRewardReader : public RewardReader
#else
class LiveRewardReader : public RewardReader
#endif
{
	Q_OBJECT
public:
	LiveRewardReader(){};
	void setRunStart();
	void setLatestRewardData(double time,double duration);
	void setRunEnd();

	//Frame Reader Interface
	virtual double getLatestTime();
	virtual double getNextTime();
	virtual int getLatestDuration();
	virtual int getNextDuration();
	//Returns a list of reward times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time);
	//Returns a list of rewards with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time);
	//Returns a list of reward durations from rewards that occured with times > the input time and <= the current time.  
	virtual QVariantList getDurationsSince(double time);
	//Returns a list of reward durations from rewards with times > the current time and <= the input time.  
	virtual QVariantList getDurationsUntil(double time);

private:

	struct RewardData
	{
		RewardData(){time_ = 0;duration_ = 0;};
		RewardData(double time,double duration){time_ = time;duration_ = duration;};
		inline bool operator<(const RewardData& someData) const {
			return time_ < someData.time_;
		}
		double time_;
		double duration_;
	};

	QVector<RewardData> rewardData_;
};


}; //namespace Picto
#endif
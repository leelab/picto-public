#ifndef _REWARDSTATE_H_
#define _REWARDSTATE_H_
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackRewardData;
/*! \brief Implements the DataState and RewardReader classes to load a Picto Session database, 
 *	extract the data for reward deliveries and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PlaybackRewardData objects is loaded from the 
 *	session data.  Each PlaybackRewardData represents a single reward delivery and when moveToIndex() 
 *	is called, we just traverse through the list until we reach a PlaybackRewardData object with the appropriate PlaybackIndex.
 *	Each time moveToIndex() causes us to pass through a PlaybackRewardData entry, the rewardSupplied() 
 *	signal is called, which tells the rest of the playback system that a Reward was delivered.
 *
 *	\note Since the functions here simply implement the RewardReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding function level documentation
 *	for this class.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	//Reward Reader Interface
	virtual double getLatestTime();
	virtual double getNextTime();
	virtual int getLatestDuration();
	virtual int getNextDuration();
	//Returns a list of frame times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time);
	//Returns a list of values with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time);
	//Returns a list of reward durations from rewards that occured with times > the input time and <= the current time.  
	virtual QVariantList getDurationsSince(double time);
	//Returns a list of reward durations from rewards with times > the current time and <= the input time.  
	virtual QVariantList getDurationsUntil(double time);

signals:
	/*! \brief Emitted when moveToIndex() moves the current time passed a new reward value.  time is the
	 *	time that the reward was supplied, duration is its duraiton in milliseconds, channel is the reward channel
	 *	on which the reward was supplied.
	 */
	void rewardSupplied(double time,int duration,int channel);
private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QList<PlaybackRewardData> data_;

};

/*! \brief A struct used to store a single Reward delivery event.
 *	\details Includes the PlaybackIndex of the Reward delivery along with its duration in milliseconds and the channel on which
 *	the delivery occured.
 */
struct PlaybackRewardData
{
	PlaybackRewardData(){};
	PlaybackRewardData(PlaybackIndex index,int duration,int channel){index_ = index;duration_ = duration;channel_ = channel;};
	/*! \brief One PlaybackRewardData is lower than another if its PlaybackIndex is lower.*/
	inline bool operator<(const PlaybackRewardData& someData) const {
		return index_ < someData.index_;
	}
	PlaybackIndex index_;
	int duration_;
	int channel_;
};

}; //namespace Picto
#endif
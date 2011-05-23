#ifndef _REWARD_DATA_STORE_H_
#define _REWARD_DATA_STORE_H_

#include "../common.h"
#include "DataStore.h"

namespace Picto {

/*!	\brief Stores reward data 
 *
 *	The reward data store is designed to store information about rewards being given
 *	Each one stores information about a single reward.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardDataStore : public DataStore
#else
class RewardDataStore : public DataStore
#endif
{
	Q_OBJECT
public:

	RewardDataStore();
	RewardDataStore(int durationMs, int channel, double time);

	void setDuration(int durationMs) { durationMs_ = durationMs; };
	void setChannel(int channel) {channel_ = channel; };
	void setTime(double timestamp) {time_ = timestamp; };

	int getDuration() { return durationMs_; };
	int getChannel() { return channel_; };
	double getTime() { return time_; };

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int durationMs_;
	int channel_;
	double time_;
};


}; //namespace Picto

#endif

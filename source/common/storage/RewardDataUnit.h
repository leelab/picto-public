#ifndef _REWARDDATAUNIT_H_
#define _REWARDDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores reward data 
 *
 *	The reward data store is designed to store information about rewards being given
 *	Each one stores information about a single reward.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardDataUnit : public DataUnit
#else
class RewardDataUnit : public DataUnit
#endif
{
public:

	RewardDataUnit();
	RewardDataUnit(int durationMs, int channel, double time);

	void setDuration(int durationMs) { durationMs_ = durationMs; };
	void setChannel(int channel) {channel_ = channel; };
	void setTime(double timestamp) {time_ = QString("%1").arg(timestamp,0,'f',6);};

	int getDuration() { return durationMs_; };
	int getChannel() { return channel_; };
	QString getTime() { return time_; };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int durationMs_;
	int channel_;
	QString time_;
};


}; //namespace Picto

#endif

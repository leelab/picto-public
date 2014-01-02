#ifndef _REWARDDATAUNIT_H_
#define _REWARDDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Stores information about a single reward delivery.
 *
 *	\details This stores the time at which the reward was delivered.  The duration of the reward in
 *	milliseconds, and the reward channel on which the reward was supplied.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	/*! \brief Sets the duration of the reward delivery in milliseconds.*/
	void setDuration(int durationMs) { durationMs_ = durationMs; };
	/*! \brief Sets the reward channel on which the reward was supplied.*/
	void setChannel(int channel) {channel_ = channel; };
	/*! \brief Sets the time at which reward delivery began.*/
	void setTime(double timestamp) {time_ = QString("%1").arg(timestamp,0,'f',14);};

	/*! \brief Returns the duration of the reward delivery in milliseconds.*/
	int getDuration() { return durationMs_; };
	/*! \brief Returns the reward channel on which the reward was supplied.*/
	int getChannel() { return channel_; };
	/*! \brief Returns the time at which reward delivery began.*/
	QString getTime() { return time_; };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int durationMs_;
	int channel_;
	QString time_;
};


}; //namespace Picto

#endif

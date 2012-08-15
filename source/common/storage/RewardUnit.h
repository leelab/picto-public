#ifndef _REWARDUNIT_H_
#define _REWARDUNIT_H_

#include "../../common/storage/DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores information about a picto director instance
 *
 *	
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RewardUnit : public DataUnit
#else
class RewardUnit : public DataUnit
#endif
{
public:
	RewardUnit();
	RewardUnit(int channel,int duration);

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setDuration(int duration);
	int getDuration();
	int getChannel();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int channel_;
	int duration_;
};


}; //namespace Picto

#endif

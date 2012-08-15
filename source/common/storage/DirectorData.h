#ifndef _DIRECTORDATA_H_
#define _DIRECTORDATA_H_

#include <QHash>

#include "../../common/storage/DataUnit.h"
#include "rewardunit.h"

namespace Picto {

/*!	\brief Stores information about a picto director instance
 *
 *	
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API DirectorData : public DataUnit
#else
	class DirectorData : public DataUnit
#endif
{
public:
	DirectorData();

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setReward(int channel, int duration);
	int getRewardDuration(int channel);
	QHash<int,int> getRewardMap();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QHash<int,QSharedPointer<RewardUnit>> rewardMap_;
};


}; //namespace Picto

#endif

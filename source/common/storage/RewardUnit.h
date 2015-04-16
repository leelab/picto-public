#ifndef _REWARDUNIT_H_
#define _REWARDUNIT_H_

#include "../../common/storage/DataUnit.h"

#include <QString>

namespace Picto {

/*!	\brief Stores information about the default reward delivery time on a particular channel when a simple reward button press occurs.
 *
 *	\details This stores the default duration of a reward on the stored channel.  This is the quantity supplied when the reward button
 *	is pressed on the Pictobox or the reward button on the workstation is pressed.  These systems are synchronized by using this DataType
 *	such that they always include the same reward quantity.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

private:
	int channel_;
	int duration_;
};


}; //namespace Picto

#endif

#ifndef _FPGETCOMMANDHANDLER_H_
#define _FPGETCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief A Picto::ProtocolCommandHandler that Handles FPGET commands received by the Director from the EmbeddedFrontPanel.
 *	\details Returns the requested data (IPAddress, DirectorName, DirectorStatus, RewardDuration, FlushDuration).  
 *
 *	Rather than creating some kind of complex circular reference system, we have simply exposed functions here that higher level 
 *	classes can use to update this ProtocolCommandHandler with the data that it needs.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct FPGETCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPGETCommandHandler();

	QString method() { return QString("FPGET"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

	/*! \brief Use this function to tell this object what the Director's IP Address is so that it can be correctly returned to
	 *	anyone requesting that data.
	 */
	void setIpAddress(QString val){ip_ = val;};
	/*! \brief Use this function to tell this object what the Director's name is so that it can be correctly returned to
	 *	anyone requesting that data.
	 */
	void setName(QString val){name_ = val;};
	void setRewardDuration(int controller,int dur);
	void setFlushDuration(int controller,int dur);
	void setStatus(QString status){status_ = status;};

private:
	int getRewardDuration(int controller);
	int getFlushDuration(int controller);
	QString ip_;
	QString name_;
	QVector<int> rewardDurs_;
	QVector<int> flushDurs_;
	QString status_;
};

/*! @} */

#endif
#ifndef _FPGETCOMMANDHANDLER_H_
#define _FPGETCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPGETCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPGETCommandHandler();

	QString method() { return QString("FPGET"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

	void setIpAddress(QString val){ip_ = val;};
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
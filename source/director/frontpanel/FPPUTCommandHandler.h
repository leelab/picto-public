#ifndef _FPPUTCOMMANDHANDLER_H_
#define _FPPUTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPPUTCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPPUTCommandHandler();

	QString method() { return QString("FPPUT"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void setName(QString name);
	void setRewardDuration(int controller,int duration);
	void setFlushDuration(int controller,int duration);
};

/*! @} */

#endif
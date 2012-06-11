#ifndef _FPREWARDCOMMANDHANDLER_H_
#define _FPREWARDCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPREWARDCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPREWARDCommandHandler();

	QString method() { return QString("FPREWARD"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void giveReward(int controller);
};

/*! @} */

#endif
#ifndef _FPSTOPFLUSHCOMMANDHANDLER_H_
#define _FPSTOPFLUSHCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPSTOPFLUSHCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPSTOPFLUSHCommandHandler();

	QString method() { return QString("FPSTOPFLUSH"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void stopFlush(int controller);

};

/*! @} */

#endif
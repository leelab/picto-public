#ifndef _FPFLUSHCOMMANDHANDLER_H_
#define _FPFLUSHCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPFLUSHCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPFLUSHCommandHandler();

	QString method() { return QString("FPFLUSH"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void flush(int controller);
};

/*! @} */

#endif
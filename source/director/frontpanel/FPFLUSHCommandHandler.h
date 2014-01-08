#ifndef _FPFLUSHCOMMANDHANDLER_H_
#define _FPFLUSHCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief A Picto::ProtocolCommandHandler that Handles FPFLUSH commands received by the Director from the EmbeddedFrontPanel.
 *	\details Emits a flush() signal when a flush is requested.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
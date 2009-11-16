#ifndef _FPSTOPFLUSHCOMMANDHANDLER_H_
#define _FPSTOPFLUSHCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"
#include "../engine.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPSTOPFLUSHCommandHandler : Picto::ProtocolCommandHandler
{
public:
	FPSTOPFLUSHCommandHandler();

	QString method() { return QString("FPSTOPFLUSH"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

	void setEngine(Engine *engine) { eng = engine; }

private:
	Engine *eng;
};

/*! @} */

#endif
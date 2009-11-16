#ifndef _FPPUTCOMMANDHANDLER_H_
#define _FPPUTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"
#include "../engine.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct FPPUTCommandHandler : Picto::ProtocolCommandHandler
{
public:
	FPPUTCommandHandler();

	QString method() { return QString("FPPUT"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

	void setEngine(Engine *engine) { eng = engine; }

private:
	Engine *eng;
};

/*! @} */

#endif
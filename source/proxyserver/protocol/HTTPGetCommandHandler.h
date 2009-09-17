/*! \file HTTPGetCommandHandler.h
 * \ingroup pictoproxyserver_protocol
 * \brief
 */

#ifndef _HTTPGETCOMMANDHANDLER_H_
#define _HTTPGETCOMMANDHANDLER_H_

#include <QObject>

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \addtogroup pictoproxyserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
 */
struct HTTPGetCommandHandler : Picto::ProtocolCommandHandler
{
public:
	HTTPGetCommandHandler(QObject *acqPlugin);

	QString method() { return QString("GET"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
private:
	QObject *acgPlugin;
};

/*! @} */

#endif

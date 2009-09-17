/*! \file ACQGetCommandHandler.h
 * \ingroup pictoproxyserver_protocol
 * \brief
 */

#ifndef _ACQGETCOMMANDHANDLER_H_
#define _ACQGETCOMMANDHANDLER_H_

#include <QObject>

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

#include "interfaces.h"

/*! \addtogroup pictoproxyserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the ACQ protocol
 */
struct ACQGetCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ACQGetCommandHandler(QObject *acqPlugin);

	QString method() { return QString("GET"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
private:
	NeuralDataAcqInterface *iNDAcq;
};

/*! @} */

#endif

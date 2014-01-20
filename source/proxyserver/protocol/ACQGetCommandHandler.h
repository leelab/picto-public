#ifndef _ACQGETCOMMANDHANDLER_H_
#define _ACQGETCOMMANDHANDLER_H_

#include <QObject>

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

#include "NeuralDataAcqInterface.h"

/*! \brief Handles GET commands received specifying use of the ACQ protocol
 *	
 *	A GET command is asking for a piece of neural data from the proxy server.  The data is
 *	returned as an XML fragment.  The fragment is documented in the TRAC wiki.
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

#endif

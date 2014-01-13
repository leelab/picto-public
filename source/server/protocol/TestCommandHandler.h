#ifndef _TESTCOMMANDHANDLER_H_
#define _TESTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*!	\brief A command handler used when a component just wants to check on the server.
 *
 *	Components can send simple messages to the server using TEST anything PICTO/1.0.
 *	The server will always respond with an OK message.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct TestCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TestCommandHandler();

	QString method() { return QString("TEST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};

#endif

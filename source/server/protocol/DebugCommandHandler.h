#ifndef _DEBUGCOMMANDHANDLER_H_
#define _DEBUGCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct DebugCommandHandler : Picto::ProtocolCommandHandler
{
public:
	DebugCommandHandler();

	QString method() { return QString("DEBUG"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};

#endif

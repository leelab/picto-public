#ifndef _TERMINATECOMMANDHANDLER_H_
#define _TERMINATECOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API TerminateCommandHandler : ProtocolCommandHandler
#else
struct TerminateCommandHandler : ProtocolCommandHandler
#endif
{
public:
	TerminateCommandHandler();

	QString method() { return QString("TERMINATE"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

}; //namespace Picto

#endif

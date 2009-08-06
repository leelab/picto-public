#ifndef _GETCOMMANDHANDLER_H_
#define _GETCOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API GetCommandHandler : ProtocolCommandHandler
#else
struct GetCommandHandler : ProtocolCommandHandler
#endif
{
public:
	GetCommandHandler();

	QString method() { return QString("GET"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

}; //namespace Picto

#endif

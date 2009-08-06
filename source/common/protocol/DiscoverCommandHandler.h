#ifndef _DISCOVERCOMMANDHANDLER_H_
#define _DISCOVERCOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API DiscoverCommandHandler : ProtocolCommandHandler
#else
struct DiscoverCommandHandler : ProtocolCommandHandler
#endif
{
public:
	DiscoverCommandHandler();

	QString method() { return QString("DISCOVER"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};


}; //namespace Picto

#endif

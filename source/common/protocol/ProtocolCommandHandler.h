#ifndef _PROTOCOLCOMMANDHANDLER_H_
#define _PROTOCOLCOMMANDHANDLER_H_

#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "ProtocolResponse.h"
#include "ProtocolCommand.h"

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolCommandHandler
#else
struct ProtocolCommandHandler
#endif
{
public:
	ProtocolCommandHandler();

	virtual QString method() = 0;
	virtual QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>) = 0;
};

}; //namespace Picto

#endif

#ifndef _PROTOCOLCOMMANDHANDLER_H_
#define _PROTOCOLCOMMANDHANDLER_H_

#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "ProtocolResponse.h"
#include "ProtocolCommand.h"

namespace Picto {

/*! \brief A base class from which all command handlers are derived
 *
 *	A command handler needs to implement two functions.  The method function is 
 *	simply the name of the commang (e.g. GET).  The processCommand function is where
 *	all of the logic is.  The processCommand function should return a response, and
 *	cause any necessary internal state changes.  
 */
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

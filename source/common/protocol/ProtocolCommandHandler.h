#ifndef _PROTOCOLCOMMANDHANDLER_H_
#define _PROTOCOLCOMMANDHANDLER_H_

#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "ProtocolResponse.h"
#include "ProtocolCommand.h"

namespace Picto {

/*! \brief A base class from which all command handlers are derived.  These command handlers define
 *	responses to ProtocolCommand objects coming in over the network.
 *
 *	A command handler needs to implement two functions.  The method function is 
 *	simply the name of the command (e.g. GET).  The processCommand function is where
 *	all of the logic is.  The processCommand function should return a response, and
 *	cause any necessary internal state changes.  
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolCommandHandler : public QObject
#else
struct ProtocolCommandHandler : public QObject
#endif
{
	Q_OBJECT
public:
	ProtocolCommandHandler();

	/*! \brief Returns the name of the commeand that this ProtocolCommandHandler handles.*/
	virtual QString method() = 0;
	/*! \brief Processes the input ProtocolCommand, triggering any necessary logic based on the command contents, 
	 *	and returns an appropriate ProtocolResponse.
	 */
	virtual QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>) = 0;

};

}; //namespace Picto

#endif

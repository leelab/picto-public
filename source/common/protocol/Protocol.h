#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <map>

#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief Defines a protocol (which is really nothing more than a set of command handlers)
 *
 *	A protocol is really just a collection of commands that are "legal".  This
 *	object is a base class that makes it easy to define a custom protocol.  Each
 *	derived protocol needs to have a name and version, as well as a list of command
 *	handlers that get added to the protocol.
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API Protocol
#else
struct Protocol
#endif
{
public:
	Protocol();
	
	QSharedPointer<ProtocolCommandHandler> getCommandHandler(QString _command);

	virtual QString id() = 0;
	virtual QString version() = 0;

private:
	std::map<QString,QSharedPointer<ProtocolCommandHandler> > commandHandlers_;

protected:
	void addCommandHandler(QSharedPointer<ProtocolCommandHandler> _commandHandler);
};

}; //namespace Picto

#endif

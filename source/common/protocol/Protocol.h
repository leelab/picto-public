#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <map>

#include <QString>
#include <QSharedPointer>

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

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

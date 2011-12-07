#include <QSharedPointer>
#include "Protocol.h"

#include "../common.h"

#include "AnnounceCommandHandler.h"
#include "GetCommandHandler.h"
#include "../memleakdetect.h"

namespace Picto {

Protocol::Protocol()
{
}

void Protocol::addCommandHandler(QSharedPointer<ProtocolCommandHandler> _commandHandler)
{
	commandHandlers_[_commandHandler->method()] = _commandHandler;
}

QSharedPointer<ProtocolCommandHandler> Protocol::getCommandHandler(QString _command)
{
	QSharedPointer<ProtocolCommandHandler> commandHandler;

	std::map<QString,QSharedPointer<ProtocolCommandHandler> >::const_iterator commandHandlerIter = commandHandlers_.find(_command);

	if(commandHandlerIter != commandHandlers_.end())
	{
		commandHandler = commandHandlerIter->second;
	}

	return commandHandler;
}


}; //namespace Picto

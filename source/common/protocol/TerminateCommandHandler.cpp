#include "TerminateCommandHandler.h"

namespace Picto {

TerminateCommandHandler::TerminateCommandHandler()
{
}

QSharedPointer<ProtocolResponse> TerminateCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	if(command->getTarget() == "CONNECTION")
	{
		QSharedPointer<ProtocolResponse> response(new ProtocolResponse("PICTO","1.0",ProtocolResponseType::OK));
		response->setShouldTerminateConnection(true);
		return response;
	}
	else
	{
		QSharedPointer<ProtocolResponse> response(new ProtocolResponse("PICTO","1.0",ProtocolResponseType::NotFound));
		return response;
	}
}

}; //namespace Picto

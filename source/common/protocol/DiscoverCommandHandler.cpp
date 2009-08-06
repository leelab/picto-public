#include "DiscoverCommandHandler.h"

namespace Picto {

DiscoverCommandHandler::DiscoverCommandHandler()
{
}

QSharedPointer<ProtocolResponse> DiscoverCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	/*! \todo implement handler */
	QSharedPointer<ProtocolResponse> response(new ProtocolResponse("PICTO","1.0",ProtocolResponseType::NotImplemented));

	return response;
}

}; //namespace Picto

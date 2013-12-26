#include "DiscoverCommandHandler.h"
#include "../common/globals.h"
#include "../memleakdetect.h"

namespace Picto {

DiscoverCommandHandler::DiscoverCommandHandler()
{
}

/*! \brief Implements ProtocolCommandHandler::processCommand() to return a ProtocolResponseType::NotImplemented ProtocolResponse.
 */
QSharedPointer<ProtocolResponse> DiscoverCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	/*! \todo implement handler */
	QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",ProtocolResponseType::NotImplemented));

	return response;
}

}; //namespace Picto

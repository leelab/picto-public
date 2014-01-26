#include "AnnounceCommandHandler.h"
#include "../common/globals.h"
#include "../memleakdetect.h"

namespace Picto {

AnnounceCommandHandler::AnnounceCommandHandler()
{
}

/*! \brief Implements ProtocolCommandHandler::processCommand() to return a ProtocolResponseType::NotImplemented ProtocolResponse.
 */
QSharedPointer<ProtocolResponse> AnnounceCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",ProtocolResponseType::NotImplemented));

	return response;
}

}; //namespace Picto

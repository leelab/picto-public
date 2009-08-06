/*! \file AnnounceCommandHandler.cpp
 * \ingroup picto_protocol
 * \brief
 */

#include "AnnounceCommandHandler.h"

namespace Picto {

AnnounceCommandHandler::AnnounceCommandHandler()
{
}

QSharedPointer<ProtocolResponse> AnnounceCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	/*! \todo implement handler */
	QSharedPointer<ProtocolResponse> response(new ProtocolResponse("PICTO","1.0",ProtocolResponseType::NotImplemented));

	return response;
}

}; //namespace Picto

#include "DirectorListCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

DirectorListCommandHandler::DirectorListCommandHandler()
{
}

/*! \brief handles a DIRECTORLIST command
 *
 */
QSharedPointer<Picto::ProtocolResponse> DirectorListCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	if(command->getTarget() != "/")
		return notFoundResponse;

	ConnectionManager *conMgr = ConnectionManager::Instance();
	QString directorsXML = conMgr->getDirectorList();

	response->setContent(directorsXML.toUtf8());
	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	return response;
}

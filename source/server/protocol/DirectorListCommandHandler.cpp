#include "DirectorListCommandHandler.h"

#include "../../common/globals.h"
#include "../session/SessionManager.h"
#include "../session/SessionInfo.h"
#include "../network/DirectorList.h"


#include <QXmlStreamReader>
#include <QFile>

DirectorListCommandHandler::DirectorListCommandHandler()
{
}

/*! \brief handles a DIRECTORLIST command
 *
 */
QSharedPointer<Picto::ProtocolResponse> DirectorListCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	if(command->getTarget() != "/")
		return notFoundResponse;

	DirectorList directorList;
	QString directorsXML = directorList.getList();

	//Get the session ID from the command
	QUuid sessionID = QUuid(command->getFieldValue("Session-ID"));

	response->setContent(directorsXML.toUtf8());
	return response;
}

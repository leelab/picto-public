#include "ProxyListCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

ProxyListCommandHandler::ProxyListCommandHandler()
{
}

/*! \brief handles a PROXYLIST command
 *
 */
QSharedPointer<Picto::ProtocolResponse> ProxyListCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	if(command->getTarget() != "/")
		return notFoundResponse;

	ConnectionManager *conMgr = ConnectionManager::Instance();
	QString proxiesXML = conMgr->getProxyList();

	response->setContent(proxiesXML.toUtf8());
	return response;
}

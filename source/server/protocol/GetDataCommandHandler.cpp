#include "GetDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"


#include <QXmlStreamWriter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>

GetDataCommandHandler::GetDataCommandHandler()
{
}

/*! \brief handles a GETDATA command
 *
 *	The data is stored in a database table appropraite to the type of data,
 *	and a response is returned indicating that the data was received.
 */
QSharedPointer<Picto::ProtocolResponse> GetDataCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("GETDATA  handler: %d %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = ConnectionManager::Instance()->getSessionInfo(QUuid(command->getFieldValue("Session-ID")));
	
	if(sessionInfo.isNull())
		return notFoundResponse;

	//NOTE: If the target isn't a double, we'll get 0.0 here, which is fine
	double timestamp = command->getTarget().toDouble();

	QByteArray xmlContent;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&xmlContent));
	xmlWriter->writeStartElement("Data");

	Picto::BehavioralDataStore dataStore;
	dataStore = sessionInfo->selectBehavioralData(timestamp);
	dataStore.serializeAsXml(xmlWriter);

	//! \TODO add state transistions here...

	xmlWriter->writeEndElement();

	response->setContent(xmlContent);

	return response;

}


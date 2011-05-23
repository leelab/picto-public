#include "GetDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../../common/storage/StateDataStore.h"
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
	//printf("GETDATA  handler: %d %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = ConnectionManager::Instance()->getSessionInfo(QUuid(command->getFieldValue("Session-ID")));
	
	if(sessionInfo.isNull())
		return notFoundResponse;

	double timestamp = command->getTarget().split(':').value(1).toDouble();
	QString dataType = command->getTarget().split(':').value(0,"");

	QByteArray xmlContent;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&xmlContent));
	xmlWriter->writeStartElement("Data");

	if(dataType.compare("BehavioralDataStore",Qt::CaseInsensitive) == 0)
	{
		QSharedPointer<Picto::BehavioralDataStore> dataStore;
		dataStore = sessionInfo->selectBehavioralData(timestamp);
		dataStore->toXml(xmlWriter);
	}
	else if(dataType.compare("StateDataStore",Qt::CaseInsensitive) == 0)
	{
		QSharedPointer<QList<QSharedPointer<Picto::StateDataStore>>> dataStores;
		dataStores = sessionInfo->selectStateData(timestamp);
		foreach(QSharedPointer<Picto::StateDataStore> data, *dataStores)
		{
			data->toXml(xmlWriter);
		}
	}
	else if(dataType.compare("FrameDataStore",Qt::CaseInsensitive) == 0)
	{
		QSharedPointer<Picto::FrameDataStore> dataStore;
		dataStore = sessionInfo->selectFrameData(timestamp);
		dataStore->toXml(xmlWriter);
	}
	else
	{
		QString msg = "Unrecognized data type: " + dataType;
		Q_ASSERT_X(false, "GetDataCommandHandler", msg.toAscii());
	}


	//! \TODO add state transistions here...

	xmlWriter->writeEndElement();

	response->setContent(xmlContent);

	return response;

}


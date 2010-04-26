#include "DirectorDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"


#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>

DirectorDataCommandHandler::DirectorDataCommandHandler()
{
}

/*! \brief handles a DIRECTORDATA command
 *
 *	The data is stored in a database table appropraite to the type of data,
 *	and a response is returned indicating that the data was received.
 */
QSharedPointer<Picto::ProtocolResponse> DirectorDataCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//printf("\nDIRECTORDATA  handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	QUuid commandUuid(command->getFieldValue("Command-ID"));
	Q_ASSERT(!commandUuid.isNull());

	response->setFieldValue("Command-ID",commandUuid.toString());
	notFoundResponse->setFieldValue("Command-ID",commandUuid.toString());

	//Start reading the content
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(command->getContent()));
	while(!xmlReader->atEnd() && !xmlReader->isStartElement())
		xmlReader->readNext();

	if(xmlReader->atEnd())
	{
		return notFoundResponse;
	}
	
	QString dataType = xmlReader->name().toString();

	if(!command->hasField("Session-ID"))
		return notFoundResponse;

	ConnectionManager *conMgr = ConnectionManager::Instance();
	conMgr->updateDirector(QHostAddress(command->getFieldValue("Source-Address")), command->getTarget(), DirectorStatus::running);

	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = conMgr->getSessionInfo(QUuid(command->getFieldValue("Session-ID")));
	
	if(sessionInfo.isNull())
		return notFoundResponse;

	//We do different things depending on the type of data being sent
	if(dataType == "BehavioralDataStore")
	{

		//Extract the behavioralDataStore
		Picto::BehavioralDataStore behaveData;
		behaveData.deserializeFromXml(xmlReader);

		//Open up the database and start writing out data
		QSqlDatabase sessionDb = sessionInfo->sessionDb();
		QSqlQuery sessionQ(sessionDb);

		sessionDb.transaction();
		
		Picto::BehavioralDataStore::BehavioralDataPoint dataPoint;
		while(behaveData.length() > 0)
		{
			dataPoint = behaveData.takeFirstDataPoint();
			sessionQ.prepare("INSERT INTO behavioraldata (xpos, ypos, time)"
				"VALUES(:xpos, :ypos, :time)");
			sessionQ.bindValue(":xpos",dataPoint.x);
			sessionQ.bindValue(":ypos",dataPoint.y);
			sessionQ.bindValue(":time",dataPoint.t);
			sessionQ.exec();
		}

		sessionDb.commit();
	}
	else
	{
		return notFoundResponse;
	}

	//printf("Returning good response: "+response->getDecodedContent() + "\n");
	return response;

}


#include "PutDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"


#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>

PutDataCommandHandler::PutDataCommandHandler()
{
}

/*! \brief handles a PUTDATA command
 *
 *	The data is stored in a database table appropraite to the type of data,
 *	and a response is returned indicating that the data was received.
 */
QSharedPointer<Picto::ProtocolResponse> PutDataCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//the random value makes it easy to observe scrolling on a full command prompt.
	//printf("PUTDATA  handler: %d %d\n", QThread::currentThreadId(), rand()%10);  //the random value makes it easy to bserve scrolling on 

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	//Start reading the content
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(command->getContent()));
	while(!xmlReader->atEnd() && !xmlReader->isStartElement())
		xmlReader->readNext();

	if(xmlReader->atEnd())
	{
		return notFoundResponse;
	}
	
	QString dataType = xmlReader->name().toString();

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

		sessionInfo->insertBehavioralData(behaveData);
	}
	else
	{
		return notFoundResponse;
	}

	return response;

}


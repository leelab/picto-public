#include "GetDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/StateDataUnit.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"
#include "../../common/timing/Timestamper.h"

#include <QXmlStreamWriter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>
#include "../../common/memleakdetect.h"

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

	QString timestamp = command->getTarget().split(':').value(1);
	QString dataType = command->getTarget().split(':').value(0,"");

	QByteArray xmlContent;
	if(dataType.compare("CurrentState",Qt::CaseInsensitive) == 0)
	{
		//Picto::Timestamper tStamper;
		//double startVal = tStamper.stampMs();
		xmlContent = QString("<Data>").append(sessionInfo->selectStateVariables(timestamp)).append("</Data>").toAscii();
		//qDebug(QString("Select at %1ms took: %2ms").arg(tStamper.stampMs()).arg(tStamper.stampMs()-startVal).toAscii());
	}
	else
	{
		notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
		return notFoundResponse;
	}

	response->setContent(xmlContent);
	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	return response;

}


#include "GetDataCommandHandler.h"

#include "../../common/globals.h"
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

/*! \brief Parses the input ProtocolCommand to check its syntax, responds with either current state data, the latest Neural Data or the latest Session Data 
 *	depending on the value befpre the colon in the command (CurrenState, LatestNeural, or SessionData).  In the case of CurrentState and LatestNeural, the 
 *	returned data includes everything after the time written after the colon in the command.  adds ENDSESSION pending directives to the SessionInfo object handling the input Session and responds 
 *	with a simple okResponse or other response depending on whether something was wrong with the request.
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

	QString dataTypeIndex = command->getTarget().split(':').value(1);
	QString dataType = command->getTarget().split(':').value(0,"");

	QByteArray xmlContent;
	if(dataType.compare("CurrentState",Qt::CaseInsensitive) == 0)
	{
		//Picto::Timestamper tStamper;
		//double startVal = tStamper.stampMs();
		xmlContent = QString("<Data>").append(sessionInfo->selectStateVariables(dataTypeIndex)).append("</Data>").toLatin1();
		//qDebug(QString("Select at %1ms took: %2ms").arg(tStamper.stampMs()).arg(tStamper.stampMs()-startVal).toLatin1());
	}
	else if(dataType.compare("LatestNeural",Qt::CaseInsensitive) == 0)
	{
		xmlContent = QString("<Data>").append(sessionInfo->selectLatestNeuralData(dataTypeIndex)).append("</Data>").toLatin1();
	} 
	else if(dataType.compare("SessionData",Qt::CaseInsensitive) == 0)
	{
		xmlContent = QString("<Data>").append(sessionInfo->selectSessionDataPackage()).append("</Data>").toLatin1();
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


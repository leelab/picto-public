#include "StartsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QUuid>
#include "../../common/memleakdetect.h"

StartsessionCommandHandler::StartsessionCommandHandler()
{
}

/*! \brief Handles STARTSESSION commands
 *	\details This function takes the incoming command and extracts the Director and Proxy that are to be used in this Session, as well
 *	as the serialized Design and DesignConfig.  All of this data is used to create a new SessionInfo object to handle the new Session.
 *	The new Session will cause NEWSESSION directives to be sent to the Director and Proxy in the ComponentUpdateCommandHandler.
 *	A LOADEXP directive is also added to the SessionInfo object to be sent to the Director.  Next, this thread yields until
 *	it sees the Director and Proxy have recieved the message and updated their status.  Finally when everything is clearly started 
 *	and ready for the new session, an okResponse is returned.
 */
QSharedPointer<Picto::ProtocolResponse> StartsessionCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("STARTSESSION handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> unauthorizedResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));
	unauthorizedResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	ConnectionManager *conMgr = ConnectionManager::Instance();
	
	QString target = command->getTarget();
	QString directorID;
	QString proxyID;
	QString password = command->getFieldValue("Password");
	QByteArray sessionDefs;
	QByteArray designXml;
	QByteArray DesignConfig;
	QUuid observerId;

	directorID = target.left(target.indexOf('/'));
	proxyID = target.right(target.indexOf('/'));
	sessionDefs = command->getContent();

	int configStart = sessionDefs.indexOf("<DesignConfig>");
	Q_ASSERT(configStart>-1);
	designXml = sessionDefs.left(configStart);
	DesignConfig = sessionDefs.mid(configStart);
	QString designName = command->getFieldValue("Experiment");
	if(designName.isEmpty())
		designName = "UntitledExperiment";

	observerId = QUuid(command->getFieldValue("Observer-ID"));
	
	if(observerId.isNull())
		return notFoundResponse;

#ifndef DEVELOPMENTBUILD
	if(password.isEmpty())
	{
		unauthorizedResponse->setContent("Empty Session Key's are not allowed");
		return unauthorizedResponse;
	}
#endif

	//Check that the Director is ready to go
	if(conMgr->getComponentStatus(directorID) == ComponentStatus::notFound)
	{
		notFoundResponse->setContent("Director ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getComponentStatus(directorID) > ComponentStatus::idle)
	{
		return unauthorizedResponse;
	}
	
	// Check to see that this component isnt associated with a session that's still open
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = conMgr->getSessionInfoByComponent(directorID);
	if(!sessionInfo.isNull())
	{	// If we got here, the old session is in the process of closing.  Don't start a new session until
		// its done.
		unauthorizedResponse->setContent("A session that this director was associated with is still in the process of closing.");
		return unauthorizedResponse;
		
	}

	//Check that the Proxy is ready to go
	if(proxyID == QUuid().toString())
	{
		//No Proxy in this experiment
	}
	else if(conMgr->getComponentStatus(proxyID) == ComponentStatus::notFound)
	{
		notFoundResponse->setContent("proxy ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getComponentStatus(proxyID) > ComponentStatus::idle)
	{
		return unauthorizedResponse;
	}

	//Get the experiment part of the design and make sure it isn't empty
	QString experimentXml = extractExperimentFromDesign(designXml);
	if(experimentXml.isEmpty())
		return notFoundResponse;

	//create the session
	sessionInfo = ConnectionManager::Instance()->createSession(QUuid(directorID), QUuid(proxyID), designName, designXml, DesignConfig, observerId, password);
	
	if(sessionInfo.isNull())
	{
		notFoundResponse->setContent("Failed to create SessionInfo object");
		return notFoundResponse;
	}

	//The director only needs the experiment part of the design.  Create a PictoData design containing only the experiment
	QString pendingDirective = "LOADEXP\n"+designXml;//"<PictoData>"+experimentXml+"</PictoData>";
	sessionInfo->addPendingDirective(pendingDirective,"DIRECTOR");

	//wait for the director to change to stopped status
	while(conMgr->getComponentStatus(directorID) == ComponentStatus::idle)
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
	}
	//wait for the proxy to change to stopped status if we're using a proxy.
	if(proxyID != QUuid().toString())
	{
		while(conMgr->getComponentStatus(directorID) == ComponentStatus::idle)
		{
			QThread::yieldCurrentThread();
			QCoreApplication::processEvents();
		}
	}

	QByteArray sessionIDXml;
	QXmlStreamWriter xmlWriter(&sessionIDXml);
	xmlWriter.writeStartElement("SessionID");
	xmlWriter.writeCharacters(sessionInfo->sessionId().toString());
	xmlWriter.writeEndElement();

	okResponse->setContent(sessionIDXml);

	return okResponse;
	

}

/*! \brief Extracts the Experiment part of a serielized Picto Design and returns it.
*/
QString StartsessionCommandHandler::extractExperimentFromDesign(QString designXML)
{
	QXmlStreamReader reader(designXML);
	while(!reader.isEndDocument() && !(reader.isStartElement() && reader.name().toString() == "Experiment"))
	{
		if(reader.readNext() == QXmlStreamReader::Invalid)
		{
			return QString();
		}
	}
	if(reader.isEndDocument())
		return QString();
	
	QString returnVal;
	QXmlStreamWriter xmlWriter(&returnVal);
	while(true)
	{
		if(reader.isEndDocument())
			return QString();
		xmlWriter.writeCurrentToken(reader);
		if(reader.isEndElement() && reader.name().toString() == "Experiment")
			break;
		if(reader.readNext() == QXmlStreamReader::Invalid)
		{
			return QString();
		}
	}
	return returnVal;
}
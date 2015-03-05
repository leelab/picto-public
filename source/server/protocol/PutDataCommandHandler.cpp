#include "PutDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/InputDataUnitPackage.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/storage/TaskRunDataUnit.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"


#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>
#include <QTime>
#include <QtConcurrentRun>
#include "../../common/memleakdetect.h"

PutDataCommandHandler::PutDataCommandHandler()
{
}

/*! \brief Handles PUTDATA commands.
 *	\details PUTDATA is probably the most heavily used command in Picto.  It is used to send all Behavioral and Neural
 *	data from the Director and Proxy to the Picto Server.  Each incoming command contains information about the source
 *	of the command in various command fields and serialized Session data in the command content area.  The Session data
 *	is serialized in XML format and an XML reader goes through the XML tags one by one, writing the data into the Session's SessionInfo
 *	object using the appropriate function each time.  ProtocolResponse objects returned from this object include any pending directives for 
 *	the calling Component, or a simple OK response otherwise.
 *	\note This function takes part in making sure the correct registered command responses are sent to Components to let them know which
 *	of their commands have been processed and their data saved to disk.  See Picto::RegisteredResponseType for more details.
 */
QSharedPointer<Picto::ProtocolResponse> PutDataCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//int messageIndex=0;
	//qDebug((QString("PUTDATA  handler: %1 %2").arg(command->getFieldValue("Source-ID")).arg(command->getFieldValue("Command-ID"))).toLatin1());
	//QTime commandProcessingTimer;
	//commandProcessingTimer.start();
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	ConnectionManager *conMgr = ConnectionManager::Instance();
	QHostAddress sourceAddr(command->getFieldValue("Source-Address"));
	QUuid sourceID(command->getFieldValue("Source-ID"));
	QString sourceType(command->getFieldValue("Source-Type"));
	QUuid sessionId(command->getFieldValue("Session-ID"));
	ComponentStatus::ComponentStatus status;

	QString statusStr;
	QString name;
	QString targetStr;

	targetStr = command->getTarget();
	name = targetStr.left(targetStr.indexOf(':'));
	statusStr = targetStr.right(targetStr.length() - targetStr.indexOf(':') - 1);

	if(statusStr.toUpper() == "IDLE")
	{
		status = ComponentStatus::idle;
		notFoundResponse->setContent("PUTDATA commands originating from an idle component are ignored.");
		return notFoundResponse;
	}
	else if(statusStr.toUpper() == "ENDING")
	{
		status = ComponentStatus::ending;
	}
	else if(statusStr.toUpper() == "STOPPED")
	{
		status = ComponentStatus::stopped;
	}
	else if(statusStr.toUpper() == "PAUSED")
	{
		status = ComponentStatus::paused;
	}
	else if(statusStr.toUpper() == "RUNNING")
	{
		status = ComponentStatus::running;
	}
	conMgr->updateComponent(sourceID,sourceAddr,sessionId, name,sourceType,status,"");

	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = conMgr->getSessionInfo(QUuid(command->getFieldValue("Session-ID")));


	if(sessionInfo.isNull())
		return notFoundResponse;
	
	//Start reading the content
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(command->getContent()));
	while(!xmlReader->atEnd() && !xmlReader->isStartElement() && xmlReader->text() != "Data")
		xmlReader->readNext();

	if(xmlReader->atEnd())
	{
		return notFoundResponse;
	}
	xmlReader->readNext();
	while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "Data") && !xmlReader->atEnd())
	{
		if(!xmlReader->isStartElement())
		{
			xmlReader->readNext();
			continue;
		}

		QString dataType = xmlReader->name().toString();

		//We do different things depending on the type of data being sent
		if(dataType == "BDUP")
		{
			//Extract the BehavioralDataUnitPackage
			QSharedPointer<Picto::BehavioralDataUnitPackage> behaveData(new Picto::BehavioralDataUnitPackage());
			behaveData->fromXml(xmlReader);

			sessionInfo->insertBehavioralData(behaveData);
		}
		else if (dataType == "IDUP")
		{
			//Extract the InputDataUnitPackage
			QSharedPointer<Picto::InputDataUnitPackage> inputData(new Picto::InputDataUnitPackage());
			inputData->fromXml(xmlReader);

			sessionInfo->insertInputData(inputData);
		}
		else if(dataType == "PropertyDataUnitPackage")
		{
			//Extract the PropertyDataUnitPackage
			QSharedPointer<Picto::PropertyDataUnitPackage> propData(new Picto::PropertyDataUnitPackage());
			propData->fromXml(xmlReader);
			sessionInfo->insertPropertyData(propData);
		}
		else if(dataType == "StateDataUnitPackage")
		{
			//deserialize the data store
			QSharedPointer<Picto::StateDataUnitPackage> stateData(new Picto::StateDataUnitPackage());
			stateData->fromXml(xmlReader);

			sessionInfo->insertStateData(stateData);
		}
		else if(dataType == "FrameDataUnitPackage")
		{
			QSharedPointer<Picto::FrameDataUnitPackage> frameData(new Picto::FrameDataUnitPackage());
			frameData->fromXml(xmlReader);

			sessionInfo->insertFrameData(frameData);
		}
		else if(dataType == "RDU")
		{
			QSharedPointer<Picto::RewardDataUnit> rewardData(new Picto::RewardDataUnit());
			rewardData->fromXml(xmlReader);

			sessionInfo->insertRewardData(rewardData);
		}
		else if(dataType == "NDU")
		{
			/*commandProcessingTimer.start();*/
			QSharedPointer<Picto::NeuralDataUnit> neuralData(new Picto::NeuralDataUnit());
			neuralData->fromXml(xmlReader);
			//qDebug((QString("Time to deserialize neural data: ")+QString::number(timer.elapsed())).toLatin1()); 
			sessionInfo->insertNeuralData(neuralData);
			//qDebug("NEURAL " + QString::number(messageIndex++).toLatin1() + " " + QString::number(commandProcessingTimer.elapsed()).toLatin1());
		}
		else if(dataType == "ADU")
		{
			/*commandProcessingTimer.start();*/
			QSharedPointer<Picto::AlignmentDataUnit> alignmentData(new Picto::AlignmentDataUnit());
			alignmentData->fromXml(xmlReader);

			sessionInfo->insertAlignmentData(alignmentData);
			//if(sourceType == "PROXY")
			//	qDebug("ALIGN  " + QString::number(messageIndex++).toLatin1() + " " + QString::number(commandProcessingTimer.elapsed()).toLatin1());
		}
		else if(dataType == "LFPDataUnitPackage")
		{
			/*commandProcessingTimer.start();*/
			QSharedPointer<Picto::LFPDataUnitPackage> lfpData(new Picto::LFPDataUnitPackage());
			lfpData->fromXml(xmlReader);
			//qDebug("LFPDeserialize  " + QString::number(messageIndex++).toLatin1() + " " + QString::number(commandProcessingTimer.elapsed()).toLatin1());

			sessionInfo->insertLFPData(lfpData);
			//qDebug("LFPTotal  " + QString::number(messageIndex++).toLatin1() + " " + QString::number(commandProcessingTimer.elapsed()).toLatin1());
		}
		else if(dataType == "TaskRunDataUnit")
		{
			/*commandProcessingTimer.start();*/
			QSharedPointer<Picto::TaskRunDataUnit> taskRunData(new Picto::TaskRunDataUnit());
			taskRunData->fromXml(xmlReader);
			sessionInfo->insertTaskRunData(taskRunData);
		}
		else
		{
			Q_ASSERT_X(false,"a",QString("DataType: "+dataType+" not found").toLatin1());
			return notFoundResponse;
		}

		xmlReader->readNext();
	}
	//if(sourceType == "PROXY")
	//	qDebug("Proxy Total  " + QString::number(messageIndex++).toLatin1() + " " + QString::number(commandProcessingTimer.elapsed()).toLatin1());
	QString directive = sessionInfo->pendingDirective(sourceID);
	bool lastDataPrecededFlush = sessionInfo->lastDataPrecededFlush(sourceType);
	bool lastDataFollowedFlush = sessionInfo->lastDataFollowedFlush(sourceType);
	sessionInfo->markLastDataTime(sourceType);
	bool currDataPrecededFlush = sessionInfo->lastDataPrecededFlush(sourceType);
	bool currDataFollowedFlush = sessionInfo->lastDataFollowedFlush(sourceType);
	int regType = 0;
	if(lastDataPrecededFlush && !currDataPrecededFlush)
	{
		regType |= Picto::RegisteredResponseType::FirstInCommandPackage;
	}
	if(!lastDataFollowedFlush && currDataFollowedFlush)
	{
		regType |= Picto::RegisteredResponseType::SendLastCommandPackage;
	}
	response->setRegisteredType(Picto::RegisteredResponseType::RegisteredResponseType(regType));


	if(directive.isEmpty())
	{
		response->setContent("OK");
	}
	else
	{
		qDebug(QString("Sent %1 Directive to %2").arg(directive).arg(sourceType).toLatin1());
		response->setContent(directive.toUtf8());
	}
	return response;
}


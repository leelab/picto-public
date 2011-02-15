#include "PutDataCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../../common/storage/StateDataStore.h"
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

	ConnectionManager *conMgr = ConnectionManager::Instance();
	QUuid sourceID(command->getFieldValue("Source-ID"));
	QString sourceType(command->getFieldValue("Source-Type"));
	conMgr->updateComponent(sourceID,QHostAddress(command->getFieldValue("Source-Address")), command->getTarget(),sourceType);

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
		if(dataType == "BehavioralDataStore")
		{
			//Extract the behavioralDataStore
			Picto::BehavioralDataStore behaveData;
			behaveData.deserializeFromXml(xmlReader);

			sessionInfo->insertBehavioralData(behaveData);
		}
		else if(dataType == "StateDataStore")
		{
			//deserialize the data store
			Picto::StateDataStore stateData;
			stateData.deserializeFromXml(xmlReader);

			sessionInfo->insertStateData(stateData);
		}
		else if(dataType == "FrameDataStore")
		{
			Picto::FrameDataStore frameData;
			frameData.deserializeFromXml(xmlReader);

			sessionInfo->insertFrameData(frameData);
		}
		else if(dataType == "RewardDataStore")
		{
			Picto::RewardDataStore rewardData;
			rewardData.deserializeFromXml(xmlReader);

			sessionInfo->insertRewardData(rewardData);
		}
		else if(dataType == "event")
		{
			if(xmlReader->attributes().value("type").toString() == "spike")
			{
				Picto::NeuralDataStore neuralData;
				neuralData.deserializeFromXml(xmlReader);

				sessionInfo->insertNeuralData(neuralData);
			}
			else if(xmlReader->attributes().value("type").toString() == "externalEvent")
			{
				double time = 0.0;
				int eventCode = -1;
				int trialNum = -1;
				while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "event") && !xmlReader->atEnd())
				{
					if(!xmlReader->isStartElement())
					{
						xmlReader->readNext();
						continue;
					}
					if(xmlReader->name() == "timestamp")
					{
						time = xmlReader->readElementText().toDouble();
					}
					else if(xmlReader->name() == "eventCode")
					{
						eventCode = xmlReader->readElementText().toInt();
					}
					xmlReader->readNext();
				}
				sessionInfo->insertTrialEvent(time,eventCode,trialNum,sourceType);
			}
		}
		else if((dataType == "device"))
		{	//Ignore these until they are handled correctly in NeuralDataStore
			while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "device") && !xmlReader->atEnd())
			{xmlReader->readNext();}
		}
		else if((dataType == "deviceStatus"))
		{	//Ignore these until they are handled correctly in NeuralDataStore
			while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "deviceStatus") && !xmlReader->atEnd())
			{xmlReader->readNext();}
		}
		else if((dataType == "sampleRate"))
		{	//Ignore these until they are handled correctly in NeuralDataStore
			while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "sampleRate") && !xmlReader->atEnd())
			{xmlReader->readNext();}
		}
		else if((dataType == "numEvents"))
		{	//Ignore these until they are handled correctly in NeuralDataStore
			while(!(xmlReader->isEndElement() && xmlReader->name().toString() == "numEvents") && !xmlReader->atEnd())
			{xmlReader->readNext();}
		}
		else
		{
			Q_ASSERT_X(false,"a",QString("DataType: "+dataType+" not found").toAscii());
			return notFoundResponse;
		}

		xmlReader->readNext();
	}

	QString directive = sessionInfo->pendingDirective(sourceID);
	if(directive.isEmpty())
		response->setContent("OK");
	else
		response->setContent(directive.toUtf8());
	
	return response;
}


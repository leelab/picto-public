#include "Task.h"
#include "../storage/StateDataUnit.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"

namespace Picto {

Task::Task() 
{
		AddDefinableProperty("Name","Unnamed Task");
		AddDefinableObjectFactory("StateMachine",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(StateMachine::Create))));

}

//! returns the name of this task
QString Task::name()
{
	return propertyContainer_->getPropertyValue("Name").toString();
}

//! Sets the name of this task
void Task::setName(QString name)
{
	propertyContainer_->setPropertyValue("Name",name);
}

bool Task::run(QSharedPointer<Engine::PictoEngine> engine)
{
	if(stateMachine_.isNull())
	{
		return false;
	}
	else
	{
		//Before running the statemachine, we need to append the task name to its path
		stateMachine_->setPath(QStringList()<<name());

		if(engine->slaveMode())
		{
			stateMachine_->runAsSlave(engine);
		}
		else
		{
			sendInitialStateDataToServer(engine);

			QString result;
			result = stateMachine_->run(engine);

			//After the task has finished running, we need to report the final result.
			//This can't be done within the state machine, because the state machine
			//has no idea if it's the top level.
			sendFinalStateDataToServer(result, engine);
		}

		return true;
	}
}

//!	Jumps to the specified state in the task's main state machine
bool Task::jumpToState(QStringList path, QString state)
{
	//If there's nothing left in the path, it means that we haven't started 
	//running the task yet, so we don't need to do any jumping
	if(path.isEmpty())
		return true;

	//The first name in the path should be the name of our state machine
	if(path.takeFirst() != stateMachine_->getName())
		return false;

	return stateMachine_->jumpToState(path,state);
}

/*! \brief Sends the initial state transition to the server
 *
 *	If a task is stopped, and then restarted, the transitions recordered by the server
 *	will be this:
 *		1. Final transition before stop command recieved
 *		2. First transition within state machine after start
 *	This is a problem for a remotely viewed session, since if it tries to join just after
 *	the task is restarted, it will put itself into the destination state from 1, which 
 *	probably won't be the source state for 2.
 *
 *	To fix this issue, we'll generate a starting the task transition with the following
 *	values:
 *		source: NULL
 *		sourceResult: NULL
 *		destination: the task's statemachine
 */
void Task::sendInitialStateDataToServer(QSharedPointer<Engine::PictoEngine> engine)
{
	// JOEY 12/24/10 If there's no data channel, we're running in test mode.
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
		return;

	Q_ASSERT(sendStateData("NULL","NULL",stateMachine_->getName(),engine));
}


/*! \brief Sends the final state transition to the server
 *
 *	When a task completes, the final state transition never gets sent to the server.
 *	by the state machine, so we send it here.
 */
void Task::sendFinalStateDataToServer(QString result, QSharedPointer<Engine::PictoEngine> engine)
{
	// JOEY 12/24/10 If there's no data channel, we're running in test mode.
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
		return;

	if(result == "EngineAbort")
		Q_ASSERT(sendStateData("NULL",result,"NULL",engine));
	else
		Q_ASSERT(sendStateData(stateMachine_->getName(),result,"NULL",engine));

	//while(dataChannel->pendingResponses())
	//{
	//	// AssureConnection gets up to 2 seconds to make a connection.  If not, we
	//	// can't send anything, so we try again;
	//	if(!dataChannel->assureConnection(2000))
	//		continue;

	//	QSharedPointer<ProtocolResponse> resp;

	//	//reading responses
	//	while(dataChannel->waitForResponse(0))
	//	{
	//		resp = dataChannel->getResponse();
	//		Q_ASSERT(!resp.isNull());
	//		Q_ASSERT(resp->getResponseType() == "OK");
	//	}

	//	//resend all of the pending responses
	//	dataChannel->resendPendingCommands(10);
	//}


}

//! \brief Sends state data to the server
bool Task::sendStateData(QString source, QString sourceResult, QString destination, QSharedPointer<Engine::PictoEngine> engine)
{
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
		return false;
	
	QString status = "running";
	int engCmd = engine->getEngineCommand();
	switch(engCmd)
	{
	case Engine::PictoEngine::ResumeEngine:
		status = "running";
		break;
	case Engine::PictoEngine::PauseEngine:
		status = "paused";
		break;
	case Engine::PictoEngine::StopEngine:
		status = "stopped";
		break;
	}
	QString dataCommandStr = "PUTDATA " + engine->getName() + ":" + status + " PICTO/1.0";

	QSharedPointer<Picto::ProtocolResponse> dataResponse;
	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

	QByteArray stateDataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&stateDataXml));

	Timestamper stamper;
	double timestamp = stamper.stampSec();

	StateDataUnit stateData;
	stateData.setTransition(source,sourceResult,destination,timestamp,name());

	xmlWriter->writeStartElement("Data");
	stateData.toXml(xmlWriter);
	xmlWriter->writeEndElement();

	dataCommand->setContent(stateDataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(stateDataXml.length()));

	dataChannel->sendRegisteredCommand(dataCommand);
	dataChannel->processResponses(0);

	//if(!dataChannel->waitForResponse(1000))
	//	return false;
	//dataResponse = dataChannel->getResponse();
	//if(dataResponse->getResponseCode() != ProtocolResponseType::OK)
	//	return false;

	return true;
}


/*! \brief Turns this task into an XML fragment
 *
 *	The XML for a task looks like this:
 *	
 *	<Task>
 *		<Name>SomeExperiment</Name>
 *		<StateMachine>
 *			...
 *		</StateMachine>
 *	</Task>
 */
//bool Task::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Task");
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//
//	xmlStreamWriter->writeStartElement("StateMachine");
//	stateMachine_->toXml(xmlStreamWriter);
//	xmlStreamWriter->writeEndElement(); //StateMachine
//
//	xmlStreamWriter->writeEndElement(); //Task
//	
//	return true;
//}
//
////! Turns XML into a Task
//bool Task::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Task")
//	{
//		addError("Task","Incorrect tag, expected <Task>",xmlStreamReader);
//		return false;
//	}
//	
//	xmlStreamReader->readNext();
//	
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Task") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Name")
//		{
//			setName(xmlStreamReader->readElementText());
//		}
//		else if(name == "StateMachine")
//		{
//			stateMachine_ = QSharedPointer<StateMachine>(new StateMachine);
//			if(!stateMachine_->fromXml(xmlStreamReader))
//			{
//				addError("Task","State Machine failed to deserialize", xmlStreamReader);
//				return false;
//			}
//		}
//		else
//		{
//			addError("Task", "Unexpected tag", xmlStreamReader);
//			return false;
//		}		
//
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Task", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//	if(stateMachine_->validateStateMachine())
//	{
//		return true;
//	}
//	else
//	{
//		addError("StateMachine","StateMachine failed validation",xmlStreamReader);
//		return false;
//	}
//}

bool Task::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QList<QSharedPointer<Asset>> stateMachines = getGeneratedChildren("StateMachine");
	if(!stateMachines.isEmpty())
	{
		stateMachine_ = stateMachines.first().staticCast<StateMachine>();
	}
	return true;
}

}; //namespace Picto

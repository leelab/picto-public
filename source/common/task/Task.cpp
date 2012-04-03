#include "Task.h"
#include "../storage/StateDataUnit.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

Task::Task() 
{
		taskNumber_ = 0;
		AddDefinableObjectFactory("StateMachine",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(StateMachine::Create))));

}

QSharedPointer<Task> Task::Create()
{
	return QSharedPointer<Task>(new Task());
}

QString Task::run(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result = "";
	if(stateMachine_.isNull())
	{
		return result;
	}
	else
	{
		//Before running the statemachine, we need to append the task name to its path
		stateMachine_->setPath(QStringList()<<getName());

		if(engine->slaveMode())
		{
			result = stateMachine_->runAsSlave(engine);
		}
		else
		{
			sendInitialStateDataToServer(engine);

			result = stateMachine_->run(engine);

			//After the task has finished running, we need to report the final result.
			//This can't be done within the state machine, because the state machine
			//has no idea if it's the top level.
			sendFinalStateDataToServer(result, engine);
			result = "";
		}

		return result;
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

void Task::setTaskNumber(int num)
{
	taskNumber_ = num;
	initTransition_ = QSharedPointer<Transition>(new Transition("","",stateMachine_->getName()));
	initTransition_->setAssetId(-taskNumber_);
	initTransition_->setSelfPtr(initTransition_);
	initTransition_->setParentAsset(selfPtr());
	expConfig_->addManagedAsset(initTransition_);	//This adds the transition to the expConfig list so that it will be recognized
													//By the server and remote workstations.  In the future we will add transitions
													//into initial states which will make all of this unnecessary.
	expConfig_->fixDuplicatedAssetIds();
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

	QSharedPointer<Transition> tran(new Transition("","",stateMachine_->getName()));
	tran->setAssetId(-taskNumber_);
	engine->addStateTransitionForServer(tran);
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
	{

		QSharedPointer<Transition> tran(new Transition("",result,""));
		engine->addStateTransitionForServer(tran);
	}
	else
	{
		QSharedPointer<Transition> tran(new Transition(stateMachine_->getName(),result,""));
		engine->addStateTransitionForServer(tran);
	}
	//Even though there's no actual frame here, calling reportNewFrame has the effect of 
	//marking all of the latest data with a new frame id and sending this data to the server.
	//Since there's no frame after the end of the experiment, this "fake last frame" takes 
	//its place.
	Timestamper stamper;
	engine->reportNewFrame(stamper.stampSec(),getAssetId());
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
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->getName() != "Task")
//	{
//		addError("Task","Incorrect tag, expected <Task>",xmlStreamReader);
//		return false;
//	}
//	
//	xmlStreamReader->readNext();
//	
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->getName().toString() == "Task") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->getName().toString();
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

void Task::postDeserialize()
{
	UIEnabled::postDeserialize();
	QList<QSharedPointer<Asset>> stateMachines = getGeneratedChildren("StateMachine");
	if(!stateMachines.isEmpty())
	{
		stateMachine_ = stateMachines.first().staticCast<StateMachine>();
	}
}

bool Task::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

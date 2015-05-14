#include <QDateTime>
#include "Task.h"
#include "../storage/StateDataUnit.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new Task object.
 *	\details Automatically adds a required StateMachine object to the Task.
 */
Task::Task() 
	: taskNumber_(0)
{
	sizeList_ << "1x1" << "2x2" << "3x3" << "4x4";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "DefaultTaskViewSize", 1, "enumNames", sizeList_);
	AddDefinableObjectFactory("StateMachine",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(StateMachine::Create))));
}

/*! \brief Creates a new Task object and returns a shared Asset pointer to it.
 */
QSharedPointer<Task> Task::Create()
{
	return QSharedPointer<Task>(new Task());
}

/*! \brief Runs this Task's execution logic within the framework of the input PictoEngine.
 *	\details Since the Task only ever contains a single StateMachine, this function just takes care of initializing the
 *	new Task run, running the StateMachine, and sending any final data to the Server (if necessary).  
 *	\note Currently, this function blocks the Qt Event Loop execution until it finishes except in the case where the
 *	PictoEngine is running in slave mode, in which case the event loop is processed at various times from within this
 *	engine (which is not a very clean way to do things).  In the future this function should be rewritten to run a
 *	single frame at a time from within an event loop.  See CommandChannel::processResponses() for more details.
 */
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
			result = stateMachine_->slaveRun(engine);
		}
		else
		{
			sendInitialStateDataToServer(engine);
			resetScriptableValues();
			result = stateMachine_->run(engine);

			//After the task has finished running, we need to report the final result.
			//  This can't be done within the state machine, because the state machine has no idea if it's the top level.
			sendFinalStateDataToServer(result, engine);
			result = "";
		}

		return result;
	}
}

/*! \brief Sets the index of this Task within its container.
 *	\details This is useful since the Task's index is used to define an "initial transition" that is sent to the server
 *	during run time to indicate that this Task is the one that was run.  The idea here was that it is necessary to
 *	create a "fake" transition, since execution only really begins at the Task level, so it is somewhat illogical to
 *	store a Transition in the design above the top level of the StateMachine.  This can certainly be reconsidered at
 *	some point and changed, so long as reverse design compatibility is maintained somehow.
 */
void Task::setTaskNumber(int num)
{
	taskNumber_ = num;
	initTransition_ = QSharedPointer<Transition>(new Transition(QSharedPointer<Asset>(),QSharedPointer<Asset>(),stateMachine_));
	initTransition_->setAssetId(-taskNumber_);
	initTransition_->setSelfPtr(initTransition_);
	initTransition_->setParentAsset(selfPtr());
	//This adds the transition to the designConfig list so that it will be recognized by the server and remote
	//  workstations.  In the future we can consider adding transitions into initial states which would make all of
	//  this unnecessary.
	designConfig_->addManagedAsset(initTransition_);	
	
	designConfig_->fixDuplicatedAssetIds();
}

/*! \brief Sends the initial state transition to the server
 *	\details Takes care of some initial operations for the Task before it is actually run.
 *	These include things like, marking the start time of the Task, setting up the 
 *	default name of the Task Run using this Task start time, sending the initial transition
 *	data to the server, etc.
 *	
 *	\note If a task is stopped, and then restarted, the transitions recordered by the server
 *	would be this:
 *		- Final transition before stop command recieved
 *		- First transition within state machine after start
 *	This is a problem for a remotely viewed session, since if it tries to join just after
 *	the task is restarted, it will put itself into the destination state from 1, which 
 *	probably won't be the source state for 2.
 *	To fix this issue, we generate a "starting the task" transition with the following
 *	values:
 *		- source: NULL
 *		- sourceResult: NULL
 *		- destination: the task's statemachine
 */
void Task::sendInitialStateDataToServer(QSharedPointer<Engine::PictoEngine> engine)
{
	// JOEY 12/24/10 If there's no data channel, we're running in test mode.
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
	{
		//If there's no data channel, this is a Test run.  Restart the FrameResolutionTimer every time.
		QDateTime dateTime = QDateTime::currentDateTime();
		QString taskRunName = getName()+"_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");	
		engine->markTaskRunStart(taskRunName);
		return;
	}

	QSharedPointer<Transition> tran(new Transition(QSharedPointer<Asset>(),QSharedPointer<Asset>(),stateMachine_));
	tran->setAssetId(-taskNumber_);
	engine->addStateTransitionForServer(tran);
	QDateTime dateTime = QDateTime::currentDateTime();
	QString taskRunName = getName()+"_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");	
	engine->markTaskRunStart(taskRunName);
	//At the beginning of a new TaskRun, all of the initial property values are sent to
	//the server.  This means that the director gets a huge number of "received" responses
	//at an unknown time near the beginning of the experiment that can cause frame skipping.
	//To work around this, we report a virtual initial frame that will automatically include
	//all of the initial property data.  We then wait until we get "received" responses for
	//that data before continuing.
	Timestamper stamper;
	double frameTime = stamper.stampSec();
	engine->reportNewFrame(frameTime,getAssetId());
	while(dataChannel->pendingResponses())
	{
		dataChannel->processResponses(100);
	}
}


/*! \brief Sends the final StateMachine Transition to the server
 *
 *	When a task completes, the final Transition never gets sent to the server by the StateMachine, so we send it here.
 *	We also create a fake final frame whose frameId can be used to stamp all of the Property changes, etc that occured
 *	after the last frame that was actually presented.
 */
void Task::sendFinalStateDataToServer(QString result, QSharedPointer<Engine::PictoEngine> engine)
{
	// JOEY 12/24/10 If there's no data channel, we're running in test mode.
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
	{
		engine->markTaskRunStop();
		return;
	}

	if(result == "EngineAbort")
	{

		QSharedPointer<Transition> tran(new Transition(result));
		engine->addStateTransitionForServer(tran);
	}
	else
	{
		QSharedPointer<Transition> tran(new Transition(stateMachine_,result));
		engine->addStateTransitionForServer(tran);
	}
	//Even though there's no actual frame here, calling reportNewFrame has the effect of 
	//marking all of the latest data with a new frame id and sending this data to the server.
	//Since there's no frame after the end of the experiment, this "fake last frame" takes 
	//its place.
	Timestamper stamper;
	double frameTime = stamper.stampSec();
	engine->markTaskRunStop();
	engine->reportNewFrame(frameTime,getAssetId());
}

/*! \brief Extends ScriptableContainer::preDeserialize() to create the TaskConfig object for this task.
 */
void Task::preDeserialize()
{
	setTaskConfig(QSharedPointer<TaskConfig>(new TaskConfig()));

	ScriptableContainer::preDeserialize();
}

/*! \brief Extends ScriptableContainer::postDeserialize() to set the UIEnabled Property invisible since it doesn't do
 *	anything in Task objects.
 */
void Task::postDeserialize()
{
	//Pass the default view size to the TaskConfig
	getTaskConfig()->setTaskViewSize(getDefaultViewSize());

	ScriptableContainer::postDeserialize();
	QList<QSharedPointer<Asset>> stateMachines = getGeneratedChildren("StateMachine");
	if(!stateMachines.isEmpty())
	{
		stateMachine_ = stateMachines.first().staticCast<StateMachine>();
		stateMachine_->hideName();
	}
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
	propertyContainer_->getProperty("Name")->setVisible(false);
}

bool Task::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

//! Propagates new name to Task and child StateMachine - reducing complexity
void Task::rename(const QString &newName)
{
	setName(newName);
	if (!stateMachine_.isNull())
	{
		stateMachine_->setName(newName);
	}
}

DataViewSize::ViewSize Task::getDefaultViewSize() const
{
	//We add one to the value because ViewSizes are enumerated from 1 for ease of size-calculation.
	return (DataViewSize::ViewSize)(propertyContainer_->getPropertyValue("DefaultTaskViewSize").toInt() + 1);
}

}; //namespace Picto

/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */

#include "PictoEngine.h"

#include "../timing/Timestamper.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/PropertyDataUnit.h"
#include "../storage/FrameDataUnit.h"

#include <QApplication>
#include <QTime>
#include <QtConcurrentRun>

namespace Picto {
	namespace Engine {


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	propTable_(NULL),
	slave_(false),
	userIsOperator_(false),
	lastTimePropChangesRequested_("0.0"),
	lastTimeStateDataRequested_("0.0"),
	currBehavUnit_(QSharedPointer<BehavioralDataUnit>(new BehavioralDataUnit())),
	currStateUnit_(QSharedPointer<StateDataUnit>(new StateDataUnit())),
	runningPath_(""),
	engineCommand_(NoCommand)
{
	bExclusiveMode_ = true;
	setSessionId(QUuid());
}

void PictoEngine::setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType)
{
	timingType_ = _timingType;
}

QList<QSharedPointer<RenderingTarget> > PictoEngine::getRenderingTargets()
{
	return renderingTargets_;
}

void PictoEngine::addRenderingTarget(QSharedPointer<RenderingTarget> target)
{	
	renderingTargets_.append(target);
}

bool PictoEngine::hasVisibleRenderingTargets()
{
	foreach(QSharedPointer<RenderingTarget> target, renderingTargets_)
	{
		if(target->getVisualTarget()->isVisible())
			return true;
	}
	return false;
}

QSharedPointer<SignalChannel> PictoEngine::getSignalChannel(QString name)
{
	return signalChannels_.value(name, QSharedPointer<SignalChannel>());
}

void PictoEngine::addSignalChannel(QString name, QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(name, channel);
}

//! \brief Starts every signal channel in the engine
void PictoEngine::startAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->start();
	}
}

//! \brief Stops every signal channel in the engine
void PictoEngine::stopAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->stop();
	}
}

void PictoEngine::generateEvent(unsigned int eventCode)
{
	if(!eventCodeGenerator_.isNull())
		eventCodeGenerator_->sendEvent(eventCode);
}


//! \brief Issues a reward and sends notfication to the server
void PictoEngine::giveReward(int channel)
{
	if(rewardController_.isNull())
		return;

	Timestamper stamper;
	double timestamp = stamper.stampSec();
	double lastMsgTime = 0;

	//Since we don't want the server to timeout the director, we make sure that we
	//update the server at least once per second by running th giveReward function
	//in as separate thread.
	QFuture<void> future = QtConcurrent::run(rewardController_.data(),&RewardController::giveReward,channel);
	//while(!future.isFinished())
	//{
	//	QCoreApplication::processEvents();
	//	if(stamper.stampSec() > (lastMsgTime+1))
	//	{
	//		if((!dataCommandChannel_.isNull()) && (!slave_))
	//		{
	//			QString status = "running";
	//			int engCmd = getEngineCommand();
	//			switch(engCmd)
	//			{
	//			case Engine::PictoEngine::ResumeEngine:
	//				status = "running";
	//				break;
	//			case Engine::PictoEngine::PauseEngine:
	//				status = "paused";
	//				break;
	//			case Engine::PictoEngine::StopEngine:
	//				status = "stopped";
	//				break;
	//			}
	//			QString updateCommandStr = "COMPONENTUPDATE "+getName()+":"+status+" PICTO/1.0";
	//			QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));
	//			dataCommandChannel_->sendCommand(updateCommand);
	//		}
	//		lastMsgTime = stamper.stampSec();
	//	}
	//}
	if(dataCommandChannel_.isNull())
		return;

	if(slave_)
		return;

	int duration = rewardController_->getRewardDurationMs(channel);
	RewardDataUnit rewardData(duration,channel,timestamp);

	QString dataCommandStr = "PUTDATA "+getName()+" PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

	QByteArray dataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&dataXml));

	xmlWriter->writeStartElement("Data");

	rewardData.toXml(xmlWriter);

	xmlWriter->writeEndElement();  //End Data

	dataCommand->setContent(dataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(dataXml.length()));

	//Send the command in registered mode (so we don't have to wait for a response)
	dataCommandChannel_->sendRegisteredCommand(dataCommand);


}

void PictoEngine::addChangedProperty(QSharedPointer<Property> changedProp)
{
	if(slave_)
		return;
	if(!propPackage_)
		propPackage_ = QSharedPointer<PropertyDataUnitPackage>(new PropertyDataUnitPackage());
	Timestamper stamper;
	//If the changedProp has no parent, its a UI parameter.  Set the path as such.
	propPackage_->addData(changedProp->getIndex(),changedProp->getParentAsset()?changedProp->getPath():"UIParameter",changedProp->toUserString(),stamper.stampSec());
}
//! \brief Retrieves the latest package of changed properties.
//! Note that a package can only be retrieved once after which a new package is created.
QSharedPointer<PropertyDataUnitPackage> PictoEngine::getChangedPropertyPackage()
{
	QSharedPointer<PropertyDataUnitPackage> returnVal = propPackage_;
	propPackage_.clear();
	return returnVal;
}

//! \brief Gets the latest property changes from the server and applies them to the local properties.
void PictoEngine::updatePropertiesFromServer()
{
	return;
	if(propTable_.isNull())
		return;
	//Collect the data from the server
	QString commandStr = QString("GETDATA PropertyDataUnitPackage:%1 PICTO/1.0").arg(lastTimePropChangesRequested_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	slaveCommandChannel_->sendCommand(command);
	//No response
	if(!slaveCommandChannel_->waitForResponse(1000))
		return;

	response = slaveCommandChannel_->getResponse();

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return;
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return;

	PropertyDataUnitPackage propData;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "PropertyDataUnitPackage")
		{
			propData.fromXml(xmlReader);
		}
		else
		{
			return;
		}
	}

	while(propData.length() > 0)
	{
		// Push the data into our signal channel
		QSharedPointer<Picto::PropertyDataUnit> dataPoint;
		dataPoint = propData.takeFirstDataPoint();

		propTable_->updatePropertyValue(dataPoint->index_,dataPoint->value_);
		//qDebug(QString("Received Prop: %1\nval:\n%2\n\n").arg(dataPoint->path_,dataPoint->value_).toAscii());
		lastTimePropChangesRequested_ = dataPoint->time_;
	}
}


bool PictoEngine::updateCurrentStateFromServer()
{
	//Clear any old responses out of the response queue
	while(slaveCommandChannel_->incomingResponsesWaiting())
		slaveCommandChannel_->getResponse();
	if(!slaveCommandChannel_->assureConnection(100))
		return false;

	//Collect the data from the server
	QString commandStr = QString("GETDATA CurrentState:%1 PICTO/1.0").arg(lastTimeStateDataRequested_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	slaveCommandChannel_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//qDebug(QString("Sent command: %1 at Time:%2").arg(commandID).arg(command->getFieldValue("Time-Sent")).toAscii());

	do
	{
		QCoreApplication::processEvents();
		if(!slaveCommandChannel_->waitForResponse(1000))
		{
			return false;
		}
		response = slaveCommandChannel_->getResponse();
		//if(response)
		//	qDebug(QString("Received command: %1 sent by server at Time:%2").arg(response->getFieldValue("Command-ID")).arg(response->getFieldValue("Time-Sent")).toAscii());
	}while(!response || response->getFieldValue("Command-ID") != commandID);

	////No response
	//if(!slaveCommandChannel_->waitForResponse(1000))
	//	return false;

	//response = slaveCommandChannel_->getResponse();

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return false;
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return false;

	PropertyDataUnitPackage propData;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "PropertyDataUnit")
		{
			PropertyDataUnit unit;
			unit.fromXml(xmlReader);
			propData.addData(unit.index_,unit.path_,unit.value_,unit.time_);
			if(unit.time_.toDouble() > lastTimeStateDataRequested_.toDouble())
				lastTimeStateDataRequested_ = unit.time_;
		}
		else if(xmlReader->name() == "BehavioralDataUnit")
		{
			currBehavUnit_->fromXml(xmlReader);
			getSignalChannel("PositionChannel")->insertValue("xpos",currBehavUnit_->x);
			getSignalChannel("PositionChannel")->insertValue("ypos",currBehavUnit_->y);
			getSignalChannel("PositionChannel")->insertValue("time",currBehavUnit_->t.toDouble());
			if(currBehavUnit_->t.toDouble() > lastTimeStateDataRequested_.toDouble())
				lastTimeStateDataRequested_ = currBehavUnit_->t;
		}
		else if(xmlReader->name() == "StateDataUnit")
		{
			currStateUnit_->fromXml(xmlReader);
			if(currStateUnit_->getTime().toDouble() > lastTimeStateDataRequested_.toDouble())
				lastTimeStateDataRequested_ = currStateUnit_->getTime();
		}
		else if(xmlReader->name() == "FrameDataUnit")
		{
			FrameDataUnit unit;
			unit.fromXml(xmlReader);
			if(unit.time.toDouble() > lastTimeStateDataRequested_.toDouble())
				lastTimeStateDataRequested_ = unit.time;
			//qDebug(QString("WORKSTATION: RECEIVED FRAME: %1").arg(unit.frameNumber).toAscii());
		}
		xmlReader->readNext();
	}

	while(propData.length() > 0)
	{
		// Push the data into our signal channel
		QSharedPointer<Picto::PropertyDataUnit> dataPoint;
		dataPoint = propData.takeFirstDataPoint();

		propTable_->updatePropertyValue(dataPoint->index_,dataPoint->value_);
		//qDebug(QString("Received Prop: %1\nval:\n%2\n\n").arg(dataPoint->path_,dataPoint->value_).toAscii());
	}
	return true;
}

void PictoEngine::setRunningPath(QString path)
{
	runningPath_ = path;
}

QString PictoEngine::getServerPathUpdate()
{
	QString result = currStateUnit_->getDestination();
	if(result == "NULL")
		result = "";
	if(result != "EngineAbort")
		result.prepend(currStateUnit_->getMachinePath()+"::");
	if(result == runningPath_)
		result = "";
	setRunningPath(currStateUnit_->getMachinePath()+"::"+currStateUnit_->getDestination());
	return result;
}
QSharedPointer<Picto::BehavioralDataUnit> PictoEngine::getCurrentBehavioralData()
{
	return currBehavUnit_;
}

//! Sets the CommandChannel used for data.  Returns true if the channel's status is connected
bool PictoEngine::setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	dataCommandChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}

QSharedPointer<CommandChannel> PictoEngine::getDataCommandChannel()
{
	return dataCommandChannel_;
}

//! Sets the CommandChannel used for updates.  Returns true if the channel's status is connected
bool PictoEngine::setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	updateCommandChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}

QSharedPointer<CommandChannel> PictoEngine::getUpdateCommandChannel()
{
	return updateCommandChannel_;
}

void PictoEngine::setPropertyTable(QSharedPointer<PropertyTable> propTable)
{
	if(propTable_ == propTable)
		return;
	if(propTable_)
		disconnect(propTable_.data(),SIGNAL(propertyChanged(QSharedPointer<Property>)),this,SLOT(addChangedProperty(QSharedPointer<Property>)));
	propTable_ = propTable;	
	connect(propTable_.data(),SIGNAL(propertyChanged(QSharedPointer<Property>)),this,SLOT(addChangedProperty(QSharedPointer<Property>)));
}

void PictoEngine::sendAllPropertyValuesToServer()
{
	Q_ASSERT(propTable_);
	propTable_->reportChangeInAllProperties();
}

void PictoEngine::setSessionId(QUuid sessionId)
{
	sessionId_ = sessionId;
	if(!dataCommandChannel_.isNull())
		dataCommandChannel_->setSessionId(sessionId_);
	if(!updateCommandChannel_.isNull())
		updateCommandChannel_->setSessionId(sessionId_);
}

int PictoEngine::getEngineCommand()
{
	return engineCommand_;
}
 
void PictoEngine::stop()
{ 
	engineCommand_ = StopEngine; 
	stopAllSignalChannels();
}


}; //namespace Engine
}; //namespace Picto

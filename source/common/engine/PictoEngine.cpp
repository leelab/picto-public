/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */

#include "PictoEngine.h"

#include "../timing/Timestamper.h"
#include "../storage/RewardDataStore.h"

#include <QApplication>
#include <QTime>
#include <QtConcurrentRun>

namespace Picto {
	namespace Engine {


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	slave_(false)
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
	while(!future.isFinished())
	{
		QCoreApplication::processEvents();
		if(stamper.stampSec() > (lastMsgTime+1))
		{
			if((!dataCommandChannel_.isNull()) && (!slave_))
			{
				QString status = "running";
				int engCmd = getEngineCommand();
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
				QString updateCommandStr = "COMPONENTUPDATE "+getName()+":"+status+" PICTO/1.0";
				QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));
				dataCommandChannel_->sendCommand(updateCommand);
			}
			lastMsgTime = stamper.stampSec();
		}
	}
	if(dataCommandChannel_.isNull())
		return;

	if(slave_)
		return;

	int duration = rewardController_->getRewardDurationMs(channel);
	RewardDataStore rewardData(duration,channel,timestamp);

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

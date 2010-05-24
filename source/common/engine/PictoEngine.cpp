/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */

#include "PictoEngine.h"

#include <QApplication>
#include <QTime>

namespace Picto {
	namespace Engine {

QList<QSharedPointer<RenderingTarget> > PictoEngine::renderingTargets_;
QMap<QString, QSharedPointer<SignalChannel> > PictoEngine::signalChannels_;
QSharedPointer<RewardController> PictoEngine::rewardController_;
QSharedPointer<EventCodeGenerator> PictoEngine::eventCodeGenerator_;
QSharedPointer<CommandChannel> PictoEngine::commandChannel_;
QUuid PictoEngine::sessionId_;
QString PictoEngine::name_;
bool PictoEngine::bExclusiveMode_;
int PictoEngine::engineCommand_;

PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise)
{
	bExclusiveMode_ = true;
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

void PictoEngine::giveReward(int channel)
{
	if(!rewardController_.isNull())
		rewardController_->giveReward(channel);
}

//! Sets the CommandChannel.  Returns true if the channel's status is connected
bool PictoEngine::setCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
	{
		return false;
	}
	else
	{
		commandChannel_ = commandChannel;
		return true;
	}
}

QSharedPointer<CommandChannel> PictoEngine::getCommandChannel()
{
	return commandChannel_;
}

void PictoEngine::setSessionId(QUuid sessionId)
{
	sessionId_ = sessionId;
	if(!commandChannel_.isNull())
		commandChannel_->setSessionId(sessionId_);
}


bool PictoEngine::loadExperiment(QSharedPointer<Picto::Experiment> experiment)
{
	experiment_ = experiment;

	return true;
}

//! Runs the task with the passed in name
bool PictoEngine::runTask(QString taskName)
{
	bool success;
	engineCommand_ = NoCommand;
	startAllSignalChannels();
	success = experiment_->runTask(taskName);
	stopAllSignalChannels();
	return success;
}

int PictoEngine::getEngineCommand()
{
	return engineCommand_;
}


}; //namespace Engine
}; //namespace Picto

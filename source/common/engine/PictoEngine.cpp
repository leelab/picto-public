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


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	bExclusiveMode_(false)
{
}

void PictoEngine::setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType)
{
	timingType_ = _timingType;
}

void PictoEngine::beginExclusiveMode()
{
	/*! \todo Implement Me */
}

void PictoEngine::endExclusiveMode()
{
	/*! \todo Implement Me */
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

/*! \brief Sends the passed in command over the command channel.  
 *
 *	The passed in command is sent out over the command channel and the response (since all
 *	commands are expected to generate responsese) is stored in response.  If no respose is
 *	recieved within the timeout period, or if there is some other problem, false is returned.
 */
QSharedPointer<ProtocolResponse> PictoEngine::sendCommand(QSharedPointer<ProtocolCommand> command, int timeout)
{
	QSharedPointer<ProtocolResponse> nullResponse;
	if(commandChannel_.isNull())
	{
		return nullResponse;
	}
	if(commandChannel_->getChannelStatus() == CommandChannel::disconnected)
	{
		return nullResponse;
	}
	if(commandChannel_->incomingResponsesWaiting() >0)
	{
		Q_ASSERT(false);
		return nullResponse;
	}
	commandChannel_->sendCommand(command);

	if(commandChannel_->waitForResponse(timeout))
	{
		return commandChannel_->getResponse();
	}
	else
	{
		return nullResponse;
	}

}


void PictoEngine::loadExperiment(QSharedPointer<Picto::Experiment> experiment)
{
	experiment_ = experiment;
}

//! Runs the task with the passed in name
bool PictoEngine::runTask(QString taskName)
{
	return experiment_->runTask(taskName);
}


}; //namespace Engine
}; //namespace Picto

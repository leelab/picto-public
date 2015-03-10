#include <QTextStream>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QTcpSocket>
#include <QHostAddress>
#include <QApplication>
#include <QUuid>
#include <QTime>
#include <QSettings>

#include "../../common/globals.h"
#include "DirectorInterface.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*! \file 
 * \def RESPONSEDELAYMS 
 * \brief The number of milliseconds that this interface waits from the time it sends a command to the 
 * director application until it gives up on waiting for a reply.
 */
#define RESPONSEDELAYMS 200
/*! \brief Constructs a new DirectorInterface object.
 * \details This function sets up the director interface.  It takes in a panelInfo object pointer, since that
 * is the object that creates and manages the sockets to the Director application.  It also sets up the local
 * reward/flush handling system that allows the Front Panel application to trigger rewards and flushes even
 * if the Picto Director application is not running.  This is particularly important if the PictoBox is being
 * used with some other non-picto application (ie. Legacy Orion application).  For the purpose of functioning
 * with Leelab's legacy Orion application, all reward/flush durations are stored in the Windows registry and 
 * this is set up in this constructor.  
 * Note that the latest Orion code has been updated to query this registry entry for reward durations so that
 * Orion can run on Pictoboxes and use the reward timing data set through their front panel.
 */
DirectorInterface::DirectorInterface(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo),
	lastIp_(""),
	lastStatus_(DirUnknown),
	lastCommandId_(0)
{
	//Setup local reward supply incase director isn't attached
	rewardController_ = QSharedPointer<FrontPanelRewardController>(new FrontPanelRewardController());
	rewardTimer_ = QSharedPointer<QTimer>(new QTimer);
	rewardTimer_->setInterval(16);
	connect(rewardTimer_.data(), SIGNAL(timeout()), this, SLOT(updateLocalRewards()));

	//Figure out reward durations
	QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
	if(settings.contains("RewardDurations"))
	{
		rewardDurs_ = settings.value("RewardDurations").toString().split(":",QString::SkipEmptyParts);
	}
	else
	{
		rewardDurs_ << "100" << "100" << "100" << "100" << "100";
		settings.setValue("RewardDurations",rewardDurs_.join(":"));
	}

	//Figure out flush durations
	if(settings.contains("FlushDurations"))
	{
		flushDurs_ = settings.value("FlushDurations").toString().split(":",QString::SkipEmptyParts);
	}
	else
	{
		flushDurs_ << "300" << "300" << "300" << "300" << "300";
		settings.setValue("FlushDurations",flushDurs_.join(":"));
	}
}

DirectorInterface::~DirectorInterface()
{
	delete connectionTimer;
}

/*! \brief Not yet implemented in the director appliction
 * \details This function is supposed to change the ip address of the director.  This is not yet supported
 * by the director. When it is supported, it will return true on success and false on failure.
 * currently true and false only indicate whether the message was sent.  It doesn't actually do
 * anything yet though. Trust me.
 */
bool DirectorInterface::setIP(QString addr)
{
	Picto::ProtocolCommand command("FPPUT /IP PICTO/1.0");
	QByteArray content = QString("%1").arg(addr).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return false;
	return true;
}
/*! \brief Gets the current IP address from the Direction application. 
 * \details Unlike the setIP() function, this function works correctly on the director side.  On
 * succesful communication, a string containing the ip address is returned, otherwise a blank string
 * is returned.
 */
QString DirectorInterface::getIP()
{
	//send a command
	Picto::ProtocolCommand command("FPGET /IP PICTO/1.0");
		QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return lastIp_;
	}
	lastIp_ = reply;
	return lastIp_;
}

/*! \brief Sets the name of the Pictobox to the Director application 
 * \details Returns true on successful set, false on failure.
 */
bool DirectorInterface::setName(QString name)
{
	//inform the engine of the change
	Picto::ProtocolCommand command(QString("FPPUT /boxname PICTO/1.0"));
	QByteArray content = QString("%1").arg(name).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return false;
	return true;
}

/*! \brief Returns the current name of the PictoBox as read from the Director application
 * \details Returns 'Picto Not Found' when communication fails.  Don't name the PictoBox
 * 'Picto Not Found', that would be confusing...
 */
QString DirectorInterface::getName()
{
	//send a command
	Picto::ProtocolCommand command("FPGET /boxname PICTO/1.0");
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return "Picto Not Found";
	}
	return reply;
}

/*! \brief Returns a DirectorStatus enum containing the current status as reported by the Director application
 * \details See DirectorStatus for interpretations of returned values.
 */
DirectorStatus DirectorInterface::getStatus()
{
	//send a command
	Picto::ProtocolCommand command("FPGET /status PICTO/1.0");
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return DirUnknown;
	}

	if(reply == "Running")
		lastStatus_ = DirRun;
	else if(reply == "Paused")
		lastStatus_ = DirPause;
	else if(reply == "Idle")
		lastStatus_ = DirConnect;
	else if(reply == "Ending")
		lastStatus_ = DirEnding;
	else if(reply == "Stopped")
		lastStatus_ = DirStop;
	else if(reply == "Disconnected")
		lastStatus_ = DirDisconnect;
	return lastStatus_;
}

/*! \brief Sets the reward duration of the for the input controller number to the input reward duration (in ms)
 * \details Returns true on successful set, false on failure.
 */
bool DirectorInterface::setRewardDuration(int rewardDur,int controller)
{
	if(controller < 0)
		return false;
	//Update locally stored reward duration
	if(rewardDur != rewardDurs_[controller].toInt())
	{
		rewardDurs_[controller] = QString::number(rewardDur);
		QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
		settings.setValue("RewardDurations",rewardDurs_.join(":"));
	}
	//inform the engine of the change
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/duration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(rewardDur).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return true;
	return true;
}

/*! \brief Returns the current reward duration for the input controller number
 * \details Returns -1 when communication fails or the reward duration is ms if communication is successful
 */
int DirectorInterface::getRewardDuration(int controller)
{
	if(controller < 0 || controller > 4)
		return -1;
	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/duration PICTO/1.0").arg(controller));
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return rewardDurs_[controller].toInt();
	}
	int newDur = reply.toInt();
	if(newDur != rewardDurs_[controller].toInt())
	{
		rewardDurs_[controller] = QString::number(newDur);
		QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
		settings.setValue("RewardDurations",rewardDurs_.join(":"));
	}
	return rewardDurs_[controller].toInt();
}

/*! \brief Sets the reward duration of the for the input controller number to the input flush duration (in seconds)
 * \details Returns true on successful set, false on failure.
 */
bool DirectorInterface::setFlushDuration(int flushDur,int controller)
{
	if(controller < 0 || controller > 4)
		return false;
	//Update locally stored flush duration
	if(flushDur != rewardDurs_[controller].toInt())
	{
		flushDurs_[controller] = QString::number(flushDur);
		QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
		settings.setValue("FlushDurations",flushDurs_.join(":"));
	}

	//inform the engine of the change
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/flushduration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(flushDur).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return true;
	return true;
}
/*! \brief Returns the current flush duration for the input controller number
 * \details Returns -1 when communication fails or the flush duration is seconds if communication is successful
 */
int DirectorInterface::getFlushDuration(int controller)
{
	if(controller < 0)
		return -1;
	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushduration PICTO/1.0").arg(controller));
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return flushDurs_[controller].toInt();
	}
	int newDur = reply.toInt();
	if(newDur != flushDurs_[controller].toInt())
	{
		flushDurs_[controller] = QString::number(newDur);;
		QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
		settings.setValue("FlushDurations",flushDurs_.join(":"));
	}
	return flushDurs_[controller].toInt();
}

/*! \brief Starts a flush or stops it if one is already in progress on the input controller
 *	\details If a flush is already in progress for the input controller, it is stopped, otherwise
 * a message is sent to the director application to begin a flush according to the currently set 
 * flush duration.  Note that flushes can still occur even if the director is not running.  In this
 * case flushes are simply handled locally within this application.  This is important in case the 
 * pictobox is being used with some third party application (ie. Legacy Orion software) and not the 
 * Picto Director.  This function returns false only if the input controller number or the currently
 * set flush duration were invalid, otherwise it always successfully flushes whether or not the 
 * director application is reachable.
 * \sa updateLocalRewards()
 */
bool DirectorInterface::flush(int controller)
{
	//If we started a flush locally, abort it.
	if(rewardController_->isFlushing(controller))
	{
		//qDebug("Aborting flush");
		rewardController_->abortFlush(controller);
		return true;
	}
	if(controller < 0)
		return false;
	//send out a start flushing command
	Picto::ProtocolCommand command(QString("FPFLUSH /reward/%1 PICTO/1.0").arg(controller));
	if(!sendCommandGetResponse(command,&QString()))
	{
		//Couldn't reach director.  Do reward locally.
		int flushDur = getFlushDuration(controller);
		if(flushDur < 0)
			return false;
		rewardController_->flush(controller,flushDur);
		if(!rewardTimer_->isActive())
			rewardTimer_->start();
		return true;
	}
	return true;
}

/*! \brief Returns the number of seconds remaining before the currently active flush will be completed.
 * \details This only works with the Director application.  If flushes are performed local to the 
 * Front Panel application (see flush()) this function will always return -1.
 */
int DirectorInterface::getFlushTimeRemaining(int controller)
{
	if(controller < 0)
		return -1;
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushtimeremain PICTO/1.0").arg(controller));
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
		return -1;
	return reply.toInt();
}

/*! \brief Starts a reward on the input controller
 *	\details This function sends a message to the director application to begin a reward according to 
 * the currently set reward duration on the input controller.  Note that rewards can still occur even 
 * if the director is not running.  In this case rewards are simply handled locally within this 
 * application.  This is important in case the pictobox is being used with some third party application 
 * (ie. Legacy Orion software) and not the Picto Director.  This function returns false only if the 
 * input controller number or the currently set reward duration were invalid, otherwise it always 
 * successfully starts a reward whether or not the director application is reachable.
 * \sa updateLocalRewards()
 */
bool DirectorInterface::startReward(int controller)
{
	if(controller < 0)
		return false;
	//qDebug(QTime::currentTime().toString().toLatin1() + ": Start Reward Sent");

	//tell the engine to give a reward
	Picto::ProtocolCommand command(QString("FPREWARD /reward/%1 PICTO/1.0").arg(controller));
	if(!sendCommandGetResponse(command,&QString()))
	{
		//Couldn't reach director.  Do reward locally.
		int rewDur = getRewardDuration(controller);
		if(rewDur < 0)
			return false;
		rewardController_->addReward(controller,rewDur,rewDur);
		if(!rewardTimer_->isActive())
			rewardTimer_->start();
		return true;
	}
	return true;
}


/*! \brief Returns true if the Director application is connected to this Front Panel application, false otherwise.
 */
bool DirectorInterface::isConnected()
{
	//check to see if we've connected to the engine yet
	QTcpSocket *commSock;
	commSock = panelInfo->getCommandSocket();

	if (!commSock || !commSock->isValid())
	{
		return false;
	}
	return true;
}

/*! Sends the input command to the Director application and returns the response in the string pointed to by reply if the function returns true.
 * The DirectorInterface connects the Front Panel application to the director over a socket.  After
 * sending a command over this socket, it waits RESPONSEDELAYMS for a reply.  If that reply is 
 * received in time, it puts it into the reply string and returns true.  If the connection fails
 * or the director returns an error message, the function returns false.
 */
bool DirectorInterface::sendCommandGetResponse(Picto::ProtocolCommand command,QString* reply)
{
	Q_ASSERT(reply);
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	if(!commSock)
		return false;
	if(command.getContent().size())
		command.setFieldValue("Content-Length", QString("%1").arg(command.getContent().size()));
	QString commandId = QString::number(++lastCommandId_);
	command.setFieldValue("Command-ID",commandId);
	command.write(commSock);
	//qDebug("SentID: " + commandId.toLatin1());
	Picto::ProtocolResponse response;
	int r = response.read(commSock,RESPONSEDELAYMS);
	//If we got a reponse (r>=0) but it wasn't for our command, throw it out and read a new one.
	while((r >= 0) && (response.getFieldValue("Command-ID") != commandId))
	{
		//qDebug("ReceivedID: " + response.getFieldValue("Command-ID").toLatin1());
		r = response.read(commSock,RESPONSEDELAYMS);
	}
	(*reply) = "";
	if(r < 0)	//A response to our command didn't arrive in time.
		return false;
	//qDebug("ReceivedID: " + response.getFieldValue("Command-ID").toLatin1());

	(*reply) = QString(response.getDecodedContent());
	if(response.getResponseCode() != 200)
		return false;
	return true;
}

/*! \brief This function is called periodically by the reward timer to update the state of the local rewards and flushes
 * \details The reward timer that calls this function is started whenever a local reward or flush begins 
 * it calls this function every 16ms.  This function starts by telling the reward controller to handle
 * any pending rewards.  This has the effect of stopping rewards that are done and starting new ones
 * that are waiting.  If there are no more pending rewards, and the latest rewards and flushes are all finished 
 * the function stops the rewardTimer_ until the next local reward or flush is started.
 * \sa flush(), startReward()
 */
void DirectorInterface::updateLocalRewards()
{
	rewardController_->triggerRewards(false);
	if(rewardController_->hasPendingRewards())
		return;
	for(int i=0;i<5;i++)
	{
		if(rewardController_->rewardInProgress(i) 
			|| rewardController_->isFlushing(i))
			return;
	}
	//qDebug("Stopping reward timer");
	rewardTimer_->stop();
}
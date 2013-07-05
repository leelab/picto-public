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

#define RESPONSEDELAYMS 200
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

bool DirectorInterface::setIP(QString addr)
{
	Picto::ProtocolCommand command("FPPUT /IP PICTO/1.0");
	QByteArray content = QString("%1").arg(addr).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return false;
	return true;
}

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



bool DirectorInterface::isConnected()
{
	//check to see if we've connected to the engine yet
	QTcpSocket *commSock, *eventSock;
	commSock = panelInfo->getCommandSocket();
	eventSock = panelInfo->getEventSocket();

	//The order in this "if" is important, since if we call isValid() on a 
	//null pointer, bad things will happen. (Remember, || will return true
	//when it encounters the first true expression.)
	if(!commSock || !eventSock ||
		!commSock->isValid()  || !eventSock->isValid())
	{
		return false;
	}
	return true;
}

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
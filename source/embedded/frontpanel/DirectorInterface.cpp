#include <QTextStream>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QTcpSocket>
#include <QHostAddress>
#include <QApplication>
#include <QUuid>
#include <QTime>

#include "DirectorInterface.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

#define RESPONSEDELAYMS 200
DirectorInterface::DirectorInterface(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo),
	lastIp_(""),
	lastStatus_(DirUnknown),
	lastRewardDur_(-1),
	lastFlushDur_(-1),
	lastName_(""),
	lastCommandId_(0)
{
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
		return lastName_;
	}
	lastName_ = reply;
	return lastName_;
}

DirectorStatus DirectorInterface::getStatus()
{
	//send a command
	Picto::ProtocolCommand command("FPGET /status PICTO/1.0");
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return DirDisconnect;
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
	//inform the engine of the change
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/duration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(rewardDur).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return false;
	return true;
}


int DirectorInterface::getRewardDuration(int controller)
{
	if(controller < 0)
		return -1;
	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/duration PICTO/1.0").arg(controller));
	QString reply;
	if(!sendCommandGetResponse(command,&reply))
	{
		return lastRewardDur_;
	}
	lastRewardDur_ = reply.toInt();
	return lastRewardDur_;
}

bool DirectorInterface::setFlushDuration(int flushDur,int controller)
{
	if(controller < 0)
		return false;
	//inform the engine of the change
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/flushduration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(flushDur).toUtf8();
	command.setContent(content);
	if(!sendCommandGetResponse(command,&QString()))
		return false;
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
		return lastFlushDur_;
	lastFlushDur_ = reply.toInt();
	return lastFlushDur_;
}

bool DirectorInterface::flush(int controller)
{
	if(controller < 0)
		return false;
	//send out a start flushing command
	Picto::ProtocolCommand command(QString("FPFLUSH /reward/%1 PICTO/1.0").arg(controller));
	if(!sendCommandGetResponse(command,&QString()))
		return false;
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
	qDebug(QTime::currentTime().toString().toAscii() + ": Start Reward Sent");
	//NOTE: No indication is given through the UI that a reward has been issued.
	//In the Lee lab, this is not a problem, since the reward solenoids are loud.
	//However, it might be desirable to give some sort of indication of a reward
	//This could be done using the "reward trigger" light on the front panel.

	//tell the engine to give a reward
	Picto::ProtocolCommand command(QString("FPREWARD /reward/%1 PICTO/1.0").arg(controller));
	if(!sendCommandGetResponse(command,&QString()))
		return false;
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
	qDebug("SentID: " + commandId.toAscii());
	Picto::ProtocolResponse response;
	int r = response.read(commSock,RESPONSEDELAYMS);
	//If we got a reponse (r>=0) but it wasn't for our command, throw it out and read a new one.
	while((r >= 0) && (response.getFieldValue("Command-ID") != commandId))
	{
		qDebug("ReceivedID: " + response.getFieldValue("Command-ID").toAscii());
		r = response.read(commSock,RESPONSEDELAYMS);
	}
	(*reply) = "";
	if(r < 0)	//A response to our command didn't arrive in time.
		return false;
	qDebug("ReceivedID: " + response.getFieldValue("Command-ID").toAscii());

	(*reply) = QString(response.getDecodedContent());
	if(response.getResponseCode() != 200)
		return false;
	return true;
}

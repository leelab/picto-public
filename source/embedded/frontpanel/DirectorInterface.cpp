#include <QTextStream>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QTcpSocket>
#include <QHostAddress>
#include <QApplication>

#include "DirectorInterface.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

#define RESPONSEDELAYMS 1000
DirectorInterface::DirectorInterface(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo)
{
}

DirectorInterface::~DirectorInterface()
{
	delete connectionTimer;
}

bool DirectorInterface::setIP(QString addr)
{
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command("FPPUT /IP PICTO/1.0");
	QByteArray content = QString("%1").arg(addr).toUtf8();

	command.setContent(content);
	command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}

QString DirectorInterface::getIP()
{
	//grab the system IP from the engine, and update panelInfo
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	//send a command
	Picto::ProtocolCommand command("FPGET /IP PICTO/1.0");
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock,RESPONSEDELAYMS);
	if(bytesRead<0)
	{
		return "";
	}
	return engineResponse.getDecodedContent();
}

bool DirectorInterface::setName(QString name)
{
	//inform the engine of the change
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPPUT /boxname PICTO/1.0"));
	QByteArray content = QString("%1").arg(name).toUtf8();

	command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
	command.setContent(content);
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}

QString DirectorInterface::getName()
{
	//grab the system name from the engine, and update panelInfo
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	//send a command
	Picto::ProtocolCommand command("FPGET /boxname PICTO/1.0");
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock,RESPONSEDELAYMS);
	if(bytesRead<0)
	{
		return "";
	}

	QString name = engineResponse.getDecodedContent();
	return name;
}

bool DirectorInterface::setRewardDuration(int rewardDur,int controller)
{
	//inform the engine of the change
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/duration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(rewardDur).toUtf8();

	command.setContent(content);
	command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}


int DirectorInterface::getRewardDuration(int controller)
{
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/duration PICTO/1.0").arg(controller));
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock,RESPONSEDELAYMS);
	if(bytesRead<0)
	{
		return -1;
	}

	QString duration = engineResponse.getDecodedContent();
	return duration.toInt();
}

bool DirectorInterface::setFlushDuration(int flushDur,int controller)
{
	//inform the engine of the change
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPPUT /reward/%1/flushduration PICTO/1.0").arg(controller));
	QByteArray content = QString("%1").arg(flushDur).toUtf8();

	command.setContent(content);
	command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}

int DirectorInterface::getFlushDuration(int controller)
{
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushduration PICTO/1.0").arg(controller));
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock,RESPONSEDELAYMS);
	if(bytesRead<0)
	{
		return -1;
	}

	QString duration = engineResponse.getDecodedContent();
	return duration.toInt();
}

bool DirectorInterface::startFlush(int controller)
{
	//send out a start flushing command
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPSTARTFLUSH /reward/%1 PICTO/1.0").arg(controller));
	command.write(commSock);

	//get a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}
int DirectorInterface::getFlushTimeRemaining(int controller)
{
	int timeRem;

	//get the time remaining in the flush
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushtimeremain PICTO/1.0").arg(controller));
	command.write(commSock);

	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock,RESPONSEDELAYMS);
	if(bytesRead<0)
	{
		return -1;
	}

	return engineResponse.getDecodedContent().toInt();
}



bool DirectorInterface::stopFlush(int controller)
{
	//send out a stop flushing command
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPSTOPFLUSH /reward/%1 PICTO/1.0").arg(controller));
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
	}
	return true;
}

bool DirectorInterface::startReward(int controller)
{
	//NOTE: No indication is given through the UI that a reward has been issued.
	//In the Lee lab, this is not a problem, since the reward solenoids are loud.
	//However, it might be desirable to give some sort of indication of a reward
	//This could be done using the "reward trigger" light on the front panel.

	//tell the engine to give a reward
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPREWARD /reward/%1 PICTO/1.0").arg(controller));
	command.write(commSock);

	//wait for a response from the engine 
	QApplication::processEvents();
	Picto::ProtocolResponse response;
	response.read(commSock,RESPONSEDELAYMS);
	if(response.getResponseCode() != 200)
	{
		return false;
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

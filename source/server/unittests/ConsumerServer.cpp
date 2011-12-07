#include "ConsumerServer.h"

#include <QNetworkInterface>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QByteArray>

#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/protocol/ProtocolResponse.h"
#include "../../common/memleakdetect.h"

ConsumerServer::ConsumerServer(int port, QObject *parent) :
	QTcpServer(parent)
{
	QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
	if(!hostAddresses.empty())
	{
		serverAddress.setAddress(hostAddresses[0].toIPv4Address());
	}

	listen(serverAddress, port);

	numCommandsToSend = -1;
	numResponsesReceived = 0;

	incomingCommands = 0;
	incomingResponses = 0;

	timer = new QTimer();
}

void ConsumerServer::incomingConnection(int socketDescriptor)
{
	//Set up a new socket
	QTcpSocket *newSocket;
	newSocket = new QTcpSocket;
	newSocket->setSocketDescriptor(socketDescriptor);

	connect(newSocket, SIGNAL(readyRead()), this, SLOT(readClient()));



	//Note that we're simply guessing which connection is which.  Once a 
	//command arrives, we can switch the sockets if needed.
	if(!incomingCommands && !incomingResponses)
		incomingCommands = newSocket;
	else if(incomingCommands && !incomingResponses)
		incomingResponses = newSocket;
	else
		delete newSocket;
}

//This is a really dumb "server", it does the following:
//	- Any line that start with TEST results in the socket which received the
//	  command being declared the incomingCommands socket, and the other
//	  socket being declared incomingResponses
//	- Any line that start with TEST followed by /X results in X commands
//	  being sent to the incomingResponses socket
//  - Any line that starts with PICTO is assumed to be a response from one
//	  of the POST commands sent out by sendCommands() 
//

//Since this "server" is only for test purposes, I'm not using a command handler,
//but instead parsing everything directly
void ConsumerServer::readClient()
{
	QTcpSocket *socket;
	socket = (QTcpSocket*)QObject::sender();

	QString incomingData;
	while(socket->canReadLine())
	{
		incomingData = socket->readLine();

		if(incomingData.startsWith("TEST"))
		{
			if(socket != incomingCommands)
			{
				QTcpSocket *temp;
				temp = incomingCommands;
				incomingCommands = incomingResponses;
				incomingResponses = temp;
			}

			QStringList commandTokens = incomingData.split(' ');
			Q_ASSERT(commandTokens.size() > 1);

			numCommandsToSend = commandTokens[1].mid(1).toInt();
			Q_ASSERT(numCommandsToSend >= 0);

			timer->setInterval(numCommandsToSend*50+1000);
			connect(timer, SIGNAL(timeout()), this, SLOT(sendReport()));
			timer->start();

			sendCommands();
		}
		else if(incomingData.startsWith("PICTO"))
		{
			Q_ASSERT(socket == incomingResponses);

			numResponsesReceived++;

			if(numResponsesReceived == numCommandsToSend)
				sendReport();
		}
	}


}

void ConsumerServer::sendCommands()
{
	for(int i=0; i<numCommandsToSend; i++)
	{
		Picto::ProtocolCommand command(QString("POST /test%1 PICTO/1.0").arg(i));
		command.write(incomingResponses);
	}
}

void ConsumerServer::sendReport()
{
	timer->stop();

	//generate report
	QByteArray xmlReport;
	QXmlStreamWriter writer(&xmlReport);

	//this just makes debugging easier
	writer.setAutoFormatting(true);

	writer.writeStartDocument();
	writer.writeStartElement("ConsumerServerReport");

	if(numCommandsToSend == numResponsesReceived)
		writer.writeAttribute("status","pass");
	else
		writer.writeAttribute("status","fail");

	writer.writeAttribute("commandsSent",QString("%1").arg(numCommandsToSend));
	writer.writeAttribute("responsesReceived",QString("%1").arg(numResponsesReceived));

	writer.writeEndElement();  //ConsumerServerReport


	Picto::ProtocolResponse response("ConsumerServer (for testing)", "PICTO", "1.0", Picto::ProtocolResponseType::OK);

	response.setContent(xmlReport);

	response.write(incomingCommands);
}

#include "serverthread.h"

#include <QtNetwork>
#include <QDateTime>

ServerThread::ServerThread(int socketDescriptor, QSharedPointer<ServerProtocols> _protocols, QObject *parent)
    : QThread(parent),
      socketDescriptor(socketDescriptor),
	  pendingCommand(""),
	  protocols(_protocols)
{
}

void ServerThread::run()
{
	QTcpSocket socket;

	tcpSocket = &socket;

    if (!socket.setSocketDescriptor(socketDescriptor))
    {
        emit error(socket.error());
        return;
    }

	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readClient()), Qt::DirectConnection);
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDroppedClient()), Qt::DirectConnection);

	//We establish the timer here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	timer = new QTimer();
	timer->setSingleShot(true);
	timer->setInterval(timeoutInterval);
	connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()), Qt::DirectConnection);
	timer->start();

	QThread::exec();

	delete timer;

	socket.disconnectFromHost();
	if(socket.state() != QTcpSocket::UnconnectedState)
	{
		socket.waitForDisconnected();
	}
}

bool ServerThread::receiveContent(QSharedPointer<Picto::ProtocolCommand> command)
{
	int remainingContentLength = command->getFieldValue("Content-Length").toInt();

	while(command->isPendingContent())
	{
		if(!tcpSocket->waitForReadyRead(timeoutInterval))
		{
			return false;
		}
		else
		{
			remainingContentLength = command->addToContent(tcpSocket->read(remainingContentLength));
		}
	}

	return true;
}

QSharedPointer<Picto::ProtocolResponse> ServerThread::processCommand(QSharedPointer<Picto::ProtocolCommand> _command)
{
	QSharedPointer<Picto::Protocol> protocol = protocols->getProtocol(_command->getProtocolName());
	if(protocol.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> unknownProtocolResponse(
			new Picto::ProtocolResponse("PICTO",
										"1.0",
										Picto::ProtocolResponseType::BadRequest));

		return unknownProtocolResponse;
	}

	if(_command->isPendingContent())
	{
		QSharedPointer<Picto::ProtocolResponse> incompleteRequestResponse(
			new Picto::ProtocolResponse(protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::IncompleteRequest));

		return incompleteRequestResponse;
	}

	if(!_command->isWellFormed())
	{
		QSharedPointer<Picto::ProtocolResponse> malformedRequestResponse(
			new Picto::ProtocolResponse(protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::BadRequest));

		return malformedRequestResponse;
	}
	
	QSharedPointer<Picto::ProtocolCommandHandler> handler = protocol->getCommandHandler(_command->getMethod());
	if(handler.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> notImplementedResponse(
			new Picto::ProtocolResponse(protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::NotImplemented));

		return notImplementedResponse;
	}
	
	QSharedPointer<Picto::ProtocolResponse> response = handler->processCommand(_command);

	return response;
}

void ServerThread::deliverResponse(QSharedPointer<Picto::ProtocolResponse> response)
{
	if(response.isNull())
	{
		response = QSharedPointer<Picto::ProtocolResponse>(
			new Picto::ProtocolResponse("PICTO",
										"1.0",
										Picto::ProtocolResponseType::InternalServerError));
	}

	QDataStream os(tcpSocket);

	if(response->getMultiPart() <= Picto::MultiPartResponseType::MultiPartInitial)
	{
		QString headers = response->getHeaders();
		os.writeRawData(headers.toUtf8().data(),headers.toUtf8().length());
		os.writeRawData("\r\n",2);
	}

	if(response->getMultiPart() == Picto::MultiPartResponseType::MultiPartPart)
	{
		os.writeRawData("\r\n",2);
	}

	if(response->getMultiPart() >= Picto::MultiPartResponseType::MultiPartInitial)
	{
		QString multiPartHeaders = response->getMultiPartHeaders();
		os.writeRawData(multiPartHeaders.toUtf8().data(),multiPartHeaders.toUtf8().length());
		os.writeRawData("\r\n",2);
	}

	if(response->hasContent())
	{
		QByteArray data = response->getContent();
		os.writeRawData(data.data(),data.length());
	}

	if(response->shouldTerminateConnection())
	{
		exit();
	}
}

void ServerThread::readClient()
{
	timer->stop();

	while(tcpSocket->canReadLine())
	{
		QString currentLine = tcpSocket->readLine();
		QRegExp newLineRegExp("[\r\n]+");

		if(newLineRegExp.exactMatch(currentLine))
		{
			QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(pendingCommand));
			if(command->isPendingContent())
			{
				receiveContent(command);
			}
			QSharedPointer<Picto::ProtocolResponse> response;
			do
			{
				response = processCommand(command);
				QString acceptedEncodings = command->getFieldValue("Accept-Encoding");
				QStringList acceptedEncodingList = acceptedEncodings.split(",");
				if(response->getMultiPart() < Picto::MultiPartResponseType::MultiPartInitial)
				{
					if(acceptedEncodingList.contains("gzip", Qt::CaseInsensitive))
					{
						response->setContentEncoding(Picto::ContentEncodingType::gzip);
					}
					else if(acceptedEncodingList.contains("deflate", Qt::CaseInsensitive))
					{
						response->setContentEncoding(Picto::ContentEncodingType::deflate);
					}
				}
				deliverResponse(response);
				if(response->shouldStream())
				{
					command->setFieldValue("X-PictoStreamState",response->getFieldValue("X-PictoStreamState"));
				}
			} while(tcpSocket->state() == QTcpSocket::ConnectedState &&
				    tcpSocket->waitForBytesWritten() &&
					response->shouldStream());

			pendingCommand.clear();
		}
		else
		{
			pendingCommand += currentLine;
		}
	}

	timer->start();
}

void ServerThread::handleDroppedClient()
{
	exit();
}

void ServerThread::handleTimeout()
{
	//QSharedPointer<Picto::ProtocolResponse> response = new Picto::ProtocolResponse(protocol->id(),protocol->version(),Picto::ProtocolResponseType::ConnectionTimedOut);
	//deliverResponse(response);
	exit();
}

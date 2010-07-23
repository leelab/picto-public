#include "serverthread.h"
#include "../../common/globals.h"

#include "../connections/ConnectionManager.h"

#include <QtNetwork>
#include <QDateTime>

ServerThread::ServerThread(int socketDescriptor_, QSharedPointer<ServerProtocols> protocols, QObject *parent)
    : QThread(parent),
      socketDescriptor_(socketDescriptor_),
	  pendingCommand_(""),
	  protocols_(protocols)
{
}

void ServerThread::run()
{
	QTcpSocket socket;

	tcpSocket_ = &socket;

	if (!socket.setSocketDescriptor(socketDescriptor_))
    {
        emit error(socket.error());
        return;
    }

	peerAddress_ = socket.peerAddress().toString();

	connect(tcpSocket_, SIGNAL(readyRead()), this, SLOT(readClient()), Qt::DirectConnection);
	connect(tcpSocket_, SIGNAL(disconnected()), this, SLOT(handleDroppedClient()), Qt::DirectConnection);

	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	timer_ = new QTimer();
	timer_->setInterval(timeoutInterval_);
	connect(timer_, SIGNAL(timeout()), this, SLOT(handleTimeout()), Qt::DirectConnection);
	timer_->start();

	//check for data as soon as the thread starts
	connect(this, SIGNAL(started()), this, SLOT(readClient()));

	QThread::exec();

	delete timer_;

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
		if(tcpSocket_->bytesAvailable() == 0 && !tcpSocket_->waitForReadyRead(timeoutInterval_))
		{
			return false;
		}
		else
		{
			remainingContentLength = command->addToContent(tcpSocket_->read(remainingContentLength));
		}
	}

	return true;
}

QSharedPointer<Picto::ProtocolResponse> ServerThread::processCommand(QSharedPointer<Picto::ProtocolCommand> _command)
{
	QSharedPointer<Picto::Protocol> protocol = protocols_->getProtocol(_command->getProtocolName());
	if(protocol.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> unknownProtocolResponse(
			new Picto::ProtocolResponse(Picto::Names->serverAppName,
										"PICTO",
										"1.0",
										Picto::ProtocolResponseType::BadRequest));

		return unknownProtocolResponse;
	}

	if(_command->isPendingContent())
	{
		QSharedPointer<Picto::ProtocolResponse> incompleteRequestResponse(
			new Picto::ProtocolResponse(Picto::Names->serverAppName,
										protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::IncompleteRequest));

		return incompleteRequestResponse;
	}

	if(!_command->isWellFormed())
	{
		QSharedPointer<Picto::ProtocolResponse> malformedRequestResponse(
			new Picto::ProtocolResponse(Picto::Names->serverAppName,
										protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::BadRequest));

		return malformedRequestResponse;
	}
	
	QSharedPointer<Picto::ProtocolCommandHandler> handler = protocol->getCommandHandler(_command->getMethod());
	if(handler.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> notImplementedResponse(
			new Picto::ProtocolResponse(Picto::Names->serverAppName,
										protocol->id(),
										protocol->version(),
										Picto::ProtocolResponseType::NotImplemented));

		return notImplementedResponse;
	}

	//Add the source ip to the command
	_command->setFieldValue("Source-Address", tcpSocket_->peerAddress().toString());
	
	QSharedPointer<Picto::ProtocolResponse> response = handler->processCommand(_command);

	//If this command has a Command-ID field, we need to include the same field in the
	//response (this allows the client to match up commands and responses).
	if(_command->hasField("Command-ID"))
		response->setFieldValue("Command-ID",_command->getFieldValue("Command-ID"));

	return response;
}

void ServerThread::deliverResponse(QSharedPointer<Picto::ProtocolResponse> response)
{
	if(response.isNull())
	{

		response = QSharedPointer<Picto::ProtocolResponse>(
			new Picto::ProtocolResponse(Picto::Names->serverAppName,
										"PICTO",
										"1.0",
										Picto::ProtocolResponseType::InternalServerError));
	}

	QDataStream os(tcpSocket_);

	if(response->getMultiPart() <= Picto::MultiPartResponseType::MultiPartInitial)
	{
		QString headers = response->generateHeadersString();
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
	timer_->stop();

	while(tcpSocket_->canReadLine())
	{
		QString currentLine = tcpSocket_->readLine();

		QRegExp newLineRegExp("[\r\n]+");

		if(newLineRegExp.exactMatch(currentLine))
		{
			QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(pendingCommand_));
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

				tcpSocket_->flush();
			} while(tcpSocket_->state() == QTcpSocket::ConnectedState &&
				    //tcpSocket_->waitForBytesWritten() &&
					response->shouldStream());

			pendingCommand_.clear();
		}
		else
		{
			pendingCommand_ += currentLine;
		}
	}

	timer_->start();
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

#include "serverthread.h"
#include "../../common/globals.h"

#include "../connections/ConnectionManager.h"

#include <QtNetwork>
#include <QDateTime>
#include "../../common/memleakdetect.h"

/*! \brief Constructs a ServerThread object.
 *	@param socketDescriptor_ This is used with QTcpSocket::setSocketDescriptor() to allow this socket to handle a new 
 *	connection that came into the Server's Server::incomingConnection() function.
 *	@param protocols A ServerProtocols object that includes a list of Protocol object that will handle incoming 
 *	messages.
 */
ServerThread::ServerThread(qintptr socketDescriptor_, QSharedPointer<ServerProtocols> protocols, QObject *parent)
    : QThread(parent),
      socketDescriptor_(socketDescriptor_),
	  pendingCommand_(""),
	  protocols_(protocols)
{
}

/*! \brief This is the main method of the ServerThread.  It sets up the ServerThread's variables, Signal/Slot connections and event timing, then
 *	calls QThread::exec() to start the thread's event loop.  This continues until the thread ends at which point its socket is disconnected from 
 *	its host.
 */
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
	connect(tcpSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)), Qt::DirectConnection);

	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	timer_ = new QTimer();
	timer_->setInterval(timeoutInterval_);
	connect(timer_, SIGNAL(timeout()), this, SLOT(handleTimeout()), Qt::DirectConnection);
	timer_->start();

	//check for data as soon as the thread starts
	connect(this, SIGNAL(started()), this, SLOT(readClient()));

	QStringList list1,list2;
	closedUnconfirmedCommandIDs_ = &list1;
	unconfirmedCommandIDs_ = &list2;

	QThread::exec();

	delete timer_;

	socket.disconnectFromHost();
	if(socket.state() != QTcpSocket::UnconnectedState)
	{
		socket.waitForDisconnected();
	}
}

/*! \brief Calling this causes all data from the QTcpSocket that belongs in the input command's content field to be read into it.
 *	\note This function blocks until all the content arrives.
 */
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

/*! \brief Processes the input ProtocolCommand using the correct Protocol object extracted from the ServerProtocols object.
 *	An appropriate ProtocolResponse is returned.
 */
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
	//if(_command->hasField("Command-ID"))
	//{
	//	pendingCommandIDs_.append(_command->getFieldValue("Command-ID"));
	//}
	//if(response->getRegisteredType() == Picto::RegisteredResponseType::Immediate)
	//{
	//	response->setFieldValue("Command-ID",pendingCommandIDs_.join(","));
	//	pendingCommandIDs_.clear();
	//}
	QStringList* buffer;
	int registeredType = response->getRegisteredType();
	if(registeredType & Picto::RegisteredResponseType::FirstInCommandPackage)
	{
		//If there's an old closed command ids package, clear it, it wasn't confirmed fast enough
		closedUnconfirmedCommandIDs_->clear();
		//Switch the closed and unclosed pointers.
		buffer = closedUnconfirmedCommandIDs_;
		closedUnconfirmedCommandIDs_ = unconfirmedCommandIDs_;
		unconfirmedCommandIDs_ = buffer;
	}
	if(registeredType & Picto::RegisteredResponseType::SendLastCommandPackage)
	{
		//Send out the latest closed command ids package.
		response->setFieldValue("Command-ID",closedUnconfirmedCommandIDs_->join(","));
		//Clear the closedunconfirmed list since we are now confirming it.
		closedUnconfirmedCommandIDs_->clear();
	}
	if(_command->hasField("Command-ID"))
	{
		//Add the command id of this command to a list of command ids to be confirmed later.
		unconfirmedCommandIDs_->append(_command->getFieldValue("Command-ID"));
	}
	if(response->getRegisteredType() == Picto::RegisteredResponseType::Immediate)
	{
		//Send responses to all commands that have arrived.
		(*closedUnconfirmedCommandIDs_) << (*unconfirmedCommandIDs_);
		response->setFieldValue("Command-ID",closedUnconfirmedCommandIDs_->join(","));
		//Clear the lists because we just confirmed all of them.
		closedUnconfirmedCommandIDs_->clear();
		unconfirmedCommandIDs_->clear();
	}
	response->setFieldValue("PictoVersion",PICTOVERSION);

	return response;
}

/*! \brief Writes the input ProtocolResponse out over the QTcpSocket.
*/
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

	response->setFieldValue("Server-Bytes",QString::number(tcpSocket_->bytesAvailable()));
	//qDebug("Server-Bytes: " + response->getFieldValue("Server-Bytes").toLatin1());
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

/*! \brief Called whenever new data arrives on the QTcpSocket.  Reads the data in, uses it to build the ProtocolCommand objects that were sent on the other
 *	end of the Socket, processes them with processCommand() and delivers the responses generated in that function back to the other end of the socket.
 */
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

/*! \brief Called when the QTcpSocket is disconnected.  Causes this ServerThread to exit().
 */
void ServerThread::handleDroppedClient()
{
	exit();
}

/*! \brief Called when nothing happens on the QTcpSocket for timeoutInterval_ (10) seconds.  Causes this ServerThread to exit().
*/
void ServerThread::handleTimeout()
{
	//QSharedPointer<Picto::ProtocolResponse> response = new Picto::ProtocolResponse(protocol->id(),protocol->version(),Picto::ProtocolResponseType::ConnectionTimedOut);
	//deliverResponse(response);
	exit();
}

/*! \brief Called when an error occurs on the QTcpSocket.  Currently, doesn't do anything about it.
*/
void ServerThread::handleError(QAbstractSocket::SocketError socketError)
{
	int i = socketError;
	i = i;
}

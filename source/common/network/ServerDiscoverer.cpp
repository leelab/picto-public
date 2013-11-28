#include <QStringList>
#include "ServerDiscoverer.h"
#include "../globals.h"
#include "../common.h"
#include "../memleakdetect.h"

namespace Picto {

ServerDiscoverer::ServerDiscoverer(QObject *parent) :
	pictoServerPort_(0),
	bFoundServer_(false),
	QObject(parent)
{
	QHostAddress serverAddress(QHostAddress::Any);
	
	port_ = Picto::portNums->getMinDiscoverPort();

    while(!udpSocket_.bind(serverAddress, port_) && port_ < Picto::portNums->getMaxDiscoverPort())
	{
		port_++;
	}

    connect(&udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	connect(&timeoutTimer_,SIGNAL(timeout()),this,SLOT(timeout()));
}

/*! \brief Returns the Picto Server's IP Address, if it has been discovered or an empty QHostAddress otherwise.*/
QHostAddress ServerDiscoverer::getAddress()
{
	if(!bFoundServer_)
	{
		QHostAddress emptyAddress;

		return emptyAddress;
	}
	else
	{
		return pictoServerAddress_;
	}
}
/*! \brief Returns the port on which communication to the Picto Server should occur, or zero otherwise.*/
quint16 ServerDiscoverer::getPort()
{
	if(!bFoundServer_)
	{
		return 0;
	}
	else
	{
		return pictoServerPort_;
	}
}

/*! \brief Begins the server disocver process.
 *	\details This function will return right away and requires the Qt event loop to be functioning to operate or for 
 *	waitForDiscovered() to be called (which creates an event loop internally).
 *
 *	If the server is discovered within the input timeout (in milliseconds), the foundServer() signal will be emitted.
 *	If the server is not discovered in time, discoverFailed() will be emitted.
 */
void ServerDiscoverer::discover(int timeout)
{
	bFoundServer_ = false;
	bRunning_ = true;

	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("DISCOVER %1 PICTO/1.0").arg(port_).toLatin1();

	timeoutTimer_.setInterval(timeout);
	timeoutTimer_.setSingleShot(true);

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, Picto::portNums->getServerPort());
		
	timeoutTimer_.start();
}

/*! \brief Called by a timeout timer if the Server isn't discovered within a set time of discover() begin called.  Emits discoverFailed().
*/
void ServerDiscoverer::timeout()
{
	emit discoverFailed();
	bRunning_ = false;
	waitingEventLoop_.exit();
}

/*! \brief Called when the udpSocket recieves a datagram.  
 *	\details Reads the data.  If the data is a server ip address/port report, the function extracts the relevant data and emits foundServer.S
 */
void ServerDiscoverer::processPendingDatagrams()
{
    while (udpSocket_.hasPendingDatagrams())
	{
		QByteArray datagram;

		datagram.resize(udpSocket_.pendingDatagramSize());
		udpSocket_.readDatagram(datagram.data(), datagram.size());

		QString request = datagram.data();

		QString method, targetAddress, targetPort, protocolName, protocolVersion;

		QStringList tokens = request.split(QRegExp("[ ][ ]*"));
		if(tokens.count() == 3)
		{
			method = tokens[0];

			int targetPortPosition = tokens[1].indexOf(':');
			if(targetPortPosition != -1)
			{
				targetAddress = tokens[1].left(targetPortPosition);
				pictoServerAddress_.setAddress(targetAddress);
				targetPort = tokens[1].mid(targetPortPosition+1);
				pictoServerPort_ = targetPort.toInt();
			}

			int protocolVersionPosition = tokens[2].indexOf('/');
			if(protocolVersionPosition != -1)
			{
				protocolName = tokens[2].left(protocolVersionPosition);
				protocolVersion = tokens[2].mid(protocolVersionPosition+1);
			}

			if(method == "ANNOUNCE" &&
			   !pictoServerAddress_.isNull() &&
			   pictoServerPort_ &&
			   protocolName == "PICTO")
			{
				bFoundServer_ = true;				
				timeoutTimer_.stop();
				bRunning_ = false;
				emit foundServer(pictoServerAddress_, pictoServerPort_);
				waitingEventLoop_.exit();
			}
		}
	}
}

/*! \brief If this object is not running within a Qt Event Loop, this function may be used to create an event loop internally and allow this 
 *	server discoverer to operate.
 *	\details The input timeout is time that will pass in this function before it exits.  If used with the discover() function, the procedure
 *	would be to connect to the foundServer() and discoverFailed() signals, then call discover() with a timeout input, then call waitForDiscovered() with
 *	the same timeout input or higher.
 */
bool ServerDiscoverer::waitForDiscovered(int timeout)
{
	if(bRunning_)
	{
		QTimer waitTimeout;
		connect(&waitTimeout,SIGNAL(timeout()),&waitingEventLoop_,SLOT(quit()));
		waitTimeout.setInterval(timeout);
		waitTimeout.setSingleShot(true);
		waitTimeout.start();
		waitingEventLoop_.exec();
		waitTimeout.stop();
	}

	return bFoundServer_;
}

}; //namespace Picto

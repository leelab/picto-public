#ifndef _SERVERDISCOVERER_H_
#define _SERVERDISCOVERER_H_

#include <QUdpSocket>
#include <QThread>
#include <QTimer>
#include <QEventLoop>

#include "../common.h"

namespace Picto {

/*!	\brief Used to find the Picto server instance on the local network.
 *
 *	All Picto apps (except for Server) communicate with the server.  For this to
 *	occur, they need the IP address of the server and a communication port value.  This is something that we 
 *	could require the user to enter, but it is much more user-friendly to automate 
 *	the process.  The server discoverer object sends out a UDP broadcast, and then
 *	waits for a response from a server on the network with the requested information.
 *
 *	This object was not designed for use on a network with multiple servers, so its
 *	behavior in such a scenario is undefined. (If it still functions correctly, it will find *a* server, 
 *	but which one will be a function of the server load and network conditions.)
 *
 *	\note Much of the work of discovering the server has been moved into the CommandChannel and happens automatically.
 *	This still appears to be used by some sections of the Picto code though.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
struct PICTOLIB_API ServerDiscoverer : QObject
#else
struct ServerDiscoverer : QObject
#endif
{
	Q_OBJECT

public:
	ServerDiscoverer(QObject *parent = 0);
	bool waitForDiscovered(int timeout = 30000);
	QHostAddress getAddress();
	quint16 getPort();

public slots:
	void discover(int timeout = 5000);

signals:
	/*! \brief Emitted when a server has been discovered.  
	 *	\details pictoServerAddress is the server IP Address, pictoServerPort is the port on which communication should occur.
	 */
	void foundServer(QHostAddress pictoServerAddress, quint16 pictoServerPort);
	/*! \brief Emitted when server discovery fails within the timeout entered in a call to discover().
	*/
	void discoverFailed();

private:
	QUdpSocket udpSocket_;
	quint16 port_;
	QHostAddress pictoServerAddress_;
	quint16 pictoServerPort_;

	QTimer timeoutTimer_;
	QEventLoop waitingEventLoop_;

	bool bFoundServer_;
	bool bRunning_;

private slots:
    void processPendingDatagrams();
	void timeout();
};

} //namespace Picto

#endif

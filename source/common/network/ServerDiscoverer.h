#ifndef _SERVERDISCOVERER_H_
#define _SERVERDISCOVERER_H_

#include <QUdpSocket>
#include <QThread>
#include <QTimer>
#include <QEventLoop>

#include "../common.h"

namespace Picto {

/*!	\brief An object to find the Picto server instance on the local network.
 *
 *	All Picto apps (except for Server) communicate with the server.  For this to
 *	occur, they need to know the address of the server.  This is something that we 
 *	could require the user to enter, but it is much more user-friendly to automate 
 *	the process.  The server discoverer object sends out a UDP broadcast, and then
 *	waits for a response from a server on the network.
 *
 *	This object was not designed for use on a network with multiple servers, so its
 *	behavior in such a scenario is undefined. (I believe it will find *a* server, 
 *	but it is undefined as to which server it will find.)
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
	void foundServer(QHostAddress pictoServerAddress, quint16 pictoServerPort);
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

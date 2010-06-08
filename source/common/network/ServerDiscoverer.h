#ifndef _SERVERDISCOVERER_H_
#define _SERVERDISCOVERER_H_

#include "../common.h"

#include <QUdpSocket>
#include <QThread>
#include <QTimer>
#include <QEventLoop>

namespace Picto {

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
	void discover(int timeout = 1000);

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

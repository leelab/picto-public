#ifndef _SERVERTHREAD_H_
#define _SERVERTHREAD_H_

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include <QSharedPointer>

#include "../common/common.h"
#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include "../protocol/ProxyServerProtocols.h"

/*!	\brief The thread spun up by the server to handle each connection
 *
 *	Much like the server itself, this thread was stolen from the PictoServer
 *	code.  And we are now stuck maintinaing two nearly identical pieces of code...
 *	And again, sorry.
 */
class ProxyServerThread : public QThread
{
    Q_OBJECT

public:
	ProxyServerThread(qintptr socketDescriptor, QSharedPointer<ProxyServerProtocols> _protocol, QObject *parent);

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);
	void activity();

private:
	bool receiveContent(QSharedPointer<Picto::ProtocolCommand> command);
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand> _command);
	void deliverResponse(QSharedPointer<Picto::ProtocolResponse> response);

	qintptr socketDescriptor;
	QString pendingCommand;
	QTcpSocket * tcpSocket;
	QSharedPointer<ProxyServerProtocols> protocols;

	static const int timeoutInterval = 2000;

	QTimer * timer;

private slots:
	void readClient();
	void handleDroppedClient();
	void handleTimeout();
};

#endif

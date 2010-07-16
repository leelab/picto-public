#ifndef _SERVERTHREAD_H_
#define _SERVERTHREAD_H_

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include <QSharedPointer>
#include <QUuid>

#include "../common/common.h"
#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include "../protocol/ServerProtocols.h"

class ServerThread : public QThread
{
    Q_OBJECT

public:
	ServerThread(int socketDescriptor, QSharedPointer<ServerProtocols> _protocol, QObject *parent);

	QString getPeerAddress() { return peerAddress_; };
	//void setSessionId(QUuid sessionId) { sessionId_ = sessionId; };

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
	bool receiveContent(QSharedPointer<Picto::ProtocolCommand> command);
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand> _command);
	void deliverResponse(QSharedPointer<Picto::ProtocolResponse> response);

	int socketDescriptor_;
	QString pendingCommand_;
	QTcpSocket * tcpSocket_;
	QSharedPointer<ServerProtocols> protocols_;
	QString peerAddress_;

	static const int timeoutInterval_ = 10000;

	QTimer * timer_;

	//QUuid sessionId_;

private slots:
	void readClient();
	void handleDroppedClient();
	void handleTimeout();
};

#endif

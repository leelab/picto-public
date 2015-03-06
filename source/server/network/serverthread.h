#ifndef _SERVERTHREAD_H_
#define _SERVERTHREAD_H_

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include <QSharedPointer>
#include <QUuid>
#include <QTime>
#include <QStringList>

#include "../common/common.h"
#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include "../protocol/ServerProtocols.h"

/*!	\brief A thread used to handle TCP connections.
 *
 *	\details When a TCp connection is created by the server, a new ServerThread is spun
 *	up to handle the incoming data.  The thread reads the incoming data as if it is
 *	following HTTP protocol.  It then finds the appropriate command handler (using the 
 *	protocol from ServerProtocols and the command name) and calls that command handler for a response
 *	which is then returned to the sender.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ServerThread : public QThread
{
    Q_OBJECT

public:
	ServerThread(qintptr socketDescriptor, QSharedPointer<ServerProtocols> _protocol, QObject *parent);

	/*! \brief Returns the IP Address of the thing on the other end of the QTcpSocket in string form.
	 */
	QString getPeerAddress() { return peerAddress_; };

    void run();

signals:
	/*! \brief Emitted if there is something wrong with the socketDesriptor input into the constructor at the time
	 *	that run() is called.
	 */
    void error(QTcpSocket::SocketError socketError);

private:
	bool receiveContent(QSharedPointer<Picto::ProtocolCommand> command);
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand> _command);
	void deliverResponse(QSharedPointer<Picto::ProtocolResponse> response);

	qintptr socketDescriptor_;
	QString pendingCommand_;
	QTcpSocket * tcpSocket_;
	QSharedPointer<ServerProtocols> protocols_;
	QString peerAddress_;
	QStringList* closedUnconfirmedCommandIDs_;
	QStringList* unconfirmedCommandIDs_;

	static const int timeoutInterval_ = 10000;

	QTimer * timer_;

	//QUuid sessionId_;

private slots:
	void readClient();
	void handleDroppedClient();
	void handleTimeout();
	void handleError(QAbstractSocket::SocketError socketError);
};

#endif

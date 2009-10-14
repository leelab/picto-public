/*! This thread collects data from an individual neural
 *  data collection device (eg Plexon, or TDT) using the proxy
 * servers and the GET ACQ commands
 */

#ifndef NEURALCOLLECTORTHREAD_H
#define NEURALCOLLECTORTHREAD_H

#include <QThread>
#include <QString>
#include <QHostAddress>
#include <QTimer>
#include <QTcpSocket>
#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"


class NeuralCollectorThread : public QThread
{
	Q_OBJECT

public:
	NeuralCollectorThread(QString name, QHostAddress address, QObject *parent);

	void run();
	void stop();
private slots:
	void collectData();
	void timeoutHandler();
private:
	QString proxyName;
	QHostAddress proxyAddress;
	QTcpSocket *proxySocket;

	QTimer *collectionTimer;
	QTimer *timeoutTimer;

	Picto::ProtocolCommand *getCommand;
	Picto::ProtocolResponse *proxyResponse;


};



#endif


/*! This handles collecting data from a nerual data source.
 *  Each neural data source will be running a proxy server, that
 *  we can connect to for the data.
 */
#ifndef NEURALDATACOLLECTOR_H
#define NEURALDATACOLLECTOR_H

#include <QHostAddress>
#include <QTimer>
#include <QTcpSocket>
#include <QThread>
#include <QSqlDatabase>

#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"
#include "alignmenttool.h"

class NeuralDataCollector : public QThread
{
	Q_OBJECT

public:
	NeuralDataCollector(QHostAddress proxyAddress, int proxyPort, QSqlDatabase &sessionDb, QObject *parent, int interval=1000);
	~NeuralDataCollector();

	void setCollectionInterval(int interval) { collectionInterval = interval;};
	int getCollectionInterval() { return collectionInterval; };

	void setAlignmentTool(AlignmentTool *a) { align = a; };
	AlignmentTool* getAlignmentTool() { return align; };

	void run();
	void stop();

private slots:
	void collectData();
private:
	void parseResponse(Picto::ProtocolResponse *proxyResponse);

	AlignmentTool *align;

	int collectionInterval;
	QTimer *pollingTimer;

	QHostAddress proxyAddress;
	int proxyPort;
	QTcpSocket *proxySocket;

	QSqlDatabase db;
};




#endif


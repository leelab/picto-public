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
#include "../common/network/CommandChannel.h"
#include "alignmenttool.h"

class NeuralDataCollector : public QThread
{
	Q_OBJECT

public:
	NeuralDataCollector(int proxyId, QString sessionDbName, int interval=1000, QObject *parent=0);
	~NeuralDataCollector() { printf("~NeuralDataCollector\n"); };

	void setCollectionInterval(int interval) { collectionInterval_ = interval;};
	int getCollectionInterval() { return collectionInterval_; };

	//void setAlignmentTool(AlignmentTool *a) { align = a; };
	//AlignmentTool* getAlignmentTool() { return align; };

	void run();
	void stop();

private slots:
	void collectData();
private:
	void parseResponse(QSharedPointer<Picto::ProtocolResponse> proxyResponse);

	//AlignmentTool *align;

	int collectionInterval_;
	QTimer *pollingTimer_;

	Picto::CommandChannel *cmdChannel_;
	QHostAddress proxyAddress_;
	int proxyPort_;
	//QTcpSocket *proxySocket_;

	QString sessionDbName_;
	QSqlDatabase db_;
};




#endif


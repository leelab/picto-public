/*! This handles collecting data from each nerual data source.
 *  Each neural data source will be running a proxy server, that
 *  we can connect to for the data.
 */

#ifndef NEURALDATACOLLECTOR_H
#define NEURALDATACOLLECTOR_H

#include <QTimer>
#include <QSharedPointer>
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>

#include "../common/common.h"
#include "neuralcollectorthread.h"

class NeuralDataCollector : QObject
{
	Q_OBJECT

public:
	NeuralDataCollector();
	~NeuralDataCollector();

private slots:
	void processPendingDatagrams();
	void cleanupThreads();
private:	
	typedef struct proxyServerInfo_st
	{
		QString name;
		QHostAddress address;
		NeuralCollectorThread *collectorThread;
	} proxyServerInfo;

	QSharedPointer<QTimer> timer;
	QList<proxyServerInfo> proxyServerList;
	QUdpSocket udpSocket_;
};

#endif
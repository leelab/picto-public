#include "neuralcollectorthread.h"

#include <QDebug>


NeuralCollectorThread::NeuralCollectorThread(QString name, QHostAddress address, QObject *parent)
  : QThread(parent),
	proxyName(name),
	proxyAddress(address)
{
}

void NeuralCollectorThread::run()
{
	//set up the reccuring collection timer
	collectionTimer = new QTimer;
	collectionTimer->setInterval(1000);
	connect(collectionTimer,SIGNAL(timeout()), this, SLOT(collectData()),Qt::DirectConnection);
	collectionTimer->start();

	//set up the timeout timer
	timeoutTimer = new QTimer;
	timeoutTimer->setInterval(20000);
	connect(timeoutTimer,SIGNAL(timeout()), this, SLOT(timeoutHandler()),Qt::DirectConnection);
	timeoutTimer->start();

	//set up the network connection
	proxySocket = new QTcpSocket;;
	proxySocket->connectToHost(proxyAddress,42420,QIODevice::ReadWrite);

	getCommand = new Picto::ProtocolCommand("GET /data ACQ/1.0");
	QThread::exec();



}

void NeuralCollectorThread::stop()
{
	exit();
}

void NeuralCollectorThread::timeoutHandler()
{
	exit();
}

void NeuralCollectorThread::collectData()
{
	int bytesRead;

	
	/*! /todo Figure out a way to handle failed connections */
	if(proxySocket->state()<QAbstractSocket::ConnectingState)
		proxySocket->connectToHost(proxyAddress,42420,QIODevice::ReadWrite);
	if(!proxySocket->waitForConnected(2000))
		return;

	getCommand->write(proxySocket);

	proxyResponse = new Picto::ProtocolResponse();
		
	bytesRead = proxyResponse->read(proxySocket);

	if(bytesRead == -1)
		qDebug()<<proxyName<<": No data returned from proxy";
	else if(bytesRead == -2)
		qDebug()<<proxyName<<": Poorly formed headers";
	else if(bytesRead<0)
	{
		/*! \todo I need to make sure that in the caase, the data received is still recordered */
		qDebug()<<proxyName<<": Unable to read entire content block";
	}
	else
	{
		//reset the timeout timer
		timeoutTimer->start();
		qDebug()<<proxyName<<": Read "<<bytesRead<<" bytes";
	}
}


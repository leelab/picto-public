#include "neuraldatacollector.h"
#include <QStringList>

NeuralDataCollector::NeuralDataCollector()
{
	//Listen for broadcasts of new proxy servers
	udpSocket_.bind(42420);
	connect(&udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	QTimer *cleanupTimer = new QTimer;
	cleanupTimer->setInterval(30000);
	connect(cleanupTimer, SIGNAL(timeout()),this,SLOT(cleanupThreads()));
	cleanupTimer->start();
}

NeuralDataCollector::~NeuralDataCollector()
{
	return;
}

//Every 30 seconds, we will run through the list of threads.  
//If any of them have terminated, we'll remove them from our list
void NeuralDataCollector::cleanupThreads()
{
	QList<proxyServerInfo>::iterator i = proxyServerList.begin();
	while(i != proxyServerList.end())
	{
		if(i->collectorThread->isFinished())
			proxyServerList.erase(i);
		i++;
	}
	
}


//Add new proxy servers to the list.
void NeuralDataCollector::processPendingDatagrams()
{
    while (udpSocket_.hasPendingDatagrams())
	{
		QByteArray datagram;
		QHostAddress senderAddress;

		datagram.resize(udpSocket_.pendingDatagramSize());
		udpSocket_.readDatagram(datagram.data(), datagram.size(), &senderAddress);

		QString request = datagram.data();

		QString method, senderName, protocolName, protocolVersion;

		QStringList tokens = request.split(QRegExp("[ ][ ]*"));
		if(tokens.count() == 3)
		{
			method = tokens[0];
			senderName = tokens[1];

			int protocolVersionPosition = tokens[2].indexOf('/');
			if(protocolVersionPosition != -1)
			{
				protocolName = tokens[2].left(protocolVersionPosition);
				protocolVersion = tokens[2].mid(protocolVersionPosition+1);
			}

		
			/*! /TODO Figure out how to have proxies fall off the list 
			 *  As they get old and out of date.
			 */
			if(method == "ANNOUNCE" && protocolName == "ACQ")
			{
				proxyServerInfo newServer;
				newServer.address = senderAddress;
				newServer.name = senderName;

				//check to see if the proxy server name and adress is already 
				//on the list.  If so stop and erase it`.
				QList<proxyServerInfo>::iterator i;
				for(i=proxyServerList.begin(); i!=proxyServerList.end(); i++)
				{
					if(i->address == senderAddress && i->name == senderName)
					{
						i->collectorThread->stop();
						proxyServerList.erase(i);
						break;
					}
				}
				newServer.collectorThread = new NeuralCollectorThread(newServer.name,newServer.address,this);
				newServer.collectorThread->start();
				proxyServerList.push_back(newServer);
			}
			if(method == "DEPART" && protocolName == "ACQ")
			{
				//remove the proxy from the list
				QList<proxyServerInfo>::iterator i;
				for(i=proxyServerList.begin(); i!=proxyServerList.end(); i++)
				{
					if(i->address == senderAddress && i->name == senderName)
					{
						i->collectorThread->stop();
						proxyServerList.erase(i);
						break;
					}
				}
			}

		}
	}

}

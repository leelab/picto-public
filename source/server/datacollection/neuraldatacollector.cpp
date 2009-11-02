#include <QTimer>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QSqlQuery>
#include <QVariant>

#include "neuraldatacollector.h"


NeuralDataCollector::NeuralDataCollector(QHostAddress proxyAddress, int proxyPort, 
							QSqlDatabase &sessionDb, QObject *parent, int interval):
		QThread(parent),
		proxyAddress(proxyAddress),
		proxyPort(proxyPort),
		collectionInterval(interval),
		db(sessionDb)
{
}

NeuralDataCollector::~NeuralDataCollector()
{
	exit();
	return;
}

void NeuralDataCollector::run()
{
	//set up a timer to grab proxy data
	pollingTimer = new QTimer();
	pollingTimer->setInterval(collectionInterval);
	connect(pollingTimer, SIGNAL(timeout()), this, SLOT(collectData()),Qt::DirectConnection);
	pollingTimer->start();

	//set up the network connection
	proxySocket = new QTcpSocket;;
	proxySocket->connectToHost(proxyAddress,proxyPort,QIODevice::ReadWrite);

	QThread::exec();

}

void NeuralDataCollector::stop()
{
	exit();
	return;
}

void NeuralDataCollector::collectData()
{
	pollingTimer->stop();

	//check for failed connections
	if(proxySocket->state()<QAbstractSocket::ConnectingState)
		proxySocket->connectToHost(proxyAddress,proxyPort,QIODevice::ReadWrite);
	if(!proxySocket->waitForConnected(2000))
		return;

	Picto::ProtocolCommand *getCommand;
	getCommand = new Picto::ProtocolCommand("GET /data ACQ/1.0");
	getCommand->write(proxySocket);

	Picto::ProtocolResponse *proxyResponse = new Picto::ProtocolResponse();
	int bytesRead = proxyResponse->read(proxySocket);

	if(bytesRead == -1)
		qDebug()<<proxyAddress.toString()<<": No data returned from proxy";
	else if(bytesRead == -2)
		qDebug()<<proxyAddress.toString()<<": Poorly formed headers";
	else if(bytesRead<0)
	{
		/*! \todo I need to make sure that in this case, the data received is still recorded */
		qDebug()<<proxyAddress.toString()<<": Unable to read entire content block";
	}
	else
	{
		parseResponse(proxyResponse);
		qDebug()<<proxyAddress.toString()<<": Read "<<bytesRead<<" bytes";
	}

	//flush out any remaining data in the conenction
	proxySocket->readAll();

	pollingTimer->start();


}

//ParseResponse takes the XML fragment from the proxy server and does the following:
// 1. Spike information is stored in the spikes table in the session databasae
// 2. "External events" (which are just alignment codes) are sent to the alignment tool

void NeuralDataCollector::parseResponse(Picto::ProtocolResponse *response)
{
	QByteArray xmlData = response->getDecodedContent();

	QString timestamp,prealigntime;
	QString channel,unit,waveform,device,deviceStatus;
	int eventcode;

	QSqlQuery query(db);
	db.transaction();

	QXmlStreamReader reader(xmlData);

	reader.readNext();
	while(!reader.atEnd())
	{
		if(reader.isStartElement() && reader.name() == "event")
		{
			timestamp = 0.0;
			channel.clear();
			unit.clear();
			waveform.clear();
			eventcode = 0;

			if(reader.attributes().value("type").toString() == "spike")
			{
				while(!reader.isEndElement())
				{
					if(reader.isStartElement() && reader.name() == "timestamp")
					{
						prealigntime = reader.readElementText();
					}
					else if(reader.isStartElement() && reader.name() == "channel")
					{
						channel = reader.readElementText();
					}
					else if(reader.isStartElement() && reader.name() == "unit")
					{
						unit = reader.readElementText();
					}
					else if(reader.isStartElement() && reader.name() == "wave")
					{
						waveform = reader.readElementText();
					}
					else
					{
					}
					reader.readNext();
				}

				//use the alignment tool to 
				timestamp = prealigntime;

				query.prepare("INSERT INTO spikes (timestamp, prealigntime, channel, unit, waveform)"
					"VALUES(:timestamp, :prealigntime, :channel, :unit, :waveform)");
				query.bindValue(":timestamp", timestamp);
				query.bindValue(":prealigntime", prealigntime);
				query.bindValue(":channel", channel);
				query.bindValue(":unit", unit);
				query.bindValue(":waveform", waveform);
				query.exec();
			}
			else if(reader.attributes().value("type").toString() == "externalEvent")
			{
				while(!reader.isEndElement())
				{
					if(reader.isStartElement() && reader.name() == "timestamp")
					{
						timestamp = reader.readElementText();
					}
					else if(reader.isStartElement() && reader.name() == "eventCode")
					{
						eventcode = reader.readElementText().toInt();
					}
					else
					{
					}
					reader.readNext();
				}

				//Use the event codes to update the alignment tool
				align->addEvent(eventcode,timestamp.toDouble(), Align::neural); 
			}
		}
		else
		{
			//do nothing?
		}
		reader.readNext();

		if(reader.hasError())
			qDebug()<<"Error: "<<reader.errorString();

	}

	db.commit();


}

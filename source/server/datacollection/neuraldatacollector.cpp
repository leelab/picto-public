#include <QTimer>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QSqlQuery>
#include <QVariant>

#include "neuraldatacollector.h"


NeuralDataCollector::NeuralDataCollector(QSqlDatabase &sessionDb, int interval, QObject *parent):
		QThread(parent),
		collectionInterval_(interval),
		db_(sessionDb)
{
	//grab the proxy server port and and adression from the sessoin db_
	QSqlQuery query(sessionDb);
	query.exec("SELECT value FROM sessioninfo WHERE key='Proxy Adress'");
	Q_ASSERT(query.next());
	proxyAddress_ = QHostAddress(query.value(0).toString());
	query.exec("SELECT value FROM sessioninfo WHERE key='Proxy Port'");
	Q_ASSERT(query.next());
	proxyPort_ = query.value(0).toInt();
}

NeuralDataCollector::~NeuralDataCollector()
{
	exit();
	return;
}

void NeuralDataCollector::run()
{
	//set up a timer to grab proxy data
	pollingTimer_ = new QTimer();
	pollingTimer_->setInterval(collectionInterval_);
	connect(pollingTimer_, SIGNAL(timeout()), this, SLOT(collectData()),Qt::DirectConnection);
	pollingTimer_->start();

	//set up the network connection
	proxySocket_ = new QTcpSocket;;
	proxySocket_->connectToHost(proxyAddress_,proxyPort_,QIODevice::ReadWrite);

	QThread::exec();

}

void NeuralDataCollector::stop()
{
	exit();
	return;
}

void NeuralDataCollector::collectData()
{
	pollingTimer_->stop();

	//check for failed connections
	if(proxySocket_->state()<QAbstractSocket::ConnectingState)
		proxySocket_->connectToHost(proxyAddress_,proxyPort_,QIODevice::ReadWrite);
	if(!proxySocket_->waitForConnected(2000))
		return;

	Picto::ProtocolCommand *getCommand;
	getCommand = new Picto::ProtocolCommand("GET /data ACQ/1.0");
	getCommand->write(proxySocket_);

	Picto::ProtocolResponse *proxyResponse = new Picto::ProtocolResponse();
	int bytesRead = proxyResponse->read(proxySocket_);

	if(bytesRead == -1)
		qDebug()<<proxyAddress_.toString()<<": No data returned from proxy";
	else if(bytesRead == -2)
		qDebug()<<proxyAddress_.toString()<<": Poorly formed headers";
	else if(bytesRead<0)
	{
		/*! \todo I need to make sure that in this case, the data received is still recorded */
		qDebug()<<proxyAddress_.toString()<<": Unable to read entire content block";
	}
	else
	{
		parseResponse(proxyResponse);
		qDebug()<<proxyAddress_.toString()<<": Read "<<bytesRead<<" bytes";
	}

	//flush out any remaining data in the conenction
	proxySocket_->readAll();

	pollingTimer_->start();


}

//ParseResponse takes the XML fragment from the proxy server and does the following:
// 1. Spike information is stored in the spikes table in the session databasae
// 2. "External events" (which are just alignment codes) are sent to the alignment tool

void NeuralDataCollector::parseResponse(Picto::ProtocolResponse *response)
{
	QByteArray xmlData = response->getDecodedContent();

	double timestamp;
	double fittedTime;
	double correlation;
	QString channel,unit,waveform,device,deviceStatus;
	int eventcode;

	QSqlQuery query(db_);
	db_.transaction();

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
						timestamp = reader.readElementText().toDouble();
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

				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//! \todo use the alignment tool to fit the time
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				fittedTime = timestamp;
				correlation = 0.0;

				query.prepare("INSERT INTO spikes (timestamp, fittedtime, correlation, channel, unit, waveform)"
					"VALUES(:timestamp, :fittedtime, :correlation, :channel, :unit, :waveform)");
				query.bindValue(":timestamp", timestamp);
				query.bindValue(":fittedtime", fittedTime);
				query.bindValue(":correlation", correlation);
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
						timestamp = reader.readElementText().toDouble();
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

				//Add the event code to the neuraltrials table
				query.prepare("INSERT INTO neuraltrials (timestamp, aligncode, matched)"
					"VALUES(:timestamp, :aligncode, 0)");
				query.bindValue(":timestamp", timestamp);
				query.bindValue(":aligncode", eventcode);
				query.exec();

				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//!  \todo Rerun the alignment tool with our new data
				//
				//   Actually, wait until after we've commited to the db...
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

	db_.commit();


}

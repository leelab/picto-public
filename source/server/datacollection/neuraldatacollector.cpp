#include <QTimer>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QUuid>

#include "neuraldatacollector.h"
#include "../connections/ServerConfig.h"

NeuralDataCollector::NeuralDataCollector(int proxyId, QString sessionDbName, int interval, QObject *parent):
		QThread(parent),
		collectionInterval_(interval),
		sessionDbName_(sessionDbName)
{
	ServerConfig config;
	proxyAddress_ = config.proxyServerAddress(proxyId);
	proxyPort_ = config.proxyServerPort(proxyId);
	timestampsAligned_ = false;
}


void NeuralDataCollector::run()
{
	//cmdChannel_ = QSharedPointer<Picto::CommandChannel>(new Picto::CommandChannel(proxyAddress_,proxyPort_));//!!HERE!!
	cmdChannel_ = new Picto::CommandChannel(QUuid(),"SERVER",proxyAddress_,proxyPort_);//Since we will be changing the proxy server to a client, we just put in an empty Uuid for now

	//set up a timer to grab proxy data
	pollingTimer_ = new QTimer();
	pollingTimer_->setInterval(collectionInterval_);
	connect(pollingTimer_, SIGNAL(timeout()), this, SLOT(collectData()),Qt::DirectConnection);
	pollingTimer_->start();

	if(!cmdChannel_->isConnected())
		cmdChannel_->connectToServer(proxyAddress_,proxyPort_);

	//Open the database connection
	//This must be done here, since database connections aren't happy across threads	
	db_ = QSqlDatabase::addDatabase("QSQLITE",QString("SessionDb%1").arg((int)currentThreadId()));
	db_.setDatabaseName(sessionDbName_);
	Q_ASSERT(db_.open());

	QThread::exec();

	delete pollingTimer_;
	delete cmdChannel_;

}

void NeuralDataCollector::stop()
{
	if(isRunning())
		exit();
	return;
}

void NeuralDataCollector::collectData()
{
	pollingTimer_->stop();

	//check for failed connections
	if(!cmdChannel_->isConnected())
	{
		qDebug()<<"Command channel not connected.  Attempting to reconnect";
		cmdChannel_->connectToServer(proxyAddress_,proxyPort_);
	}

	QSharedPointer<Picto::ProtocolCommand> getCommand = QSharedPointer<Picto::ProtocolCommand>(new Picto::ProtocolCommand("GET /data ACQ/1.0"));
	cmdChannel_->sendCommand(getCommand);

	QSharedPointer<Picto::ProtocolResponse> proxyResponse;
	if(!cmdChannel_->waitForResponse(1000))
		qDebug()<<proxyAddress_.toString()<<": No data returned from proxy";
	
	proxyResponse = cmdChannel_->getResponse();

	//! \TODO Figure out how to handle missed responses from the proxy server.  We should probably \
	//!		  come up with a way to notify the workstation...
	if(proxyResponse)
		parseResponse(proxyResponse);

	pollingTimer_->start();


}

//ParseResponse takes the XML fragment from the proxy server and does the following:
// 1. Spike information is stored in the spikes table in the session databasae
// 2. "External events" (which are just alignment codes) are sent to the alignment tool

void NeuralDataCollector::parseResponse(QSharedPointer<Picto::ProtocolResponse> response)
{
	QByteArray xmlData = response->getDecodedContent();

	double timestamp;
	double fittedTime;
	double correlation;
	QString channel,unit,waveform,device,deviceStatus;
	int eventcode = -1;

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
			eventcode = -1;

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

				//! Use the alignment tool to fit the time
				fittedTime = align->convertToBehavioralTimebase(timestamp);
				correlation = align->getCorrelationCoefficient();

				//! \TODO move this code to sessioninfo
				query.prepare("INSERT INTO spikes (timestamp, fittedtime, correlation, channel, unit, waveform) "
					"VALUES(:timestamp, :fittedtime, :correlation, :channel, :unit, :waveform)");
				query.bindValue(":timestamp", timestamp);
				query.bindValue(":fittedtime", fittedTime);
				query.bindValue(":correlation", correlation);
				query.bindValue(":channel", channel);
				query.bindValue(":unit", unit);
				query.bindValue(":waveform", waveform);
				Q_ASSERT_X(query.exec(),"",query.lastError().text().toAscii());
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
	if(eventcode >= 0)
	{
		align->doIncrementalAlignment(db_);
		
		// alignment isn't calculated until the end of the first trial, meaning that all spikes in the first trial are unaligned.
		// once the first alignment values are calculated, go back and update the fittedtimes of neural trials that weren't yet aligned.
		if(!timestampsAligned_ && (align->getCorrelationCoefficient() != 0) )
		{	
			timestampsAligned_ = true;
			QSqlQuery unalignedSpikes(query);
			timestampsAligned_ &= unalignedSpikes.exec("SELECT id, timestamp FROM spikes WHERE fittedtime<0");
			while(unalignedSpikes.next())
			{
				query.prepare("UPDATE spikes SET fittedtime=:fittedtime, correlation=:correlation WHERE id=:id");
				query.bindValue(":id",unalignedSpikes.value(0).toInt());
				query.bindValue(":fittedtime",align->convertToBehavioralTimebase(unalignedSpikes.value(1).toDouble()));
				query.bindValue(":correlation",align->getCorrelationCoefficient() );
				timestampsAligned_ &= query.exec();
			}
		}
	}


}

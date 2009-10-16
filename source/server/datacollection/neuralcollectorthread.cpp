#include "neuralcollectorthread.h"

#include <QDebug>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QVariant>
#include <QSqlError>


NeuralCollectorThread::NeuralCollectorThread(QString name, QHostAddress address, QObject *parent, int interval)
  : QThread(parent),
	proxyName(name),
	proxyAddress(address),
	collectionInterval(interval)
{
}

void NeuralCollectorThread::run()
{
	//set up the database
	if(!initDatabase())
		return;

	//set up the reccuring collection timer
	collectionTimer = new QTimer;
	collectionTimer->setInterval(collectionInterval);
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

	firstGet = true;

	QThread::exec();



}

void NeuralCollectorThread::stop()
{
	QDateTime currDateTime;
	currDateTime = QDateTime::currentDateTime();

	QSqlQuery query(sessionDb);
	sessionDb.transaction();
	query.prepare("UPDATE sessionInfo SET endDate = :endDate");
	query.bindValue(":endDate",currDateTime.toString("MMM d, yyyy"));
	query.exec();
	query.prepare("UPDATE sessionInfo SET endTime = :endTime");
	query.bindValue(":endTime",currDateTime.toString("h:mm:ss"));
	query.exec();
	sessionDb.commit();

	sessionDb.close();
	exit();
}

void NeuralCollectorThread::timeoutHandler()
{
	stop();
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
		/*! \todo I need to make sure that in this case, the data received is still recorded */
		qDebug()<<proxyName<<": Unable to read entire content block";
	}
	else
	{
		//reset the timeout timer
		timeoutTimer->stop();
		parseResponse();
		timeoutTimer->start();
		qDebug()<<proxyName<<": Read "<<bytesRead<<" bytes";
	}
}

bool NeuralCollectorThread::initDatabase()
{
	bool result;

	QDateTime currDateTime;
	currDateTime = QDateTime::currentDateTime();
	
	sessionDb = QSqlDatabase::addDatabase("QSQLITE",proxyName + currDateTime.toString("yyyyMMdd_hhmmss"));
	sessionDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/neuraldata/" + proxyName + "_" + currDateTime.toString("yyyyMMdd_hhmmss"));
	result = sessionDb.open();
	if(!result)
	{
		qDebug()<<"Failed to open database";
		qDebug()<<"Error: "<<sessionDb.lastError().text();
		return false;
	}

	QSqlQuery query(sessionDb);
	query.exec("create table events (timestamp REAL, eventcode INTEGER)");
	query.exec("create table spikes (timestamp REAL, channel TEXT, unit TEXT, waveform TEXT)");
	query.exec("create table sessionInfo (proxyName TEXT, proxyAddress TEXT, device TEXT, sampleRate REAL, startDate TEXT, startTime TEXT, endDate TEXT, endTime TEXT)");

	return true;
}

void NeuralCollectorThread::parseResponse()
{
	QByteArray xmlData = proxyResponse->getContent();

	QString timestamp,sampleRate;
	QString channel,unit,waveform,device,deviceStatus;
	int eventcode;

	QSqlQuery query(sessionDb);
	sessionDb.transaction();

	QXmlStreamReader reader(xmlData);

	reader.readNext();
	while(!reader.atEnd())
	{
		if(reader.isStartElement() && reader.name() == "ResponseACQ1.0" && firstGet)
		{
			device.clear();
			deviceStatus.clear();
			sampleRate = 0.0;

			while(!reader.isEndElement())
			{
				if(reader.isStartElement() && reader.name() == "device")
				{
					device = reader.readElementText();
				}
				else if(reader.isStartElement() && reader.name() == "deviceStatus")
				{
					deviceStatus = reader.readElementText();
				}
				else if(reader.isStartElement() && reader.name() == "sampleRate")
				{
					sampleRate = reader.readElementText();
				}
				reader.readNext();
			}
			QDateTime currDateTime;
			currDateTime = QDateTime::currentDateTime();

			query.prepare("INSERT INTO sessionInfo (proxyName, proxyAddress, device, sampleRate, startDate, startTime)"
				"VALUES(:proxyName, :proxyAddress, :device, :sampleRate, :startDate, :startTime)");
			query.bindValue(":proxyName", proxyName);
			query.bindValue(":proxyAddress", proxyAddress.toString());
			query.bindValue(":device", device);
			query.bindValue(":sampleRate", sampleRate);
			query.bindValue(":startDate", currDateTime.toString("MMM d, yyyy"));
			query.bindValue(":startTime", currDateTime.toString("h:mm:ss"));
			query.exec();

			firstGet = false;
		}
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
						timestamp = reader.readElementText();
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

				query.prepare("INSERT INTO spikes "
					"VALUES(:timestamp, :channel, :unit, :waveform)");
				query.bindValue(":timestamp", timestamp);
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
				query.prepare("INSERT INTO events "
					"VALUES(:timestamp, :eventcode)");
				query.bindValue(":timestamp", timestamp);
				query.bindValue(":eventcode", eventcode);
				query.exec();
			}
		}
		else
		{
			//do nothing?
		}
		reader.readNext();
		//qDebug()<<"Current token: "<<reader.tokenString();
		//qDebug()<<"   "<<reader.text();
		if(reader.hasError())
			qDebug()<<"Error: "<<reader.errorString();

	}

	sessionDb.commit();

}


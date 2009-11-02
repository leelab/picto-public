#include <qDebug>
#include <QCoreApplication>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QHostAddress>

#include "../../common/globals.h"

#include "DebugCommandHandler.h"
#include "../datacollection/neuraldatacollector.h"
#include "../datacollection/alignmenttool.h"

DebugCommandHandler::DebugCommandHandler()
{
}
/*! This is a command handler that I've been using to test and debug the server.
 *  The command is really a "catch-all" for testing various aspects of the 
 *  server without having everything up and running.  Generally speaking, it
 *  is sent using telnet, rather than from a real Picto component
 */

QSharedPointer<Picto::ProtocolResponse> DebugCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//create an "OK" response
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("PICTO","1.0",Picto::ProtocolResponseType::OK));

	QString target = command->getTarget();

 
	if(target == "/proxytest")
	{
		qDebug() << "Data Collector Test initiated";

		//set up a fake session database
		QSqlDatabase sessionDb = QSqlDatabase::addDatabase("QSQLITE","TestSession");
		sessionDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/testSession.sqlite");
		sessionDb.open();

		//set up the session database tables
		QSqlQuery sessionQ(sessionDb);
		sessionQ.exec("DROP TABLE spikes");
		sessionQ.exec("CREATE TABLE spikes "
			"(id INTEGER PRIMARY KEY, timestamp REAL, fittedtime REAL, "
			"channel TEXT, unit TEXT, waveform TEXT)");
		sessionQ.exec("DROP TABLE trials");
		sessionQ.exec("CREATE TABLE trials (id INTEGER PRIMARY KEY,trialnumber INTEGER,"
			"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
			"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
			"endjitter REAL, correlation REAL)");

		sessionQ.exec("CREATE TABLE orphantrials (id INTEGER PRIMARY KEY, aligncode INTEGER, timestamp REAL, source TEXT)");

		//open the server config database
		//open server config database
		QSqlDatabase db = QSqlDatabase::database("PictoServerConfigDatabase");
		db.open();

		//Find the first Proxy server in the database
		if(!db.tables().contains("proxyservers"))
		{
			qDebug()<<"Missing proxyservers table";
			return response;

		}

		QSqlQuery query(db);
		query.exec("SELECT id,address,port FROM proxyservers");
		if(!query.next())
		{
			qDebug()<<"No proxyservers registered";
			return response;
		}

		QHostAddress proxyAddress(query.value(1).toString());
		int proxyPort = query.value(2).toInt();

		//creating a parentless thread seems like it might be a bad idea...
		//This is only test code, but we may need to fix this later...
		NeuralDataCollector ndc(proxyAddress, proxyPort, sessionDb, 0,1000);

		//create an alignment tool (to be used by both the neural data collector, 
		//and the behavioral data command handler)
		AlignmentTool align(sessionDb);

		ndc.setAlignmentTool(&align);
		

		ndc.start();
		ndc.wait(4000);
		ndc.stop();



		qDebug()<<"Data Collector Test Complete";

	}


	return response;
}


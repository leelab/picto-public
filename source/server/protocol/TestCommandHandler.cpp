#include <qDebug>
#include <QCoreApplication>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QHostAddress>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "../../common/globals.h"

#include "TestCommandHandler.h"
#include "../datacollection/neuraldatacollector.h"
#include "../datacollection/alignmenttool.h"

TestCommandHandler::TestCommandHandler()
{
}
/*! This is a command handler that I've been using to test and debug the server.
 *  The command is really a "catch-all" for testing various aspects of the 
 *  server without having everything up and running.  Generally speaking, it
 *  is sent using telnet, rather than from a real Picto component
 */

QSharedPointer<Picto::ProtocolResponse> TestCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{

	//If it's not a debug mode, ignore all TEST commands
#ifndef QT_DEBUG
	//If it's not a debug build, return a Bad Rewuest response
	QSharedPointer<Picto::ProtocolResponse> unknownProtocolResponse(
		new Picto::ProtocolResponse(Picto::Names->serverAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::BadRequest));
	return unknownProtocolResponse;
#endif

	//create an "OK" response
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName,"PICTO","1.0",Picto::ProtocolResponseType::OK));

	QString target = command->getTarget();

	if(target == "aligntest")
	{
		doAlignTest();
	}
	//unknown target
	else
	{
		QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
			new Picto::ProtocolResponse(Picto::Names->serverAppName, 
										"HTTP","1.1",Picto::ProtocolResponseType::NotFound));
		return notFoundResponse;

	}


	return response;
}


/*! \brief Tests the alignment functionality
 *
 *	In the very near future, most of this code will get transferred into one of the real
 *	command handlers.  However, since I wanted to get things working in a test environment
 *	I am doing it here.
 *
 *	To test the alignment tool, we will create a fake session database, and then run using the
 *	first PictoBox in the config database, and the first Proxy server in the database.  This
 *	will save us a lot of development time, and get things up and running quickly.  (I hope).
 */
void TestCommandHandler::doAlignTest()
{
		/*qDebug() << "Alignment Test initiated";

		//set up a fake session database
		QSqlDatabase sessionDb = QSqlDatabase::addDatabase("QSQLITE","AlignmentTestSession");
		sessionDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/AlignmentTestSession.sqlite");
		sessionDb.open();

		////////////////////////////////////////////////
		//set up the session database tables
		////////////////////////////////////////////////
		QSqlQuery sessionQ(sessionDb);

		sessionQ.exec("DROP TABLE sessioninfo");
		sessionQ.exec("CREATE TABLE sessioninfo(id INTEGER PRIMARY KEY,"
			"key TEXT, value TEXT)");

		sessionQ.exec("DROP TABLE spikes");
		sessionQ.exec("CREATE TABLE spikes "
			"(id INTEGER PRIMARY KEY, timestamp REAL, fittedtime REAL,"
			"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");

		sessionQ.exec("DROP TABLE neuraltrials");
		sessionQ.exec("CREATE TABLE neuraltrials "
			"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER,"
			"matched INTEGER)");

		sessionQ.exec("DROP TABLE behavioraltrials");
		sessionQ.exec("CREATE TABLE behavioraltrials "
			"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER"
			"trialnumber INTEGER, matched INTEGER)");

		sessionQ.exec("DROP TABLE trials");
		sessionQ.exec("CREATE TABLE trials (id INTEGER PRIMARY KEY,trialnumber INTEGER,"
			"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
			"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
			"endjitter REAL, correlation REAL)");

		/////////////////////////////////////////
		//  Set up the Neural Data Collector
		/////////////////////////////////////////
		//open the server config database
		QSqlDatabase db = QSqlDatabase::database("PictoServerConfigDatabase");
		db.open();

		//Find the first Proxy server in the database
		if(!db.tables().contains("proxyservers"))
		{
			qDebug()<<"Missing proxyservers table";

		}

		QSqlQuery query(db);
		query.exec("SELECT name,address,port FROM proxyservers");
		if(!query.next())
		{
			qDebug()<<"No proxyservers registered";
		}

		QString proxyName = query.value(0).toString();
		QHostAddress proxyAddress(query.value(1).toString());
		int proxyPort = query.value(2).toInt();

		//Add the proxy server info to the sessioninfo table
		sessionQ.prepare("INSERT INTO sessioninfo (key, value)"
			"VALUES('Proxy Name', :value)");
		sessionQ.bindValue(":value",proxyName);
		sessionQ.exec();
		sessionQ.prepare("INSERT INTO sessioninfo (key, value)"
			"VALUES('Proxy Address', :value)");
		sessionQ.bindValue(":value",proxyAddress.toString());
		sessionQ.exec();
		sessionQ.prepare("INSERT INTO sessioninfo (key, value)"
			"VALUES('Proxy Port', :value)");
		sessionQ.bindValue(":value",proxyPort);
		sessionQ.exec();
		

		//creating a parentless thread seems like it might be a bad idea...
		//This is only test code, but we may need to fix this later...
		NeuralDataCollector ndc(sessionDb,1000,0);*/


}
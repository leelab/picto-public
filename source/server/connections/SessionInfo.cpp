#include "SessionInfo.h"
#include "ConnectionManager.h"

#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutexLocker>

SessionInfo::SessionInfo(QString directorAddr, int proxyId, QByteArray experimentXml):
	activity_(true),
	directorAddr_(directorAddr),
	proxyId_(proxyId),
	experimentXml_(experimentXml)
{
	//CreateUUID
	QUuid uuid = QUuid::createUuid();
	uuid_ = uuid;

	//Create the base session DB
	QString databaseName;

	QDateTime dateTime = QDateTime::currentDateTime();
	databaseName = "Session_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");
	
	//If there's already a file with this name, append _x until we have a unique name
	QDir dir(QCoreApplication::applicationDirPath());
	while(dir.entryList().contains(databaseName))
	{
		databaseName.append("_x");
	}

	baseSessionDbConnection_ = QSqlDatabase::addDatabase("QSQLITE",databaseName);
	baseSessionDbConnection_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + databaseName + ".sqlite");
	baseSessionDbConnection_.open();


	QSqlQuery sessionQ(baseSessionDbConnection_);

	sessionQ.exec("CREATE TABLE sessioninfo(id INTEGER PRIMARY KEY,"
		"key TEXT, value TEXT)");

	sessionQ.exec("CREATE TABLE spikes "
		"(id INTEGER PRIMARY KEY, timestamp REAL, fittedtime REAL,"
		"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");

	sessionQ.exec("CREATE TABLE neuraltrials "
		"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER,"
		"matched INTEGER)");

	sessionQ.exec("CREATE TABLE behavioraltrials "
		"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER, "
		"trialnumber INTEGER, matched INTEGER)");

	sessionQ.exec("CREATE TABLE trials (id INTEGER PRIMARY KEY,trialnumber INTEGER,"
		"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
		"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
		"endjitter REAL, correlation REAL)");

	sessionQ.exec("CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, xpos REAL, "
		"ypos REAL, time REAL)");

	sessionQ.exec("CREATE TABLE statetransitions (id INTEGER PRIMARY KEY, machinename TEXT, "
		"source TEXT, sourceresult TEXT, destination TEXT, time REAL)");

	sessionQ.exec("CREATE TABLE framedata(id INTEGER PRIMARY KEY, frame INTEGER, time REAL, state TEXT)");

	sessionQ.exec("CREATE TABLE rewards (id INTEGER PRIMARY KEY, duration INTEGER, channel INTEGER, time REAL)");

	//Add the current time
	sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"Session start\", :time)");
	sessionQ.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	sessionQ.exec();




	//Create the cacheDB
	QString cacheDatabaseName = databaseName+"_cache";
	cacheDb_ = QSqlDatabase::addDatabase("QSQLITE",cacheDatabaseName);
	cacheDb_.setDatabaseName(":memory:");
	//baseCacheDbConnection_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + cacheDatabaseName + ".sqlite");
	Q_ASSERT(cacheDb_.open());

	QSqlQuery cacheQ(cacheDb_);
	Q_ASSERT(cacheQ.exec("CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, xpos REAL, "
		"ypos REAL, time REAL)"));
	Q_ASSERT(cacheQ.exec("CREATE TABLE framedata(id INTEGER PRIMARY KEY, frame INTEGER, time REAL, state TEXT)"));

	//create alignment tool and neuraldatacollector
	//sessInfo->alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool(sessInfo->sessionDb));
	if(proxyId_ == -1)
		ndc_ = 0;
	else
	{
		ndc_ = new NeuralDataCollector(proxyId_, QCoreApplication::applicationDirPath() + "/" + databaseName + ".sqlite",50);
		QObject::connect(ndc_, SIGNAL(finished()), ndc_, SLOT(deleteLater()));
		ndc_->start();
	}}

SessionInfo::~SessionInfo()
{
	if(ndc_ && ndc_->isRunning())
		ndc_->stop();
}

//! Returns the next pending directive
QString SessionInfo::pendingDirective()
{
	if(pendingDirectives_.isEmpty())
		return QString();
	else
		return pendingDirectives_.takeFirst();
}


/*! \brief Ends the session
 *
 *	Ending a session requires the following actions to be taken
 *	 -	Add an end time in the sessioninfo
 *   -  Send ENDSESSION directive to the Director instance
 *	 -	Flush the databse cache after we're sure that the Director instance is done.
 *
 *	When the Director stops sending DIREECTORUPDATE commands, the session will 
 *	naturally time out on its own, so we don't need to do that explicitly.
 */
void SessionInfo::endSession()
{
	ConnectionManager *conMgr = ConnectionManager::Instance();
	Q_ASSERT(conMgr->getDirectorStatus(uuid_) > DirectorStatus::idle);

	QSqlDatabase sessionDb = getSessionDb();

	//Add the end time to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"Session end\", :time)");
	query.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	Q_ASSERT(query.exec());
	
	//Let the Director know that we are planning to stop
	addPendingDirective("ENDSESSION");

	//Sit around waiting for the director's state to change
	while(conMgr->getDirectorStatus(uuid_) > DirectorStatus::idle)
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
	}

	if(ndc_ && ndc_->isRunning())
	{
		ndc_->exit();
		ndc_ = 0;
	}

	//Flush the database cache
	flushCache();
}

/*! \brief Dumps the cache database into the session database
 *
 *	The cache database is memory backed.  This is good for speed, but not so great
 *	for long-term storage (if Picto crashes, we lose the data).   This function 
 *	moves the contents of the cache database to the session database (which is disk-
 *	backed).  This should be done once per trial, and before ending an experiment.
 *
 *	The command that extracts behavioral data should be smart enough to figure out
 *	which database contains the desired data, so when we flush, we'll just copy 
 *	everything when this is called.
 */
void SessionInfo::flushCache()
{
	//If the cache database is empty, we can return immediately
	QSqlQuery query(cacheDb_);
	Q_ASSERT(query.exec("SELECT COUNT (*) FROM behavioraldata"));
	query.next();
	if(query.value(0).toInt() < 1)
		return;
	query.finish();

	/*  This isn't working.  While I'm trying to find a solution
		(http://www.qtcentre.org/threads/31281-Multiple-database-connections)
		I'm just going to do the data transfer by hand.  It's ugly, but it should work

	//The plan is to execute this command:
	//	INSERT INTO session_database_name.behavioraldata (xpos,ypos,time)  
	//	SELECT xpos,ypos,time
	//	FROM behavioraldata

	QSqlQuery flushQ(cacheDb_);

	QString queryStr = QString("INSERT INTO \"%1\".behavioraldata(xpos,ypos,time) "
							   "SELECT xpos, ypos, time "
							   "FROM behavioraldata")
							   .arg(sessionDb.connectionName());
	if(!flushQ.exec(queryStr))
	{
		QString err = flushQ.lastError().text();
		QStringList conns = QSqlDatabase::connectionNames();
		int numConns = conns.length();
		while(conns.length() >0)
			QString connName = conns.takeFirst();
		int adf = err.length();
	}

	flushQ.exec("DELETE FROM behavioraldata");*/

	QSqlDatabase sessionDb = getSessionDb();

	Q_ASSERT(sessionDb.transaction());

	//The queries are in a seperate namespace so that they get
	//deleted before we try to commit.
	{
		QSqlQuery diskQuery(sessionDb);
		QSqlQuery cacheQuery(cacheDb_);
		QSqlQuery cacheCleanup(cacheDb_);

		//Flush the behavioraldata table
		//------------------------------
		Q_ASSERT(cacheQuery.exec("SELECT id,xpos,ypos,time FROM behavioraldata"));

		while(cacheQuery.next())
		{
			diskQuery.prepare("INSERT INTO behavioraldata(xpos,ypos,time) "
				"VALUES(:xpos,:ypos,:time)");
			diskQuery.bindValue(":xpos",cacheQuery.value(1));
			diskQuery.bindValue(":ypos",cacheQuery.value(2));
			diskQuery.bindValue(":time",cacheQuery.value(3));
			Q_ASSERT(diskQuery.exec());

			cacheCleanup.prepare("DELETE FROM behavioraldata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			Q_ASSERT(cacheCleanup.exec());
		}

		//Flush the framedata table
		//------------------------------
		Q_ASSERT(cacheQuery.exec("SELECT id,frame,time,state FROM framedata"));

		while(cacheQuery.next())
		{
			diskQuery.prepare("INSERT INTO framedata(frame,time,state) "
				"VALUES(:frame,:time,:state)");
			diskQuery.bindValue(":frame",cacheQuery.value(1));
			diskQuery.bindValue(":time",cacheQuery.value(2));
			diskQuery.bindValue(":state",cacheQuery.value(3));
			Q_ASSERT(diskQuery.exec());

			cacheCleanup.prepare("DELETE FROM framedata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			Q_ASSERT(cacheCleanup.exec());
		}

	}
	Q_ASSERT_X(sessionDb.commit(),"SessionInfo::flushCache","Unable to commit to session DB: "+sessionDb.lastError().text().toAscii());

}

//! \brief Inserts a trial event into the session database
void SessionInfo::insertTrialEvent(double time, int eventCode, int trialNum)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);

	query.prepare("INSERT INTO behavioraltrials (timestamp,aligncode,trialnumber,matched)"
		"VALUES(:timestamp, :aligncode, :trialnumber, 0)");
	query.bindValue(":timestamp", time);
	query.bindValue(":aligncode", eventCode);
	query.bindValue(":trialnumber", trialNum);
	Q_ASSERT(query.exec());
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertBehavioralData(Picto::BehavioralDataStore data)
{
	QSqlQuery cacheQ(cacheDb_);

	Picto::BehavioralDataStore::BehavioralDataPoint dataPoint;
	while(data.length() > 0)
	{
		dataPoint = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO behavioraldata (xpos, ypos, time)"
			"VALUES(:xpos, :ypos, :time)");
		cacheQ.bindValue(":xpos",dataPoint.x);
		cacheQ.bindValue(":ypos",dataPoint.y);
		cacheQ.bindValue(":time",dataPoint.t);
		Q_ASSERT_X(cacheQ.exec(),"SessionInfo::insertBehavioralData","Error: "+cacheQ.lastError().text().toAscii());
	}
}

//! \brief inserts a state change record in the session database
void SessionInfo::insertStateData(Picto::StateDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);
	
	query.prepare("INSERT INTO statetransitions "
		"(machinename, source, sourceresult, destination, time) "
		"VALUES(:machinename, :source, :sourceresult, :destination, :time) ");
	query.bindValue(":machinename", data.getMachineName());
	QString machineName = data.getMachineName();
	query.bindValue(":source", data.getSource()); 
	query.bindValue(":sourceresult",data.getSourceResult());
	query.bindValue(":destination",data.getDestination());
	query.bindValue(":time",data.getTime());
	Q_ASSERT(query.exec());
}

//! \brief Inserts the passed in frame data into the cache database
void SessionInfo::insertFrameData(Picto::FrameDataStore data)
{
	QSqlQuery cacheQ(cacheDb_);

	Picto::FrameDataStore::FrameData framedata;
	while(data.length() > 0)
	{
		framedata = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO framedata (frame, time, state)"
			"VALUES(:frame, :time, :state)");
		cacheQ.bindValue(":frame",framedata.frameNumber);
		cacheQ.bindValue(":time",framedata.time);
		cacheQ.bindValue(":state",framedata.stateName);
		Q_ASSERT(cacheQ.exec());
	}
}

void SessionInfo::insertRewardData(Picto::RewardDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery sessionQ(sessionDb);
	
	sessionQ.prepare("INSERT INTO rewards (duration, channel, time) "
		"VALUES (:duration, :channel, :time)");
	sessionQ.bindValue(":duration",data.getDuration());
	sessionQ.bindValue(":channel", data.getChannel());
	sessionQ.bindValue(":time",data.getTime());
	Q_ASSERT(sessionQ.exec());
}


/*! \brief Returns a behavioral datastore with all of the data collected after the timestamp
 *
 *	This functions creates a new BehavioralDataStore object, and fills it with
 *	all of the data collected after the passed in timestamp.  If the timestamp is 0, 
 *	then all data is returned.  This is actually a bit tricky, since the data could be
 *	either the session database or the cache database.
 */
Picto::BehavioralDataStore SessionInfo::selectBehavioralData(double timestamp)
{
	QSqlDatabase sessionDb = getSessionDb();
	Picto::BehavioralDataStore dataStore;
	
	//If the timestamp is -1, we only need to return the most recent behavioral data
	if(timestamp < 0)
	{
		//check the cache database first
		QSqlQuery cacheQuery(cacheDb_);

		cacheQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
			"ORDER BY time DESC");
		Q_ASSERT(cacheQuery.exec());

		if(cacheQuery.next())
		{
			dataStore.addData(cacheQuery.value(0).toDouble(),
							  cacheQuery.value(1).toDouble(),
							  cacheQuery.value(2).toDouble());
		}
		else
		{
			//If there wasn't anything in the cache db, pull from the session db
			QSqlQuery sessionQuery;
			sessionQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
				"ORDER BY time DESC");
			Q_ASSERT(sessionQuery.exec());

			if(sessionQuery.next())
			{
				dataStore.addData(cacheQuery.value(0).toDouble(),
								  cacheQuery.value(1).toDouble(),
								  cacheQuery.value(2).toDouble());
			}
		}
	}
	else
	{
		//First, we attempt to pull data from the session database.  This will 
		//likely return an empty query.
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
			"WHERE time > :time ORDER BY time ASC");
		if(timestamp == 0)
			sessionQuery.bindValue(":time",-1);
		else
			sessionQuery.bindValue(":time", timestamp);
		Q_ASSERT(sessionQuery.exec());

		while(sessionQuery.next())
		{
			dataStore.addData(sessionQuery.value(0).toDouble(),
							  sessionQuery.value(1).toDouble(),
							  sessionQuery.value(2).toDouble());
		}


		QSqlQuery cacheQuery(cacheDb_);

		cacheQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
			"WHERE time > :time ORDER BY time ASC");
		if(timestamp == 0)
			cacheQuery.bindValue(":time",-1);
		else
			cacheQuery.bindValue(":time", timestamp);
		Q_ASSERT(cacheQuery.exec());

		while(cacheQuery.next())
		{
			dataStore.addData(cacheQuery.value(0).toDouble(),
							  cacheQuery.value(1).toDouble(),
							  cacheQuery.value(2).toDouble());
		}
	}

	return dataStore;
}


/*! \brief Returns a frame datastore with all of the data collected after the timestamp
 *
 *	This functions creates a new FrameDataStore object, and fills it with
 *	all of the data collected after the passed in timestamp.  If the timestamp is 0, 
 *	then all data is returned.  This is actually a bit tricky, since the data could be
 *	either the session database or the cache database.
 */
Picto::FrameDataStore SessionInfo::selectFrameData(double timestamp)
{
	QSqlDatabase sessionDb = getSessionDb();
	Picto::FrameDataStore dataStore;

	if(timestamp < 0)
	{
		//check the cache db first
		QSqlQuery cacheQuery(cacheDb_);
		cacheQuery.prepare("SELECT frame,time,state FROM framedata "
			"ORDER BY time DESC");
		Q_ASSERT_X(cacheQuery.exec(),"SessionInfo::insertBehavioralData","Error: "+cacheQuery.lastError().text().toAscii());

		if(cacheQuery.next())
		{
			dataStore.addFrame(cacheQuery.value(0).toInt(),
							  cacheQuery.value(1).toDouble(),
							  cacheQuery.value(2).toString());
		}
		else
		{
			//There was nothing in the cache db, so we need to pull from the session db
			QSqlQuery sessionQuery(sessionDb);	
			sessionQuery.prepare("SELECT frame,time,state FROM framedata "
				"ORDER BY time DESC");
			Q_ASSERT_X(sessionQuery.exec(),"SessionInfo::insertBehavioralData","Error: "+cacheQuery.lastError().text().toAscii());

			if(sessionQuery.next())
			{
				dataStore.addFrame(cacheQuery.value(0).toInt(),
								  cacheQuery.value(1).toDouble(),
								  cacheQuery.value(2).toString());
			}
		}

	}
	else
	{
		//First, we attempt to pull data from the session database.  This will 
		//likely return an empty query.
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT frame,time,state FROM framedata "
			"WHERE time > :time ORDER BY time ASC");
		if(timestamp == 0)
			sessionQuery.bindValue(":time",-1);
		else
			sessionQuery.bindValue(":time", timestamp);
		Q_ASSERT(sessionQuery.exec());

		while(sessionQuery.next())
		{
			dataStore.addFrame(sessionQuery.value(1).toInt(),
							   sessionQuery.value(2).toDouble(),
							   sessionQuery.value(3).toString());
		}


		QSqlQuery cacheQuery(cacheDb_);
		cacheQuery.prepare("SELECT frame,time,state FROM framedata "
			"WHERE time > :time ORDER BY time ASC");
		if(timestamp == 0)
			cacheQuery.bindValue(":time",-1);
		else
			cacheQuery.bindValue(":time", timestamp);
		Q_ASSERT_X(cacheQuery.exec(),"SessionInfo::insertBehavioralData","Error: "+cacheQuery.lastError().text().toAscii());

		while(cacheQuery.next())
		{
			dataStore.addFrame(cacheQuery.value(0).toInt(),
							  cacheQuery.value(1).toDouble(),
							  cacheQuery.value(2).toString());
		}
	}
	return dataStore;

}


/*! \brief Returns a list of state datastores with all of the data collected after the timestamp
 *
 *	This functions creates a list of state data stores and returns it.  If the timestamp is 0, 
 *	then all data is returned.
 */
QList<Picto::StateDataStore> SessionInfo::selectStateData(double timestamp)
{
	QSqlDatabase sessionDb = getSessionDb();
	QList<Picto::StateDataStore> stateDataList;

	if(timestamp < 0)
	{
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinename FROM statetransitions  "
			"ORDER BY time DESC");
		Q_ASSERT_X(sessionQuery.exec(), "SessionInfo::selectBehavioralData", sessionQuery.lastError().text().toAscii());

		if(sessionQuery.next())
		{
			Picto::StateDataStore data;
			data.setTransition(sessionQuery.value(0).toString(),
							   sessionQuery.value(1).toString(),
							   sessionQuery.value(2).toString(),
							   sessionQuery.value(3).toDouble(),
							   sessionQuery.value(4).toString());
			stateDataList.append(data);
		}
		else
		{
			//If you hit this, something has gone wrong, and we should fix it.
			//////////TESTING
			Q_ASSERT_X(false,"SessionInfo::selectStateData", "SELECT state data query failed to return data");
		}
	}
	else
	{
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinename FROM statetransitions  "
			"WHERE time > :time ORDER BY time ASC");
		if(timestamp == 0)
			sessionQuery.bindValue(":time",-1);
		else
			sessionQuery.bindValue(":time", timestamp);
		Q_ASSERT_X(sessionQuery.exec(), "SessionInfo::selectBehavioralData", sessionQuery.lastError().text().toAscii());

		while(sessionQuery.next())
		{
			Picto::StateDataStore data;
			data.setTransition(sessionQuery.value(0).toString(),
							   sessionQuery.value(1).toString(),
							   sessionQuery.value(2).toString(),
							   sessionQuery.value(3).toDouble(),
							   sessionQuery.value(4).toString());
			stateDataList.append(data);
		}
	}

	return stateDataList;

}

/*! /brief Generates a thread-specific connection to the Session database
 *
 *	The session database has threading issues because of the following:
 *		1. The session database may be accessed from different threads 
 *		   (remember, the commands coming into the server are processed
 *		   by different threads)
 *		2. Database connections can't be shared across threads
 *	The current solution to these problems is to create a unique database
 *	connection for each thread.  This is somewhat wasteful, but it's simple
 *	and it works.
 */

QSqlDatabase SessionInfo::getSessionDb()
{
	QSqlDatabase sessionDb;
	QString connectionName = QString("SessionDatabase%1").arg((int)QThread::currentThreadId());

	
	//If we already have a connection open in this thread, use it, 
	//otherwise, clone a new connection
	if(QSqlDatabase::contains(connectionName))
	{
		sessionDb = QSqlDatabase::database(connectionName);
	}
	else
	{
		sessionDb = QSqlDatabase::cloneDatabase(baseSessionDbConnection_,connectionName);
		sessionDb.open();
	}

	Q_ASSERT(sessionDb.isOpen());

	return sessionDb;
}
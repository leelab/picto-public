#include "SessionInfo.h"
#include "ConnectionManager.h"

#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SessionInfo::SessionInfo(QString directorAddr):
	activity_(true)
{
	//CreateUUID
	QUuid uuid = QUuid::createUuid();
	uuid_ = uuid;

	//create alignment tool and neuraldatacollector
	//sessInfo->alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool(sessInfo->sessionDb_));
	//sessInfo->ndc_ = QSharedPointer<NeuralDataCollector>(new NeuralDataCollector(sessInfo->sessionDb_,50));
 
	//Set the director address
	directorAddr_ = directorAddr;


	//Create a new Session Database
	QDateTime dateTime = QDateTime::currentDateTime();
	QString databaseName="Session_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");
	
	//! \todo come up with a better place to store session data...
	QDir dir(QCoreApplication::applicationDirPath());
	while(dir.entryList().contains(databaseName))
	{
		databaseName.append("_x");
	}

	sessionDb_ = QSqlDatabase::addDatabase("QSQLITE",databaseName);
	sessionDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + databaseName + ".sqlite");
	Q_ASSERT(sessionDb_.open());

	QSqlQuery sessionQ(sessionDb_);

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
	//cacheDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + cacheDatabaseName + ".sqlite");
	Q_ASSERT(cacheDb_.open());

	QSqlQuery cacheQ(cacheDb_);
	cacheQ.exec("CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, xpos REAL, "
		"ypos REAL, time REAL)");
	cacheQ.exec("CREATE TABLE framedata(id INTEGER PRIMARY KEY, frame INTEGER, time REAL, state TEXT)");
}

SessionInfo::~SessionInfo()
{
	cacheDb_.close();
	sessionDb_.close();
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
	//Add the end time to the session db
	QSqlQuery query(sessionDb_);
	query.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"Session end\", :time)");
	query.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	query.exec();

	//Let the Director know that we are planning to stop
	addPendingDirective("ENDSESSION");

	//Sit around waiting for the director's state to change
	ConnectionManager *conMgr = ConnectionManager::Instance();
	while(conMgr->getDirectorStatus(uuid_) == DirectorStatus::running)
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
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
	query.exec("SELECT COUNT (*) FROM behavioraldata");
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
							   .arg(sessionDb_.connectionName());
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


	Q_ASSERT(sessionDb_.transaction());

	//The queries are in a seperate namespace so that they get
	//deleted before we try to commit.
	{
		QSqlQuery diskQuery(sessionDb_);
		QSqlQuery cacheQuery(cacheDb_);
		QSqlQuery cacheCleanup(cacheDb_);

		//Flush the behavioraldata table
		//------------------------------
		cacheQuery.exec("SELECT id,xpos,ypos,time FROM behavioraldata");

		while(cacheQuery.next())
		{
			diskQuery.prepare("INSERT INTO behavioraldata(xpos,ypos,time) "
				"VALUES(:xpos,:ypos,:time)");
			diskQuery.bindValue(":xpos",cacheQuery.value(1));
			diskQuery.bindValue(":ypos",cacheQuery.value(2));
			diskQuery.bindValue(":time",cacheQuery.value(3));
			diskQuery.exec();

			cacheCleanup.prepare("DELETE FROM behavioraldata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			cacheCleanup.exec();
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
			diskQuery.exec();

			cacheCleanup.prepare("DELETE FROM framedata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			cacheCleanup.exec();
		}

	}
	Q_ASSERT(sessionDb_.commit());
}

//! \brief Inserts a trial event into the session database
void SessionInfo::insertTrialEvent(double time, int eventCode, int trialNum)
{
	QSqlQuery query(sessionDb_);

	query.prepare("INSERT INTO behavioraltrials (timestamp,aligncode,trialnumber,matched)"
		"VALUES(:timestamp, :aligncode, :trialnumber, 0)");
	query.bindValue(":timestamp", time);
	query.bindValue(":aligncode", eventCode);
	query.bindValue(":trialnumber", trialNum);
	query.exec();
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertBehavioralData(Picto::BehavioralDataStore data)
{
	Picto::BehavioralDataStore::BehavioralDataPoint dataPoint;
	while(data.length() > 0)
	{
		QSqlQuery cacheQ(cacheDb_);

		dataPoint = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO behavioraldata (xpos, ypos, time)"
			"VALUES(:xpos, :ypos, :time)");
		cacheQ.bindValue(":xpos",dataPoint.x);
		cacheQ.bindValue(":ypos",dataPoint.y);
		cacheQ.bindValue(":time",dataPoint.t);
		Q_ASSERT(cacheQ.exec());
	}
}

//! \brief inserts a state change record in the session database
void SessionInfo::insertStateData(Picto::StateDataStore data)
{
	QSqlQuery query(sessionDb_);
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
	Picto::FrameDataStore::FrameData framedata;
	while(data.length() > 0)
	{
		QSqlQuery cacheQ(cacheDb_);

		framedata = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO framedata (frame, time, state)"
			"VALUES(:frame, :time, :state)");
		cacheQ.bindValue(":frame",framedata.frameNumber);
		cacheQ.bindValue(":time",framedata.time);
		cacheQ.bindValue(":state",framedata.stateName);
		cacheQ.exec();
	}
}

void SessionInfo::insertRewardData(Picto::RewardDataStore data)
{
	QSqlQuery sessionQ(sessionDb_);
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
	Picto::BehavioralDataStore dataStore;

	//First, we attempt to pull data from the session database.  This will 
	//likely return an empty query.
	QSqlQuery sessionQuery(sessionDb_);
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
	Picto::FrameDataStore dataStore;

	//First, we attempt to pull data from the session database.  This will 
	//likely return an empty query.
	QSqlQuery sessionQuery(sessionDb_);
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
	Q_ASSERT(cacheQuery.exec());

	while(cacheQuery.next())
	{
		dataStore.addFrame(cacheQuery.value(0).toInt(),
						  cacheQuery.value(1).toDouble(),
						  cacheQuery.value(2).toString());
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
	QList<Picto::StateDataStore> stateDataList;

	QSqlQuery sessionQuery(sessionDb_);
	sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinename FROM statetransitions  "
		"WHERE time > :time ORDER BY time ASC");
	if(timestamp == 0)
		sessionQuery.bindValue(":time",-1);
	else
		sessionQuery.bindValue(":time", timestamp);
	if(!sessionQuery.exec())
	{
		QString err = sessionQuery.lastError().text();
		Q_ASSERT_X(false, "SessionInfo::selectBehavioralData", err.toAscii());
	}

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

	return stateDataList;

}
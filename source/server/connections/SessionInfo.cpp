#include "SessionInfo.h"
#include "ConnectionManager.h"

#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutexLocker>
#include <QStringList>

//This turns on and off the authorized user permission setup on SessionInfo
//#define NO_AUTH_REQUIRED

SessionInfo::SessionInfo(QByteArray experimentXml, QUuid initialObserverId):
	experimentXml_(experimentXml)
{
	InitializeVariables();
	
	//Add the initial observer (the session creator) to our list of authorized observers
	//This means that he can issue ENDSESSION and TASK commands.
	authorizedObservers_.append(initialObserverId);

	//Generate the base session DB name
	QDateTime dateTime = QDateTime::currentDateTime();
	QString databaseName = "Session_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");	
	//If there's already a file with this name, append _x until we have a unique name
	QDir dir(QCoreApplication::applicationDirPath());
	while(dir.entryList().contains(databaseName))
	{
		databaseName.append("_x");
	}
	databaseName.append(".sqlite");

	//Setup databases
	LoadBaseSessionDatabase(databaseName);
	SetupBaseSessionDatabase();
	CreateCacheDatabase(databaseName);
}

SessionInfo::SessionInfo(QString databaseFilePath)
{
	InitializeVariables();

	//Setup databases
	QStringList strList = databaseFilePath.split("/");
	QString databaseName = strList[strList.size()-1];
	LoadBaseSessionDatabase(databaseName);
	Q_ASSERT(baseSessionDbConnection_.tables().contains("sessioninfo"));

	SetupBaseSessionDatabase();
	CreateCacheDatabase(databaseName);

	QSqlQuery sessionQ(baseSessionDbConnection_);
	// Load SessionID
	sessionQ.exec("SELECT value FROM sessioninfo WHERE key=\"SessionID\"");
	if(sessionQ.next())
		uuid_ = QUuid(sessionQ.value(0).toString());
	
	// Load ExperimentXML
	sessionQ.exec("SELECT value FROM sessioninfo WHERE key=\"ExperimentXML\"");
	if(sessionQ.next())
		experimentXml_ = sessionQ.value(0).toByteArray();

	// Load TimeCreated
	sessionQ.exec("SELECT value FROM sessioninfo WHERE key=\"sessionstart\"");
	if(sessionQ.next())
		timeCreated_ = sessionQ.value(0).toString();
	
	// Load Authorized Observers
	sessionQ.exec("SELECT value FROM sessioninfo WHERE key=\"AuthObserverID\"");
	while(sessionQ.next())
		authorizedObservers_.append(QUuid(sessionQ.value(0).toString()));

	// Load Components
	QSharedPointer<ComponentInfo> component;
	sessionQ.exec("SELECT id,address, name, type FROM componentinfo");
	while(sessionQ.next())
	{
		component = QSharedPointer<ComponentInfo>(new ComponentInfo());
		component->setUuid(QUuid(sessionQ.value(0).toString()));
		component->setAddress(sessionQ.value(1).toString());
		component->setName(sessionQ.value(2).toString());
		component->setType(sessionQ.value(3).toString());
		component->setStatus(ComponentStatus::notFound);
		components_[component->getType()] = component;
	}

	// Load AlignToType
	sessionQ.exec("SELECT value FROM sessioninfo WHERE key=\"AlignToComponent\"");
	if(sessionQ.next())
		alignToType_ = sessionQ.value(0).toString();

	//Load latestTimeValues

	qulonglong maxID = 0;
	maxID = LoadMaxDataID("behavioraltrials");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("neuraltrials");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("spikes");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("behavioraldata");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("statetransitions");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("framedata");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("rewards");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
}

SessionInfo::~SessionInfo()
{
}

//! \brief Adds a component to this session (ie. Director or proxy)
void SessionInfo::AddComponent(QSharedPointer<ComponentInfo> component)
{
	QSqlDatabase sessionDb = getSessionDb();
	//Add the component to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO componentinfo(id,address, name, type)"
		" VALUES (:id,:address,:name,:type)");
	query.bindValue(":id", component->getUuid().toString());
	query.bindValue(":address", component->getAddress());
	query.bindValue(":name", component->getName());
	query.bindValue(":type", component->getType());
	Q_ASSERT(query.exec());
	components_[component->getType()] = component;
}

//! \brief Gets a component from this session by Type
QSharedPointer<ComponentInfo> SessionInfo::getComponentByType(QString type)
{
	if(components_.contains(type))
		return components_[type];
	return QSharedPointer<ComponentInfo>();
}

//! \brief Returns true if this Session is attached to the component with input componentID
bool SessionInfo::hasComponent(QUuid componentID)
{
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		if(component->getUuid() == componentID)
			return true;
	}
	return false;
}

/*! \brief Sets the type of component to which other component types' timestamps should be aligned.
 *	If this is not called, component timestamps will not be aligned.
 */
void SessionInfo::alignTimestampsTo(QString componentType)
{
	Q_ASSERT(!getComponentByType(componentType).isNull());
	
	QSqlDatabase sessionDb = getSessionDb();
	//Add the align to type to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO sessioninfo(key,value)"
		" VALUES (\"AlignToComponent\",:type)");
	query.bindValue(":type", componentType);
	Q_ASSERT(query.exec());
	alignToType_ = componentType;
}


//! Returns the next pending directive
QString SessionInfo::pendingDirective(QUuid componentID)
{
	if(!pendingDirectives_.contains(componentID) || pendingDirectives_[componentID].isEmpty())
		return QString();
	else
		return pendingDirectives_[componentID].takeFirst();
}

/*!	\brief Adds the input directive to the pendingDirectives list for the component with the input type
 */
void SessionInfo::addPendingDirective(QString directive, QString componentType)
{
	QSharedPointer<ComponentInfo> component = getComponentByType(componentType);
	if(component.isNull())
		return;
	QStringList* directives = &pendingDirectives_[component->getUuid()];
	if(!directives->size() || (directives->first() != directive))
		directives->append(directive); 
};


/*! \brief Ends the session
 *
 *	Ending a session requires the following actions to be taken
 *	 -	Add an end time in the sessioninfo
 *   -  Send ENDSESSION directive to the Director and Proxy instances
 *	 -	Flush the databse cache after we're sure that the Director instance is done.
 *
 */
void SessionInfo::endSession()
{
	//ConnectionManager *conMgr = ConnectionManager::Instance();
	//Q_ASSERT(conMgr->getComponentStatusBySession(uuid_,"DIRECTOR") > ComponentStatus::idle);

	
	//Let the Director and Proxy know that we are planning to stop
	addPendingDirective("ENDSESSION","DIRECTOR");
	addPendingDirective("ENDSESSION","PROXY");

	//Sit around waiting for the components' states to change
	QTime timer;
	timer.start();
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		//Keep waiting until either both components are idle, or the session hasn't had activity on it for 10 seconds.
		while(component->getStatus() > ComponentStatus::idle && timer.elapsed() < 10000)
		{
			QThread::yieldCurrentThread();
			QCoreApplication::processEvents();
			if(activity_)
				timer.start();
		}
		Q_ASSERT_X(timer.elapsed()<10000, "SessionInfo::endSession",QString("%1 failed to stop within 10 seconds").arg(component->getName()).toAscii());
	}

	QSqlDatabase sessionDb = getSessionDb();

	//Add the end time to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"sessionend\", :time)");
	query.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	Q_ASSERT(query.exec());
	
	

	//Flush the database cache
	flushCache();
}


/*!	\brief Determines if the observer with the passed in ID is authorized to issue commands
 *
 *	This is a really simple means of setting permissions on the session.  Effectively there
 *	are two levels of user: authorized, and unauthorized.  This allows us to limit who is 
 *	allowed to do things like issuing TASK and ENDSESSION commands.
 *
 *	The authorized observers list contains all observers who are authorized at the higher 
 *	permission level.  However, there is a special case.  If the list contains a null Uuid,
 *	then all observers (including those using null IDs) are considered to be at the authorized
 *	level.
 *
 *	!!!!  WARNING  !!!!
 *	This is by no means a secure method of assigning permissions.  Since all network communication
 *	is unencrypted, it would be trivial to intercept a valid observer ID, and then append it to
 *	spoofed commands.  If you want high security, you'll need to rewrite this from the ground up.
 */
bool SessionInfo::isAuthorizedObserver(QUuid observerId)
{
	if(authorizedObservers_.contains(QUuid()))
		return true;
	else if(authorizedObservers_.contains(observerId))
		return true;
	else
		return false;
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

	flushQ.exec("DELETE FROM behavioraldata");
	//Then verify no replicated data...
	*/

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
		Q_ASSERT(cacheQuery.exec("SELECT id,dataid,xpos,ypos,time FROM behavioraldata"));

		while(cacheQuery.next())
		{
			diskQuery.prepare("INSERT INTO behavioraldata(dataid, xpos,ypos,time) "
				"VALUES(:dataid, :xpos,:ypos,:time)");
			diskQuery.bindValue(":dataid",cacheQuery.value(1));
			diskQuery.bindValue(":xpos",cacheQuery.value(2));
			diskQuery.bindValue(":ypos",cacheQuery.value(3));
			diskQuery.bindValue(":time",cacheQuery.value(4));
			Q_ASSERT(diskQuery.exec());

			cacheCleanup.prepare("DELETE FROM behavioraldata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			Q_ASSERT(cacheCleanup.exec());
		}

		//Flush the framedata table
		//------------------------------
		Q_ASSERT(cacheQuery.exec("SELECT id,dataid,frame,time,state FROM framedata"));

		while(cacheQuery.next())
		{
			diskQuery.prepare("INSERT INTO framedata(dataid,frame,time,state) "
				"VALUES(:dataid,:frame,:time,:state)");
			diskQuery.bindValue(":dataid",cacheQuery.value(1));
			diskQuery.bindValue(":frame",cacheQuery.value(2));
			diskQuery.bindValue(":time",cacheQuery.value(3));
			diskQuery.bindValue(":state",cacheQuery.value(4));
			Q_ASSERT(diskQuery.exec());

			cacheCleanup.prepare("DELETE FROM framedata WHERE id=:id");
			cacheCleanup.bindValue(":id", cacheQuery.value(0));
			Q_ASSERT(cacheCleanup.exec());
		}

	}
	Q_ASSERT_X(sessionDb.commit(),"SessionInfo::flushCache","Unable to commit to session DB: "+sessionDb.lastError().text().toAscii());

}

/*! \brief Inserts a trial event into the session database after aligning timestamps with the alignToComponent.
 *	If sourceType is not the alignToType, timebase alignment will occur before adding trial to the database.
 *	If no alignToType has been set, no timestamps will be aligned.
 *	Currently, if no alignToType is set, all trials will go into the NeuralTrials database.  We should generalize
 *	this at some point.
 */
void SessionInfo::insertTrialEvent(double time, int eventCode, int trialNum, QString sourceType, qulonglong dataID)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);
	Q_ASSERT(getComponentByType(sourceType));
	// If the trial event comes from the component providing the time baseline, put it in behaviortrials an
	// don't align its timestamps.
	if(sourceType == alignToType_)
	{
		query.prepare("INSERT INTO behavioraltrials (dataid,timestamp,aligncode,trialnumber,matched)"
			"VALUES(:dataid, :timestamp, :aligncode, :trialnumber, 0)");
		query.bindValue(":dataid", dataID);
		query.bindValue(":timestamp", time);
		query.bindValue(":aligncode", eventCode);
		query.bindValue(":trialnumber", trialNum);
		Q_ASSERT_X(query.exec(),"SessionInfo::insertTrialEvent","Error: "+query.lastError().text().toAscii());	
		return;
	}
	// If we got here, its Neural Data
	//Add the event code to the neuraltrials table
	query.prepare("INSERT INTO neuraltrials (dataid, timestamp, aligncode, matched)"
		"VALUES(:dataid, :timestamp, :aligncode, 0)");
	query.bindValue(":dataid", dataID);
	query.bindValue(":timestamp", time);
	query.bindValue(":aligncode", eventCode);
	query.exec();

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
	{
		QMutexLocker locker(&alignmentMutex_);
		alignmentTool_->doIncrementalAlignment(sessionDb);
		
		// alignment isn't calculated until the end of the first trial, meaning that all spikes in the first trial are unaligned.
		// once the first alignment values are calculated, go back and update the fittedtimes of neural trials that weren't yet aligned.
		if(!timestampsAligned_ && (alignmentTool_->getCorrelationCoefficient() != 0) )
		{	
			timestampsAligned_ = true;
			QSqlQuery unalignedSpikes(query);
			timestampsAligned_ &= unalignedSpikes.exec("SELECT id, timestamp FROM spikes WHERE fittedtime<0");
			while(unalignedSpikes.next())
			{
				query.prepare("UPDATE spikes SET fittedtime=:fittedtime, correlation=:correlation WHERE id=:id");
				query.bindValue(":id",unalignedSpikes.value(0).toInt());
				query.bindValue(":fittedtime",alignmentTool_->convertToBehavioralTimebase(unalignedSpikes.value(1).toDouble()));
				query.bindValue(":correlation",alignmentTool_->getCorrelationCoefficient() );
				timestampsAligned_ &= query.exec();
			}
		}
	}
}

/*! \brief inserts a neural record in the session database
 */
void SessionInfo::insertNeuralData(Picto::NeuralDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);

	QMutexLocker locker(&alignmentMutex_);
	//! Use the alignment tool to fit the time
	data.setFittedtime(alignmentTool_->convertToBehavioralTimebase(data.getTimestamp()));
	data.setCorrelation(alignmentTool_->getCorrelationCoefficient());
	locker.unlock();
	
	query.prepare("INSERT INTO spikes (dataid, timestamp, fittedtime, correlation, channel, unit, waveform) "
		"VALUES(:dataid, :timestamp, :fittedtime, :correlation, :channel, :unit, :waveform)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":timestamp", data.getTimestamp());
	query.bindValue(":fittedtime", data.getFittedtime());
	query.bindValue(":correlation", data.getCorrelation());
	query.bindValue(":channel", data.getChannel());
	query.bindValue(":unit", data.getUnit());
	query.bindValue(":waveform", data.getWaveformAsString());
	Q_ASSERT_X(query.exec(),"",query.lastError().text().toAscii());
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertBehavioralData(Picto::BehavioralDataStore data)
{
	QSqlQuery cacheQ(cacheDb_);

	Picto::BehavioralUnitDataStore dataPoint;
	while(data.length() > 0)
	{
		dataPoint = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO behavioraldata (dataid, xpos, ypos, time)"
			"VALUES(:dataid, :xpos, :ypos, :time)");
		cacheQ.bindValue(":dataid", dataPoint.getDataID());
		cacheQ.bindValue(":xpos",dataPoint.x);
		cacheQ.bindValue(":ypos",dataPoint.y);
		cacheQ.bindValue(":time",dataPoint.t);
		Q_ASSERT_X(cacheQ.exec(),"SessionInfo::insertBehavioralData","Error: "+cacheQ.lastError().text().toAscii());
	}
}

//! \brief inserts an alignment data point in the database and attempts latest alignment
void SessionInfo::insertAlignmentData(Picto::AlignmentDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);
	// If the trial event comes with an alignNumber, it must be from the component providing the time baseline, 
	// put it in behaviortrials and don't align its timestamps.
	if(data.hasAlignNumber())
	{
		query.prepare("INSERT INTO behavioraltrials (dataid,timestamp,aligncode,trialnumber,matched)"
			"VALUES(:dataid, :timestamp, :aligncode, :trialnumber, 0)");
		query.bindValue(":dataid", data.getDataID());
		query.bindValue(":timestamp", data.getTimestamp());
		query.bindValue(":aligncode", data.getAlignCode());
		query.bindValue(":trialnumber", data.getAlignNumber());
		Q_ASSERT_X(query.exec(),"SessionInfo::insertTrialEvent","Error: "+query.lastError().text().toAscii());	
		return;
	}
	//If we got here, its not coming from the component providing the time baseline.
	//Add the event code to the neuraltrials table
	query.prepare("INSERT INTO neuraltrials (dataid, timestamp, aligncode, matched)"
		"VALUES(:dataid, :timestamp, :aligncode, 0)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":timestamp", data.getTimestamp());
	query.bindValue(":aligncode", data.getAlignCode());
	query.exec();

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
	{
		QMutexLocker locker(&alignmentMutex_);
		alignmentTool_->doIncrementalAlignment(sessionDb);
		
		// alignment isn't calculated until the end of the first trial, meaning that all spikes in the first trial are unaligned.
		// once the first alignment values are calculated, go back and update the fittedtimes of neural trials that weren't yet aligned.
		if(!timestampsAligned_ && (alignmentTool_->getCorrelationCoefficient() != 0) )
		{	
			timestampsAligned_ = true;
			QSqlQuery unalignedSpikes(query);
			timestampsAligned_ &= unalignedSpikes.exec("SELECT id, timestamp FROM spikes WHERE fittedtime<0");
			while(unalignedSpikes.next())
			{
				query.prepare("UPDATE spikes SET fittedtime=:fittedtime, correlation=:correlation WHERE id=:id");
				query.bindValue(":id",unalignedSpikes.value(0).toInt());
				query.bindValue(":fittedtime",alignmentTool_->convertToBehavioralTimebase(unalignedSpikes.value(1).toDouble()));
				query.bindValue(":correlation",alignmentTool_->getCorrelationCoefficient() );
				timestampsAligned_ &= query.exec();
			}
		}
	}
}

//! \brief inserts a state change record in the session database
void SessionInfo::insertStateData(Picto::StateDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);

	query.prepare("INSERT INTO statetransitions "
		"(dataid, machinepath, source, sourceresult, destination, time) "
		"VALUES(:dataid, :machinepath, :source, :sourceresult, :destination, :time) ");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":machinepath", data.getMachinePath());
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

	Picto::FrameUnitDataStore framedata;
	while(data.length() > 0)
	{
		framedata = data.takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO framedata (dataid, frame, time, state)"
			"VALUES(:dataid, :frame, :time, :state)");
		cacheQ.bindValue(":dataid", framedata.getDataID());
		cacheQ.bindValue(":frame",framedata.frameNumber);
		cacheQ.bindValue(":time",framedata.time);
		cacheQ.bindValue(":state",framedata.stateName);
		Q_ASSERT(cacheQ.exec());
	}
}

void SessionInfo::insertRewardData(Picto::RewardDataStore data)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);

	query.prepare("INSERT INTO rewards (dataid, duration, channel, time) "
		"VALUES (:dataid, :duration, :channel, :time)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":duration",data.getDuration());
	query.bindValue(":channel", data.getChannel());
	query.bindValue(":time",data.getTime());
	Q_ASSERT(query.exec());
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
 *	This function creates a list of state data stores and returns it.  If the timestamp is 0, 
 *	then all data is returned.
 */
QList<Picto::StateDataStore> SessionInfo::selectStateData(double timestamp)
{
	QSqlDatabase sessionDb = getSessionDb();
	QList<Picto::StateDataStore> stateDataList;

	if(timestamp < 0)
	{
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions  "
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
		//If there is no data, that's OK  (This happens when we join a session that hasn't started running yet)
	}
	else
	{
		QSqlQuery sessionQuery(sessionDb);
		sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions  "
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

void SessionInfo::InitializeVariables()
{
	activity_ = true;
	timestampsAligned_ = (false);
	//CreateUUID
	if(uuid_ == QUuid())
	{
		uuid_ = QUuid::createUuid();
	}
	maxReceivedDataID_ = 0;

#ifdef NO_AUTH_REQUIRED
	//Add a null QUuid so that everyone is considered an authorized user
	authorizedObservers_.append(QUuid());
#endif
	//create alignment tool
	alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool());
}
void SessionInfo::LoadBaseSessionDatabase(QString databaseName)
{
	baseSessionDbConnection_ = QSqlDatabase::addDatabase("QSQLITE",databaseName);
	baseSessionDbFilepath_ = QCoreApplication::applicationDirPath() + "/" + databaseName;
	baseSessionDbConnection_.setDatabaseName(baseSessionDbFilepath_);
	baseSessionDbConnection_.open();
}
void SessionInfo::SetupBaseSessionDatabase()
{
	Q_ASSERT(baseSessionDbConnection_.isOpen());
	QSqlQuery sessionQ(baseSessionDbConnection_);

	if(!baseSessionDbConnection_.tables().contains("sessioninfo"))
	{
		sessionQ.exec("CREATE TABLE sessioninfo(id INTEGER PRIMARY KEY,"
			"key TEXT, value TEXT)");
		//Add the current time
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"sessionstart\", :time)");
		timeCreated_ = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
		sessionQ.bindValue(":time", timeCreated_);
		sessionQ.exec();
		//Add the session ID
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"SessionID\", :id)");
		sessionQ.bindValue(":id", uuid_.toString());
		sessionQ.exec();
		//Add authorized observers
		foreach(QUuid observer, authorizedObservers_)
		{
			sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"AuthObserverID\", :id)");
			sessionQ.bindValue(":id", observer.toString());
			sessionQ.exec();
		}
		//Add the experimentXML
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"ExperimentXML\", :xml)");
		sessionQ.bindValue(":xml", QString(experimentXml_));
		sessionQ.exec();
	}

	if(!baseSessionDbConnection_.tables().contains("componentinfo"))
		sessionQ.exec("CREATE TABLE componentinfo(id,"
			"address TEXT, name TEXT, type TEXT)");
	

	if(!baseSessionDbConnection_.tables().contains("spikes"))
		sessionQ.exec("CREATE TABLE spikes "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, fittedtime REAL,"
			"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");

	if(!baseSessionDbConnection_.tables().contains("neuraltrials"))
		sessionQ.exec("CREATE TABLE neuraltrials "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, aligncode INTEGER,"
			"matched INTEGER)");

	if(!baseSessionDbConnection_.tables().contains("behavioraltrials"))
		sessionQ.exec("CREATE TABLE behavioraltrials "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE UNIQUE ON CONFLICT IGNORE, timestamp REAL, aligncode INTEGER, "
			"trialnumber INTEGER, matched INTEGER)");

	if(!baseSessionDbConnection_.tables().contains("trials"))
		sessionQ.exec("CREATE TABLE trials (id INTEGER PRIMARY KEY,trialnumber INTEGER,"
			"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
			"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
			"endjitter REAL, correlation REAL)");

	if(!baseSessionDbConnection_.tables().contains("behavioraldata"))
		sessionQ.exec("CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, xpos REAL, "
			"ypos REAL, time REAL)");

	if(!baseSessionDbConnection_.tables().contains("statetransitions"))
		sessionQ.exec("CREATE TABLE statetransitions (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, machinepath TEXT, "
			"source TEXT, sourceresult TEXT, destination TEXT, time REAL)");

	if(!baseSessionDbConnection_.tables().contains("framedata"))
		sessionQ.exec("CREATE TABLE framedata(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, frame INTEGER, time REAL, state TEXT)");

	if(!baseSessionDbConnection_.tables().contains("rewards"))
		sessionQ.exec("CREATE TABLE rewards (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, duration INTEGER, channel INTEGER, time REAL)");
}
void SessionInfo::CreateCacheDatabase(QString databaseName)
{
	//Create the cacheDB
	QString cacheDatabaseName = databaseName+"_cache";
	cacheDb_ = QSqlDatabase::addDatabase("QSQLITE",cacheDatabaseName);
	cacheDb_.setDatabaseName(":memory:");
	//baseCacheDbConnection_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + cacheDatabaseName + ".sqlite");
	Q_ASSERT(cacheDb_.open());

	QSqlQuery cacheQ(cacheDb_);
	Q_ASSERT(cacheQ.exec("CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, xpos REAL, "
		"ypos REAL, time REAL)"));
	Q_ASSERT(cacheQ.exec("CREATE TABLE framedata(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, frame INTEGER, time REAL, state TEXT)"));
}

//! \brief returns the maximum dataID from tableName.
double SessionInfo::LoadMaxDataID(QString tableName)
{
	QSqlDatabase sessionDb = getSessionDb();

	QSqlQuery sessionQuery(sessionDb);
	sessionQuery.prepare("SELECT MAX(dataid) FROM :tablename");
	sessionQuery.bindValue(":tablename", tableName);
	if(!sessionQuery.exec() && sessionQuery.next())
		return sessionQuery.value(0).toULongLong();
	return 0;
}

/*! \brief Generates a thread-specific connection to the Session database
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
	QString connectionName = QString("SessionDatabase_%1_%2")
					.arg(sessionId())
					.arg((int)QThread::currentThreadId());

	
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
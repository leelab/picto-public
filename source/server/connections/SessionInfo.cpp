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
	// Load Database Version
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"DatabaseVersion\"");
	if(sessionQ.next())
		databaseVersion_ = sessionQ.value(0).toString();
	sessionQ.finish();

	// Load SessionID
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"SessionID\"");
	if(sessionQ.next())
		uuid_ = QUuid(sessionQ.value(0).toString());
	sessionQ.finish();
	
	// Load ExperimentXML
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"ExperimentXML\"");
	if(sessionQ.next())
		experimentXml_ = sessionQ.value(0).toByteArray();
	sessionQ.finish();

	// Load TimeCreated
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"sessionstart\"");
	if(sessionQ.next())
		timeCreated_ = sessionQ.value(0).toString();
	sessionQ.finish();
	
	// Load Authorized Observers
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"AuthObserverID\"");
	while(sessionQ.next())
		authorizedObservers_.append(QUuid(sessionQ.value(0).toString()));
	sessionQ.finish();

	// Load Components
	QSharedPointer<ComponentInfo> component;
	executeReadQuery(&sessionQ,"SELECT id,address, name, type FROM componentinfo");
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
	sessionQ.finish();

	// Load AlignToType
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"AlignToComponent\"");
	if(sessionQ.next())
		alignToType_ = sessionQ.value(0).toString();
	sessionQ.finish();

	//Load latestTimeValues
	qulonglong maxID = 0;
	maxID = LoadMaxDataID("behavioralalignevents");
	if(maxID > maxReceivedDataID_) maxReceivedDataID_ = maxID;
	maxID = LoadMaxDataID("neuralalignevents");
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

	//Redoalignment and flush
	alignTimeBases(true);
	flushCache();
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
	executeWriteQuery(&query);
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
	executeWriteQuery(&query);
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
bool SessionInfo::endSession()
{
	//ConnectionManager *conMgr = ConnectionManager::Instance();
	//Q_ASSERT(conMgr->getComponentStatusBySession(uuid_,"DIRECTOR") > ComponentStatus::idle);


	QTime timer;
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		//Let the Component know that we want to end the session
		addPendingDirective("ENDSESSION",component->getType());
	}
	//Sit around waiting for the components' states to change
	timer.start();
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		while(component->getStatus() > ComponentStatus::ending)
		{
			//Keep waiting until either both components start the ending process
			while(component->getStatus() > ComponentStatus::ending && timer.elapsed() < 5000)
			{
				//Wait for it.
				QThread::yieldCurrentThread();
				QCoreApplication::processEvents();
			}
			if(component->getStatus() > ComponentStatus::ending)
			{	//Maybe it didn't get the message.
				//Try telling the component again
				addPendingDirective("ENDSESSION",component->getType());
				qDebug("SessionInfo::endSession",QString("%1 failed to start ending session within 5 seconds").arg(component->getName()).toAscii());
				timer.start();
			}
		}
	}
	//Now they're all either ending, idle, or not found.  Wait for them to finish the job.
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		while(component->getStatus() > ComponentStatus::idle)
		{
			//Wait for it.
			QThread::yieldCurrentThread();
			QCoreApplication::processEvents();
		}
	}

	QSqlDatabase sessionDb = getSessionDb();

	//Add the end time to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"sessionend\", :time)");
	query.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	executeWriteQuery(&query);
	
	
	//Realign timebases now that we have all the data
	alignTimeBases(true);

	//Flush the database cache
	flushCache();
	return true;
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
 *	backed).  This should be done before sending registered command responses so that
 *	components can be sure that when they receive a command response, the data from
 *	that command is permanently stored.  It should also be done before ending a session.
 */
void SessionInfo::flushCache(QString sourceType)
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery diskQuery(sessionDb);
	QSqlQuery cacheQuery(cacheDb);

	// If the cache database is empty, return immediately
	if(sourceType == "DIRECTOR")
	{
		executeReadQuery(&cacheQuery,"SELECT COUNT (*) FROM behavioraldata");
		cacheQuery.next();
		if(cacheQuery.value(0).toInt() < 1)
		{
			cacheQuery.finish();
			executeReadQuery(&cacheQuery,"SELECT COUNT (*) FROM framedata");
			cacheQuery.next();
			if(cacheQuery.value(0).toInt() < 1)
			{
				cacheQuery.finish();
				return;
			}
		}
		cacheQuery.finish();
	}
	else if(sourceType == "PROXY")
	{
		executeReadQuery(&cacheQuery,"SELECT COUNT (*) FROM spikes");
		cacheQuery.next();
		if(cacheQuery.value(0).toInt() < 1)
		{
			cacheQuery.finish();
			return;
		}
		cacheQuery.finish();
	}

	//Since we are encapsulating all of these operations in a single transaction, we put the whole thing in a mutex.
	QMutexLocker locker(databaseWriteMutex_.data());

	// Start Transaction
	bool success = cacheDb.transaction();
	Q_ASSERT_X(success,"SessionInfo::flushCache","Unable to start transaction: "+cacheDb.lastError().text().toAscii());

	// Put all values into move the data
	if((sourceType == "") || (sourceType == "DIRECTOR"))
	{
		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.behavioraldata(dataid, xpos,ypos,time) "
				"SELECT dataid,xpos,ypos,time FROM behavioraldata");
		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.framedata(dataid,frame,time,state) "
				"SELECT dataid,frame,time,state FROM framedata");

		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.behavioralalignevents(dataid,timestamp,aligncode,aligneventnumber,matched) "
				"SELECT dataid,timestamp,aligncode,aligneventnumber,matched FROM behavioralalignevents");
		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.alignevents(aligneventnumber,aligncode,neuraltime,"
			"behavioraltime,jitter,correlation)"
			"SELECT aligneventnumber,aligncode,neuraltime,behavioraltime,jitter,correlation FROM alignevents");

		// Clear cache
		executeWriteQuery(&cacheQuery,"DELETE FROM behavioraldata");
		executeWriteQuery(&cacheQuery,"DELETE FROM framedata");
		executeWriteQuery(&cacheQuery,"DELETE FROM behavioralalignevents WHERE matched<>0");
		executeWriteQuery(&cacheQuery,"DELETE FROM alignevents");
	}
	if ((sourceType == "") || (sourceType == "PROXY"))
	{
		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.spikes(dataid,timestamp,fittedtime,correlation,channel,unit,waveform) "
				"SELECT dataid,timestamp,fittedtime,correlation,channel,unit,waveform FROM spikes");
		executeWriteQuery(&cacheQuery,"INSERT INTO diskdb.neuralalignevents(dataid,timestamp,aligncode,matched) "
		"SELECT dataid,timestamp,aligncode,matched FROM neuralalignevents");
		// Clear cache
		executeWriteQuery(&cacheQuery,"DELETE FROM spikes");
		executeWriteQuery(&cacheQuery,"DELETE FROM neuralalignevents WHERE matched<>0");
	}

	// Commit the Transaction
	success = cacheDb.commit();
	if(!success)
	{
		int dbug = 1;
		dbug++;
	}
	Q_ASSERT_X(success,"SessionInfo::flushCache","Unable to commit transaction: "+cacheDb.lastError().text().toAscii());

	locker.unlock();	// The transaction is over so we can release this mutex
	return;
}

/*! \brief inserts a neural record in the session database
 */
void SessionInfo::insertNeuralData(Picto::NeuralDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	//QMutexLocker locker(&alignmentMutex_);
	//! Use the alignment tool to fit the time
	data.setFittedtime(alignmentTool_->convertToBehavioralTimebase(data.getTimestamp()));
	data.setCorrelation(alignmentTool_->getCorrelationCoefficient());
	//locker.unlock();
	
	query.prepare("INSERT INTO spikes (dataid, timestamp, fittedtime, correlation, channel, unit, waveform) "
		"VALUES(:dataid, :timestamp, :fittedtime, :correlation, :channel, :unit, :waveform)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":timestamp", data.getTimestamp());
	query.bindValue(":fittedtime", data.getFittedtime());
	query.bindValue(":correlation", data.getCorrelation());
	query.bindValue(":channel", data.getChannel());
	query.bindValue(":unit", data.getUnit());
	query.bindValue(":waveform", 1/*data.getWaveformAsString()*/);
	executeWriteQuery(&query);	
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertBehavioralData(Picto::BehavioralDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQ(cacheDb);

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
		executeWriteQuery(&cacheQ);	
	}
}

//! \brief inserts an alignment data point in the database and attempts latest alignment
void SessionInfo::insertAlignmentData(Picto::AlignmentDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);
	// If the alignevent comes with an alignNumber, it must be from the component providing the time baseline, 
	// put it in behavioralignevents and don't align its timestamps.
	if(data.hasAlignNumber())
	{
		query.prepare("INSERT INTO behavioralalignevents (dataid,timestamp,aligncode,aligneventnumber,matched)"
			"VALUES(:dataid, :timestamp, :aligncode, :aligneventnumber, 0)");
		query.bindValue(":dataid", data.getDataID());
		query.bindValue(":timestamp", data.getTimestamp());
		query.bindValue(":aligncode", data.getAlignCode());
		query.bindValue(":aligneventnumber", data.getAlignNumber());
		executeWriteQuery(&query);
		return;
	}
	//If we got here, its not coming from the component providing the time baseline.
	//Add the event code to the neuralalignevents table
	query.prepare("INSERT INTO neuralalignevents (dataid, timestamp, aligncode, matched)"
		"VALUES(:dataid, :timestamp, :aligncode, 0)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":timestamp", data.getTimestamp());
	query.bindValue(":aligncode", data.getAlignCode());
	executeWriteQuery(&query);	

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
		alignTimeBases();
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
	executeWriteQuery(&query);	
}

//! \brief Inserts the passed in frame data into the cache database
void SessionInfo::insertFrameData(Picto::FrameDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQ(cacheDb);

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
		executeWriteQuery(&cacheQ);	
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
	executeWriteQuery(&query);	
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
	QSqlDatabase cacheDb = getCacheDb();
	Picto::BehavioralDataStore dataStore;
	QSqlQuery query(cacheDb);
	bool justFirst = false;
	if(timestamp < 0)
	{
		justFirst = true;
		timestamp = 0;
		query.prepare("SELECT xpos, ypos, time FROM behavioraldata WHERE time > :time1 UNION "
		"SELECT xpos, ypos, time FROM diskdb.behavioraldata WHERE time > :time2 ORDER BY time DESC");
	}
	else
	{
		if(timestamp == 0)
			timestamp = -1;
		query.prepare("SELECT xpos, ypos, time FROM behavioraldata WHERE time > :time1 UNION "
		"SELECT diskdb.behavioraldata.xpos, diskdb.behavioraldata.ypos, diskdb.behavioraldata.time "
		"FROM diskdb.behavioraldata WHERE time > :time2 ORDER BY time ASC");
	}
	
	query.bindValue(":time1",timestamp);
	query.bindValue(":time2",timestamp);
	executeReadQuery(&query,"",true);
	if(justFirst && query.next())
	{
		dataStore.addData(query.value(0).toDouble(),
						  query.value(1).toDouble(),
						  query.value(2).toDouble());
		query.finish();
	}
	else
	{
		while(query.next())
		{
			dataStore.addData(query.value(0).toDouble(),
							  query.value(1).toDouble(),
							  query.value(2).toDouble());
		}
		query.finish();
	}
	return dataStore;

	//QSqlDatabase sessionDb = getSessionDb();
	//QSqlDatabase cacheDb = getCacheDb();
	//Picto::BehavioralDataStore dataStore;
	//
	////If the timestamp is -1, we only need to return the most recent behavioral data
	//if(timestamp < 0)
	//{
	//	//check the cache database first
	//	QSqlQuery cacheQuery(cacheDb);

	//	cacheQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
	//		"ORDER BY time DESC");
	//	executeReadQuery(&cacheQuery);

	//	if(cacheQuery.next())
	//	{
	//		dataStore.addData(cacheQuery.value(0).toDouble(),
	//						  cacheQuery.value(1).toDouble(),
	//						  cacheQuery.value(2).toDouble());
	//		cacheQuery.finish();
	//	}
	//	else
	//	{
	//		cacheQuery.finish();
	//		//If there wasn't anything in the cache db, pull from the session db
	//		QSqlQuery sessionQuery;
	//		sessionQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
	//			"ORDER BY time DESC");
	//		executeReadQuery(&sessionQuery);

	//		if(sessionQuery.next())
	//		{
	//			dataStore.addData(cacheQuery.value(0).toDouble(),
	//							  cacheQuery.value(1).toDouble(),
	//							  cacheQuery.value(2).toDouble());
	//		}
	//		sessionQuery.finish();
	//	}
	//}
	//else
	//{
	//	//First, we attempt to pull data from the session database.  This will 
	//	//likely return an empty query.
	//	QSqlQuery sessionQuery(sessionDb);
	//	sessionQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
	//		"WHERE time > :time ORDER BY time ASC");
	//	if(timestamp == 0)
	//		sessionQuery.bindValue(":time",-1);
	//	else
	//		sessionQuery.bindValue(":time", timestamp);
	//	executeReadQuery(&sessionQuery);

	//	while(sessionQuery.next())
	//	{
	//		dataStore.addData(sessionQuery.value(0).toDouble(),
	//						  sessionQuery.value(1).toDouble(),
	//						  sessionQuery.value(2).toDouble());
	//	}
	//	sessionQuery.finish();


	//	QSqlQuery cacheQuery(cacheDb);

	//	cacheQuery.prepare("SELECT xpos, ypos, time FROM behavioraldata "
	//		"WHERE time > :time ORDER BY time ASC");
	//	if(timestamp == 0)
	//		cacheQuery.bindValue(":time",-1);
	//	else
	//		cacheQuery.bindValue(":time", timestamp);
	//	executeReadQuery(&cacheQuery);

	//	while(cacheQuery.next())
	//	{
	//		dataStore.addData(cacheQuery.value(0).toDouble(),
	//						  cacheQuery.value(1).toDouble(),
	//						  cacheQuery.value(2).toDouble());
	//	}
	//	cacheQuery.finish();
	//}

	//return dataStore;
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
	QSqlDatabase cacheDb = getCacheDb();
	Picto::FrameDataStore dataStore;
	QSqlQuery query(cacheDb);
	bool justFirst = false;
	if(timestamp < 0)
	{
		justFirst = true;
		timestamp = 0;
		query.prepare("SELECT frame,time,state FROM framedata WHERE time > :time1 UNION "
		"SELECT diskdb.framedata.frame,diskdb.framedata.time,diskdb.framedata.state FROM diskdb.framedata WHERE time > :time2 ORDER BY time DESC");
	}
	else
	{
		if(timestamp == 0)
			timestamp = -1;
		query.prepare("SELECT frame,time,state FROM framedata WHERE time > :time1 UNION "
		"SELECT diskdb.framedata.frame,diskdb.framedata.time,diskdb.framedata.state FROM diskdb.framedata WHERE time > :time2 ORDER BY time ASC");
	}
	
	query.bindValue(":time1",timestamp);
	query.bindValue(":time2",timestamp);
	executeReadQuery(&query,"",true);
	if(justFirst && query.next())
	{
		dataStore.addFrame(query.value(0).toInt(),
						  query.value(1).toDouble(),
						  query.value(2).toString());
		query.finish();
	}
	else
	{
		while(query.next())
		{
			dataStore.addFrame(query.value(0).toInt(),
							   query.value(1).toDouble(),
							   query.value(2).toString());
		}
		query.finish();
	}
	return dataStore;

	//QSqlDatabase sessionDb = getSessionDb();
	//QSqlDatabase cacheDb = getCacheDb();
	//Picto::FrameDataStore dataStore;

	//if(timestamp < 0)
	//{
	//	//check the cache db first
	//	QSqlQuery cacheQuery(cacheDb);
	//	cacheQuery.prepare("SELECT frame,time,state FROM framedata "
	//		"ORDER BY time DESC");
	//	executeReadQuery(&cacheQuery);

	//	if(cacheQuery.next())
	//	{
	//		dataStore.addFrame(cacheQuery.value(0).toInt(),
	//						  cacheQuery.value(1).toDouble(),
	//						  cacheQuery.value(2).toString());
	//		cacheQuery.finish();
	//	}
	//	else
	//	{
	//		cacheQuery.finish();
	//		//There was nothing in the cache db, so we need to pull from the session db
	//		QSqlQuery sessionQuery(sessionDb);	
	//		sessionQuery.prepare("SELECT frame,time,state FROM framedata "
	//			"ORDER BY time DESC");
	//		executeReadQuery(&sessionQuery);

	//		if(sessionQuery.next())
	//		{
	//			dataStore.addFrame(cacheQuery.value(0).toInt(),
	//							  cacheQuery.value(1).toDouble(),
	//							  cacheQuery.value(2).toString());
	//		}
	//		sessionQuery.finish();
	//	}

	//}
	//else
	//{
	//	//First, we attempt to pull data from the session database.  This will 
	//	//likely return an empty query.
	//	QSqlQuery sessionQuery(sessionDb);
	//	sessionQuery.prepare("SELECT frame,time,state FROM framedata "
	//		"WHERE time > :time ORDER BY time ASC");
	//	if(timestamp == 0)
	//		sessionQuery.bindValue(":time",-1);
	//	else
	//		sessionQuery.bindValue(":time", timestamp);
	//	executeReadQuery(&sessionQuery);

	//	while(sessionQuery.next())
	//	{
	//		dataStore.addFrame(sessionQuery.value(1).toInt(),
	//						   sessionQuery.value(2).toDouble(),
	//						   sessionQuery.value(3).toString());
	//	}
	//	sessionQuery.finish();


	//	QSqlQuery cacheQuery(cacheDb);
	//	cacheQuery.prepare("SELECT frame,time,state FROM framedata "
	//		"WHERE time > :time ORDER BY time ASC");
	//	if(timestamp == 0)
	//		cacheQuery.bindValue(":time",-1);
	//	else
	//		cacheQuery.bindValue(":time", timestamp);
	//	executeReadQuery(&cacheQuery);

	//	while(cacheQuery.next())
	//	{
	//		dataStore.addFrame(cacheQuery.value(0).toInt(),
	//						  cacheQuery.value(1).toDouble(),
	//						  cacheQuery.value(2).toString());
	//	}
	//	cacheQuery.finish();
	//}
	//return dataStore;

}


/*! \brief Returns a list of state datastores with all of the data collected after the timestamp
 *
 *	This function creates a list of state data stores and returns it.  If the timestamp is 0, 
 *	then all data is returned.
 */
QList<Picto::StateDataStore> SessionInfo::selectStateData(double timestamp)
{

	QSqlDatabase cacheDb = getCacheDb();
	QList<Picto::StateDataStore> dataStoreList;
	QSqlQuery query(cacheDb);
	bool justFirst = false;
	if(timestamp < 0)
	{
		justFirst = true;
		timestamp = -1;
		query.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions WHERE time > :time1 UNION "
		"SELECT diskdb.statetransitions.source, diskdb.statetransitions.sourceresult, diskdb.statetransitions.destination, diskdb.statetransitions.time, "
		"diskdb.statetransitions.machinepath FROM diskdb.statetransitions WHERE time > :time2 ORDER BY time DESC");
	}
	else
	{
		if(timestamp == 0)
			timestamp = -1;
		query.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions WHERE time > :time1 UNION "
		"SELECT diskdb.statetransitions.source, diskdb.statetransitions.sourceresult, diskdb.statetransitions.destination, "
		"diskdb.statetransitions.time, diskdb.statetransitions.machinepath FROM diskdb.statetransitions WHERE time > :time2 ORDER BY time ASC");
	}
	
	query.bindValue(":time1",timestamp);
	query.bindValue(":time2",timestamp);
	executeReadQuery(&query,"",true);
	if(justFirst && query.next())
	{
		Picto::StateDataStore data;
		data.setTransition(query.value(0).toString(),
						   query.value(1).toString(),
						   query.value(2).toString(),
						   query.value(3).toDouble(),
						   query.value(4).toString());
		dataStoreList.append(data);
		query.finish();
	}
	else
	{
		while(query.next())
		{
			Picto::StateDataStore data;
			data.setTransition(query.value(0).toString(),
							   query.value(1).toString(),
							   query.value(2).toString(),
							   query.value(3).toDouble(),
							   query.value(4).toString());
			dataStoreList.append(data);
		}
		query.finish();
	}
	return dataStoreList;

	//QSqlDatabase sessionDb = getSessionDb();
	//QList<Picto::StateDataStore> stateDataList;

	//if(timestamp < 0)
	//{
	//	QSqlQuery sessionQuery(sessionDb);
	//	sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions  "
	//		"ORDER BY time DESC");
	//	executeReadQuery(&sessionQuery);

	//	if(sessionQuery.next())
	//	{
	//		Picto::StateDataStore data;
	//		data.setTransition(sessionQuery.value(0).toString(),
	//						   sessionQuery.value(1).toString(),
	//						   sessionQuery.value(2).toString(),
	//						   sessionQuery.value(3).toDouble(),
	//						   sessionQuery.value(4).toString());
	//		stateDataList.append(data);
	//	}
	//	sessionQuery.finish();
	//	//If there is no data, that's OK  (This happens when we join a session that hasn't started running yet)
	//}
	//else
	//{
	//	QSqlQuery sessionQuery(sessionDb);
	//	sessionQuery.prepare("SELECT source, sourceresult, destination, time, machinepath FROM statetransitions  "
	//		"WHERE time > :time ORDER BY time ASC");
	//	if(timestamp == 0)
	//		sessionQuery.bindValue(":time",-1);
	//	else
	//		sessionQuery.bindValue(":time", timestamp);
	//	executeReadQuery(&sessionQuery);

	//	while(sessionQuery.next())
	//	{
	//		Picto::StateDataStore data;
	//		data.setTransition(sessionQuery.value(0).toString(),
	//						   sessionQuery.value(1).toString(),
	//						   sessionQuery.value(2).toString(),
	//						   sessionQuery.value(3).toDouble(),
	//						   sessionQuery.value(4).toString());
	//		stateDataList.append(data);
	//	}
	//	sessionQuery.finish();
	//}

	//return stateDataList;

}

void SessionInfo::InitializeVariables()
{
	databaseWriteMutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	activity_ = true;
	timestampsAligned_ = (false);
	//CreateUUID
	if(uuid_ == QUuid())
	{
		uuid_ = QUuid::createUuid();
	}
	maxReceivedDataID_ = 0;
	databaseVersion_ = "1.0";

#ifdef NO_AUTH_REQUIRED
	//Add a null QUuid so that everyone is considered an authorized user
	authorizedObservers_.append(QUuid());
#endif
	latestNeuralTimestamp_ = 0;
	latestBehavioralTimestamp_ = 0;
	//create alignment tool
	alignToType_ = "";
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
		executeWriteQuery(&sessionQ,"CREATE TABLE sessioninfo(id INTEGER PRIMARY KEY,"
			"key TEXT, value TEXT)");
		//Add the Database Version
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"DatabaseVersion\", :databaseversion)");
		sessionQ.bindValue(":databaseversion",databaseVersion_);
		executeWriteQuery(&sessionQ);
		//Add the current time
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"sessionstart\", :time)");
		timeCreated_ = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
		sessionQ.bindValue(":time", timeCreated_);
		executeWriteQuery(&sessionQ);	
		//Add the session ID
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"SessionID\", :id)");
		sessionQ.bindValue(":id", uuid_.toString());
		executeWriteQuery(&sessionQ);
		//Add authorized observers
		foreach(QUuid observer, authorizedObservers_)
		{
			sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"AuthObserverID\", :id)");
			sessionQ.bindValue(":id", observer.toString());
			executeWriteQuery(&sessionQ);
		}
		//Add the experimentXML
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"ExperimentXML\", :xml)");
		sessionQ.bindValue(":xml", QString(experimentXml_));
		executeWriteQuery(&sessionQ);
	}

	if(!baseSessionDbConnection_.tables().contains("componentinfo"))
		executeWriteQuery(&sessionQ,"CREATE TABLE componentinfo(id,"
			"address TEXT, name TEXT, type TEXT)");
	

	if(!baseSessionDbConnection_.tables().contains("spikes"))
		executeWriteQuery(&sessionQ,"CREATE TABLE spikes "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, fittedtime REAL,"
			"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");

	// For NeuralAlignEvents, the matched value may change and a alignevent be reinserted.  For this reason we use ON CONFLIG REPLACE on the dataid
	if(!baseSessionDbConnection_.tables().contains("neuralalignevents"))
		executeWriteQuery(&sessionQ,"CREATE TABLE neuralalignevents "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT REPLACE, timestamp REAL, aligncode INTEGER,"
			"matched INTEGER)");

	// For BehavioralAlignEvents, the matched value may change and a alignevent be reinserted.  For this reason we use ON CONFLIG REPLACE on the dataid
	if(!baseSessionDbConnection_.tables().contains("behavioralalignevents"))
		executeWriteQuery(&sessionQ,"CREATE TABLE behavioralalignevents "
			"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT REPLACE, timestamp REAL, aligncode INTEGER, "
			"aligneventnumber INTEGER, matched INTEGER)");

	if(!baseSessionDbConnection_.tables().contains("alignevents"))
		executeWriteQuery(&sessionQ,"CREATE TABLE alignevents (id INTEGER PRIMARY KEY,aligneventnumber INTEGER UNIQUE ON CONFLICT IGNORE,"
			"aligncode INTEGER, neuraltime REAL,behavioraltime REAL, jitter REAL, correlation REAL)");

	if(!baseSessionDbConnection_.tables().contains("behavioraldata"))
		executeWriteQuery(&sessionQ,"CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, xpos REAL, "
			"ypos REAL, time REAL)");

	if(!baseSessionDbConnection_.tables().contains("statetransitions"))
		executeWriteQuery(&sessionQ,"CREATE TABLE statetransitions (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, machinepath TEXT, "
			"source TEXT, sourceresult TEXT, destination TEXT, time REAL)");

	if(!baseSessionDbConnection_.tables().contains("framedata"))
		executeWriteQuery(&sessionQ,"CREATE TABLE framedata(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, frame INTEGER, time REAL, state TEXT)");

	if(!baseSessionDbConnection_.tables().contains("rewards"))
		executeWriteQuery(&sessionQ,"CREATE TABLE rewards (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, duration INTEGER, channel INTEGER, time REAL)");
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

	executeWriteQuery(&cacheQ,"CREATE TABLE behavioraldata (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, xpos REAL, "
		"ypos REAL, time REAL)");
	executeWriteQuery(&cacheQ,"CREATE TABLE framedata(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, frame INTEGER, time REAL, state TEXT)");
	executeWriteQuery(&cacheQ,"CREATE TABLE spikes (id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, fittedtime REAL,"
			"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");
	//If a NeuralAlignEvent is reinserted it means that the data was resent.  For this reason, in the cache table, we use ON CONFLICT IGNORE for dataid
	executeWriteQuery(&cacheQ,"CREATE TABLE neuralalignevents "
		"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, aligncode INTEGER,"
		"matched INTEGER)");
	//If a BehavioralAlignEvent is reinserted it means that the data was resent.  For this reason, in the cache table, we use ON CONFLICT IGNORE for dataid
	executeWriteQuery(&cacheQ,"CREATE TABLE behavioralalignevents "
		"(id INTEGER PRIMARY KEY, dataid INTEGER UNIQUE ON CONFLICT IGNORE, timestamp REAL, aligncode INTEGER, "
		"aligneventnumber INTEGER, matched INTEGER)");
	executeWriteQuery(&cacheQ,"CREATE TABLE alignevents (id INTEGER PRIMARY KEY,aligneventnumber INTEGER UNIQUE ON CONFLICT IGNORE,"
		"aligncode INTEGER, neuraltime REAL,behavioraltime REAL, jitter REAL, correlation REAL)");

	// Attach the disk backed database to the database connection so that we can easily access both from the same query.
	cacheQ.prepare("ATTACH DATABASE :databaseName AS diskdb");
	cacheQ.bindValue(":databaseName", baseSessionDbConnection_.databaseName());
	executeWriteQuery(&cacheQ);
}

//! \brief returns the maximum dataID from tableName.
double SessionInfo::LoadMaxDataID(QString tableName)
{
	QSqlDatabase sessionDb = getSessionDb();

	QSqlQuery sessionQuery(sessionDb);
	sessionQuery.prepare("SELECT MAX(dataid) FROM :tablename");
	sessionQuery.bindValue(":tablename", tableName);
	if(!executeReadQuery(&sessionQuery) && sessionQuery.next())
	{
		sessionQuery.finish();
		return sessionQuery.value(0).toULongLong();
	}
	sessionQuery.finish();
	return 0;
}

bool SessionInfo::executeReadQuery(QSqlQuery* query, QString optionalString, bool debug)
{
	QMutexLocker locker(databaseWriteMutex_.data());
	bool success;
	if(optionalString != "")
		success = query->exec(optionalString);
	else
		success = query->exec();
	locker.unlock();
	Q_ASSERT_X(!debug || success,"SessionInfo::executeWriteQuery","Error: "+query->lastError().text().toAscii());
	return success;
}

//! \brief Executes a Sql write query, making it threadsafe and including debug assertion
bool SessionInfo::executeWriteQuery(QSqlQuery* query, QString optionalString,bool debug)
{
	QMutexLocker locker(databaseWriteMutex_.data());
	bool success;
	if(optionalString != "")
		success = query->exec(optionalString);
	else
		success = query->exec();
	query->finish();
	locker.unlock();
	if(debug && !success)
	{
		int debugInt = 0;
		debugInt++;
	}
	Q_ASSERT_X(!debug || success,"SessionInfo::executeWriteQuery","Error: "+query->lastError().text().toAscii());
	return success;
}

void SessionInfo::alignTimeBases(bool realignAll)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);
	QSqlQuery updateNeuralQuery(cacheDb);
	QSqlQuery updateBehavioralQuery(cacheDb);
	QSqlQuery insertAlignEventQuery(cacheDb);
	if(realignAll)
	{
		// Move everything back into the cache, set it all as unmatched, and delete all previous alignevents
		flushCache();
		executeWriteQuery(&query,"INSERT INTO neuralalignevents(dataid,timestamp,aligncode,matched) "
		"SELECT diskdb.neuralalignevents.dataid,diskdb.neuralalignevents.timestamp,diskdb.neuralalignevents.aligncode,diskdb.neuralalignevents.matched FROM diskdb.neuralalignevents");
		executeWriteQuery(&query,"INSERT INTO behavioralalignevents(dataid,timestamp,aligncode,aligneventnumber,matched) "
		"SELECT diskdb.behavioralalignevents.dataid,diskdb.behavioralalignevents.timestamp,diskdb.behavioralalignevents.aligncode,diskdb.behavioralalignevents.aligneventnumber,diskdb.behavioralalignevents.matched FROM diskdb.behavioralalignevents");
		executeWriteQuery(&query,"UPDATE behavioralalignevents SET matched=0");
		executeWriteQuery(&query,"UPDATE neuralalignevents SET matched=0");
		executeWriteQuery(&query,"DELETE FROM alignevents");
		executeWriteQuery(&query,"DELETE FROM diskdb.alignevents");
		latestBehavioralTimestamp_ = 0;
		latestNeuralTimestamp_ = 0;
		alignmentTool_->resetValues();
	}

	// We ignore any align codes that are sent out of order.  Set them all to ignored.
	query.prepare("UPDATE neuralalignevents SET matched = -1 WHERE timestamp < :time AND matched <> 1");
	query.bindValue(":time",latestNeuralTimestamp_);
	executeWriteQuery(&query);
	query.prepare("UPDATE behavioralalignevents SET matched = -1 WHERE timestamp < :time AND matched <> 1");
	query.bindValue(":time",latestBehavioralTimestamp_);
	executeWriteQuery(&query);

	// Find all the places where the aligncodes match on the lists, order by behavioral, then neural timestamp.
	executeReadQuery(&query,"SELECT behavioralalignevents.timestamp, neuralalignevents.timestamp, behavioralalignevents.id, neuralalignevents.id, behavioralalignevents.aligncode, behavioralalignevents.aligneventnumber "
							"FROM behavioralalignevents JOIN neuralalignevents ON behavioralalignevents.aligncode=neuralalignevents.aligncode WHERE behavioralalignevents.matched=0 AND neuralalignevents.matched=0 "
							"ORDER BY behavioralalignevents.timestamp,neuralalignevents.timestamp ASC",true);
	// Go through the list.  Use the first alignable value for each aligncode, throwout any subsequent values for the same aligncode.
	double behavTime = latestBehavioralTimestamp_;
	double neuralTime = latestNeuralTimestamp_;
	updateNeuralQuery.prepare("UPDATE neuralalignevents SET matched=1 WHERE id=:id");
	updateBehavioralQuery.prepare("UPDATE behavioralalignevents SET matched=1 WHERE id=:id");
	insertAlignEventQuery.prepare(	"INSERT INTO alignevents(aligneventnumber,aligncode,neuraltime,"
								"behavioraltime,jitter,correlation) "
								"VALUES(:aligneventnumber,:aligncode,:neuraltime, "
								":behavioraltime,:jitter,:correlation)");
	bool hadData = false;
	while(query.next())
	{
		hadData = true;
		behavTime = query.value(0).toDouble();
		neuralTime = query.value(1).toDouble();
		if((behavTime <= latestBehavioralTimestamp_)
			|| (neuralTime <= latestNeuralTimestamp_))
			// We already matched this aligncode
			continue;
		latestBehavioralTimestamp_ = behavTime;
		latestNeuralTimestamp_ = neuralTime;
		alignmentTool_->updateCoefficients(latestBehavioralTimestamp_,latestNeuralTimestamp_);
		updateBehavioralQuery.bindValue(":id",query.value(2));
		updateNeuralQuery.bindValue(":id",query.value(3));
		insertAlignEventQuery.bindValue(":aligneventnumber",query.value(5));
		insertAlignEventQuery.bindValue(":aligncode",query.value(4));
		insertAlignEventQuery.bindValue(":neuraltime",latestNeuralTimestamp_);
		insertAlignEventQuery.bindValue(":behavioraltime",latestBehavioralTimestamp_);
		insertAlignEventQuery.bindValue(":jitter",alignmentTool_->getJitter(latestBehavioralTimestamp_,latestNeuralTimestamp_));
		insertAlignEventQuery.bindValue(":correlation",alignmentTool_->getCorrelationCoefficient());
		executeWriteQuery(&updateBehavioralQuery);
		executeWriteQuery(&updateNeuralQuery);
		executeWriteQuery(&insertAlignEventQuery);
	}
	query.finish();
	// Update all jitter and correlation values with the latest calculated alignment coefficients.
	executeWriteQuery(&query,(QString("UPDATE alignevents SET ")+alignmentTool_->getSQLJitterEquation("jitter","neuraltime","behavioraltime","correlation")).toAscii());
	if(realignAll || (hadData && !timestampsAligned_))
	{
		recalculateFittedTimes();
		if(hadData)
			timestampsAligned_ = true;
	}

}

void SessionInfo::recalculateFittedTimes()
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);
	flushCache();
	QString queryString = "UPDATE spikes SET "+alignmentTool_->getSQLTimeConverstionEquation("fittedtime","timestamp","correlation");
	executeWriteQuery(&query,queryString);
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

/*! \brief Returns a connection to the Cache database
 */

QSqlDatabase SessionInfo::getCacheDb()
{
	return cacheDb_;
}
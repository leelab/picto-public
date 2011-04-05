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
#include <QSqlRecord>
//This turns on and off the authorized user permission setup on SessionInfo
//#define NO_AUTH_REQUIRED
QMap<QUuid,QWeakPointer<SessionInfo>> SessionInfo::loadedSessions_;
QSharedPointer<SessionInfo> SessionInfo::CreateSession(QByteArray experimentXml, QUuid initialObserverId)
{
	//Creates a shared pointer for the object that will use the deleteSession function to actually delete
	//the object
	QSharedPointer<SessionInfo> returnVal(new SessionInfo(experimentXml,initialObserverId),&deleteSession);
	loadedSessions_[returnVal->sessionId()] = QWeakPointer<SessionInfo>(returnVal);
	return returnVal;
}

QSharedPointer<SessionInfo> SessionInfo::LoadSession(QString sessionID, QString databaseFilePath)
{
	QSharedPointer<SessionInfo> returnVal;
	if(loadedSessions_.contains(QUuid(sessionID)))
	{
		//Get a shared pointer to the session
		returnVal = QSharedPointer<SessionInfo>(loadedSessions_[QUuid(sessionID)]);
		//Checking if returnVal is null below will verify that the session is still there just 
		//in case it was deleted during the previous call 
		//(the fact that loadedSessions_ contains weak pointers takes care of this issue)
	}
	if(returnVal.isNull())
		returnVal = QSharedPointer<SessionInfo>(new SessionInfo(databaseFilePath),&deleteSession);
	return returnVal;
}

/*!
 * \brief Handles session deletion while maintaining a list of loaded sessions
 *	If a session is removed from the openSessions list by the ConnectionManager but
 *	still hasn't been deleted since something with a shared pointer is stuck in a long
 *	operation, we want to make sure that nothing else reopens the session from file
 *	before the loaded session actually gets deleted.  To do this, we maintain a 
 *	loadedSessions_ list that contains QWeakPointers to all currently loaded sessions.  
 *	When LoadSession is called, this list is checked first and if a session is found 
 *	with the input sessionID, the weak reference is converted to a regular shared pointer
 *	and returned.  Viola!  No duplicate copies of sessions.
 */
void SessionInfo::deleteSession(SessionInfo* session)
{
	if(loadedSessions_.contains(session->sessionId()))
		loadedSessions_.remove(session->sessionId());
	delete session;
}

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
	QMutexLocker locker(databaseWriteMutex_.data());
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
	executeReadQuery(&sessionQ,"SELECT componentid,address, name, type, active FROM componentinfo");
	while(sessionQ.next())
	{
		component = QSharedPointer<ComponentInfo>(new ComponentInfo());
		component->setUuid(QUuid(sessionQ.value(0).toString()));
		component->setAddress(sessionQ.value(1).toString());
		component->setName(sessionQ.value(2).toString());
		component->setType(sessionQ.value(3).toString());
		component->setStatus(ComponentStatus::notFound);
		components_[component->getType()] = component;
		componentActivity_[component->getUuid()] = sessionQ.value(4).toBool();
	}
	sessionQ.finish();

	// Load AlignToType
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"AlignToComponent\"");
	if(sessionQ.next())
		alignToType_ = sessionQ.value(0).toString();
	sessionQ.finish();
	locker.unlock();

	////Redoalignment and flush
	////I removed this because it isn't strictly necessary (since we realign when we end the session)
	////and it adds a lot of time to session loading, which is something that can occur frequently in
	////certain situations
	//alignTimeBases(true);
	//flushCache();
}

SessionInfo::~SessionInfo()
{
	qDebug("Session: " + sessionId().toString().toAscii() + " is destructing!");
}

//! \brief Adds a component to this session (ie. Director or proxy)
void SessionInfo::AddComponent(QSharedPointer<ComponentInfo> component)
{
	QSqlDatabase sessionDb = getSessionDb();
	//Add the component to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO componentinfo(componentid,address, name, type, active)"
		" VALUES (:componentid,:address,:name,:type, :active)");
	query.bindValue(":componentid", component->getUuid().toString());
	query.bindValue(":address", component->getAddress());
	query.bindValue(":name", component->getName());
	query.bindValue(":type", component->getType());
	query.bindValue(":active", false);
	executeWriteQuery(&query);
	components_[component->getType()] = component;
}

void SessionInfo::UpdateComponentActivity()
{
	QSqlDatabase sessionDb = getSessionDb();
	//Add the component to the session db
	QSqlQuery query(sessionDb);
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		//If the component isn't found, it may be temporarily disconnected but still running, so don't change anything.
		if(component->getStatus() != ComponentStatus::notFound)
		{
			bool active = component->getSessionID() == sessionId();
			if(componentActivity_[component->getUuid()] != active)
			{
				query.prepare("UPDATE componentinfo SET active=:active WHERE componentid=:componentid");
				query.bindValue(":active",active);
				query.bindValue(":componentid",component->getUuid().toString());
				executeWriteQuery(&query);
				componentActivity_[component->getUuid()] = active;
			}
		}
	}
}

bool SessionInfo::hasActiveComponents()
{
	if(ignoreComponents_)
		return false;
	for(QMap<QUuid,bool>::iterator iter = componentActivity_.begin();iter != componentActivity_.end();iter++)
	{
		if(iter.value())
			return true;
	}
	return false;
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


	//QTime timer;
	//foreach(QSharedPointer<ComponentInfo> component,components_)
	//{
	//	//Let the Component know that we want to end the session
	//	addPendingDirective("ENDSESSION",component->getType());
	//}
	////Sit around waiting for the components' states to change. If a component is not found, also sitting
	////around.  The session will end up timing out, and we don't want to end if there may still be some 
	////data coming in.
	//timer.start();
	//foreach(QSharedPointer<ComponentInfo> component,components_)
	//{
	//	while((component->getStatus() == ComponentStatus::notFound) 
	//		|| (component->getStatus() > ComponentStatus::ending))
	//	{
	//		//Keep waiting until either both components start the ending process
	//		while(timer.elapsed() < 5000)
	//		{
	//			//Wait for it.
	//			QThread::yieldCurrentThread();
	//			QCoreApplication::processEvents();
	//		}
	//		if(component->getStatus() > ComponentStatus::ending)
	//		{	//Maybe it didn't get the message.
	//			//Try telling the component again
	//			addPendingDirective("ENDSESSION",component->getType());
	//			qDebug("SessionInfo::endSession",QString("%1 failed to start ending session within 5 seconds").arg(component->getName()).toAscii());
	//			timer.start();
	//		}
	//	}
	//}
	////Now they're all either ending, idle, or not found.  Wait for them to finish the job.
	//foreach(QSharedPointer<ComponentInfo> component,components_)
	//{
	//	while( (component->getStatus() == ComponentStatus::notFound)
	//		|| (component->getStatus() > ComponentStatus::idle))
	//	{
	//		//Wait for it.
	//		QThread::yieldCurrentThread();
	//		QCoreApplication::processEvents();
	//	}
	//}

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
	qDebug("Session: " + sessionId().toString().toAscii() + " Ended!");
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
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQuery(cacheDb);

	//Since we are encapsulating all of these operations in a single transaction, we put the whole thing in a mutex.
	QMutexLocker locker(databaseWriteMutex_.data());
	QTime timer;
	timer.start();

	bool success;
	int attempt = 0;
	do
	{
		//Start transaction
		success = cacheDb.transaction();
		if(!success)
			continue;

		// Loop through all tables.  If this sourceType is responsible for that table, flush its contents to the diskdb and erase cache.
		// In the case of the neural/behavioralalignevents, don't erase unvisited, unmatched alignevents
		foreach(QString table,tables_)
		{
			if(tableDataProviders_.contains(table) && ((sourceType == "") || (tableDataProviders_.value(table)==sourceType)) )
			{
				QString queryString = "INSERT INTO diskdb.%1(%2) SELECT %2 FROM %1";
				executeWriteQuery(&cacheQuery,QString("INSERT INTO diskdb.%1(%2) SELECT %2 FROM %1").arg(table).arg(tableColumns_[table]));
				executeWriteQuery(&cacheQuery,QString("DELETE FROM %1%2").arg(table).arg(table.contains("ralalignevents")?" WHERE matched<>0":""));
			}
		}
		cacheQuery.clear();
		//End Transaction
		success = cacheDb.commit();
		//If Transaction failed, rollback and try again.
		if(!success)
		{
			qDebug("Flush Cache failed for session: " + sessionId().toString().toAscii() + " on attempt: " 
				+ QString::number(attempt++).toAscii() + " rolling back transaction and reattempting.");
			cacheDb.rollback();
		}
	}while(!success);

	locker.unlock();	// The transaction is over so we can release this mutex
	qDebug("Time to flush cache: " + QString::number(timer.elapsed()).toAscii() + "ms");
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
	query.bindValue(":waveform", data.getWaveformAsString());
	executeWriteQuery(&query,"",false);	
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
		executeWriteQuery(&cacheQ,"",false);	
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
		executeWriteQuery(&query,"",false);
		return;
	}
	//If we got here, its not coming from the component providing the time baseline.
	//Add the event code to the neuralalignevents table
	query.prepare("INSERT OR IGNORE INTO neuralalignevents (dataid, timestamp, aligncode, matched)"
		"VALUES(:dataid, :timestamp, :aligncode, 0)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":timestamp", data.getTimestamp());
	query.bindValue(":aligncode", data.getAlignCode());
	executeWriteQuery(&query,"",false);	

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
		alignTimeBases();
}

////! \brief inserts an LFP data point in the cache database
//void SessionInfo::insertLFPData(Picto::LFPDataStore data)
//{
//	QSqlDatabase cacheDb = getCacheDb();
//	QSqlQuery cacheQ(cacheDb);
//
//	// Create a 2D matrix with the lfp data (ie. each row is a channel, each column is a new time.
//	QSharedPointer<QList<Picto::LFPDataPoint>> dataPoints = data.getData();
//	QVector<QVector<int>> dataMatrix;
//	QVector<QString> channels;
//	QString columns;
//	int maxRowLength = 0;
//	for(QList<Picto::LFPDataPoint>::iterator iter = dataPoints->begin();iter != dataPoints->end();iter++)
//	{
//		dataMatrix.push_back(iter->potentials_->toVector());
//		if(dataMatrix.last().size() > maxRowLength)
//			maxRowLength = dataMatrix.last().size();
//		channels.push_back(QString("ch").append(QString::number(iter->channel_)));
//		columns.append(",").append(channels.last());
//	}
//	bool addedColumns;
//	QMutexLocker locker(databaseWriteMutex_.data());
//	QTime debugTimer;
//	do
//	{
//		addedColumns = false;
//		int currCol = 0;
//		double currTime = data.getTimestamp();
//		bool cmdFailed = false;
//		QString command = QString("INSERT INTO lfp (dataid,timestamp,fittedtime,correlation%1) ").arg(columns);
//		for(int currCol = 0;currCol<maxRowLength;currCol++,currTime += data.getSecPerSample())
//		{
//
//			QString values;
//			//Iterate through the channels in this timestamp, creating the column and data commands for the INSERT.
//			for(int i=0;i<channels.size();i++)
//			{
//				if(dataMatrix[i].size() <= currCol)
//					continue; // There's no more data for this channel
//				values.append(",");
//				values.append(QString::number(dataMatrix[i][currCol]));
//			}
//			command.append(QString("SELECT %2,%3,%4,%5%6")
//				
//				.arg(data.getDataID())
//				.arg(currTime)
//				.arg(alignmentTool_->convertToBehavioralTimebase(currTime))
//				.arg(alignmentTool_->getCorrelationCoefficient())
//				.arg(values));
//			if(currCol < maxRowLength-1)
//				command.append(" UNION ");
//		}
//debugTimer.start();
//		if(!executeWriteQuery(&cacheQ,command,false))
//		{
//			//If we're here then we failed to write one of the queries.  Maybe its because the data columns being used aren't in the database yet.
//
//			//These records are used to check if our column already exists
//			QSqlRecord cacheLFPRecord = cacheDb.record("lfp");
//			QSqlRecord diskLFPRecord = cacheDb.record("diskdb.lfp");
//			QSqlQuery cacheAlterQ(cacheDb);
//			foreach(QString channel, channels)
//			{
//				int cacheIndex = cacheLFPRecord.indexOf(channel);
//				if(cacheIndex > -1)
//					continue;//This column is already in the cache database.
//				int diskIndex = diskLFPRecord.indexOf(channel);
//				if(diskIndex == -1)	// The column may also be in the disk database if we reloaded a dropped session, so we need to check.
//					executeWriteQuery(&cacheAlterQ,QString("ALTER TABLE diskdb.lfp ADD COLUMN %1 INTEGER").arg(channel));
//				executeWriteQuery(&cacheAlterQ,QString("ALTER TABLE lfp ADD COLUMN %1 INTEGER").arg(channel));
//				//Now that we added the column to the tables, make sure its added in our lists.
//				tableColumns_["lfp"].append(",").append(channel).append(" ");
//				tableColumnTypes_["lfp"].append(",").append("INTEGER ");
//				addedColumns = true;
//				qDebug(QString("Added %1 column to lfp database").arg(channel).toAscii());
//			}
//			// If the command failed and we didn't add any columns, the error must have been for some other reason.  Report it.
//			Q_ASSERT_X(!cmdFailed || addedColumns,"SessionInfo::insertLFPData","Error: "+cacheQ.lastError().text().toAscii());
//		}
//
//	}while(addedColumns);	// If we added columns, then the data wasn't added.  Do it again.
////qDebug("LFPwrite to sql total  " + QString::number(debugTimer.elapsed()).toAscii());
//}

//! \brief inserts an LFP data point in the cache database
void SessionInfo::insertLFPData(Picto::LFPDataStore data)
{
	bool addedColumns;
	QSqlDatabase cacheDb = getCacheDb();
	//Get Fitted Times
	QStringList timestamps = data.getTimes().split(" ",QString::SkipEmptyParts);
	QVector<QString> potentialsVec = data.getPotentials();
	QVector<QStringList> potentials;
	for(int i=0;i<potentialsVec.size();i++)
		potentials.push_back(potentialsVec[i].split(" ",QString::SkipEmptyParts));
	QVariantList timestampList;
	QVariantList fittedTimeList;
	QVariantList dataIDList;
	QVariantList correlationList;
	QVector<QVariantList> potentialLists;
	double correlation = alignmentTool_->getCorrelationCoefficient();
	qulonglong dataID = data.getDataID();
	foreach(QString time,timestamps)
	{
		timestampList << time;
		fittedTimeList << alignmentTool_->convertToBehavioralTimebase(time.toDouble());
		correlationList << correlation;
		dataIDList << dataID;
	}
	for(int i=0;i<potentials.size();i++)
	{
		potentialLists.push_back(QVariantList());
		foreach(QString potent,potentials[i])
		{
			potentialLists[i] << potent;
		}
	}
	QString channels;
	QString chanVals;
	for(int i=0;i<potentialLists.size();i++)
	{
		channels.append(",ch"+QString::number(i).toAscii());
		chanVals.append(",?");
	}





	//Check if all the necessary rows are there
	//This records is used to check if our columns already exists
	QSqlRecord cacheLFPRecord = cacheDb.record("lfp");
	if(cacheLFPRecord.count() != 5+potentialsVec.size())
	{
		//If we're here then we need to add columns to our table (cache and potentially disk)
		QSqlRecord diskLFPRecord = cacheDb.record("diskdb.lfp");
		QSqlQuery cacheAlterQ(cacheDb);
		foreach(QString channel, channels.split(",",QString::SkipEmptyParts))
		{
			int cacheIndex = cacheLFPRecord.indexOf(channel);
			if(cacheIndex > -1)
				continue;//This column is already in the cache database.
			int diskIndex = diskLFPRecord.indexOf(channel);
			if(diskIndex == -1)	// The column may also be in the disk database if we reloaded a dropped session, so we need to check.
				executeWriteQuery(&cacheAlterQ,QString("ALTER TABLE diskdb.lfp ADD COLUMN %1 INTEGER").arg(channel));
			executeWriteQuery(&cacheAlterQ,QString("ALTER TABLE lfp ADD COLUMN %1 INTEGER").arg(channel));
			//Now that we added the column to the tables, make sure its added in our lists.
			tableColumns_["lfp"].append(",").append(channel).append(" ");
			tableColumnTypes_["lfp"].append(",").append("INTEGER ");
			addedColumns = true;
			qDebug(QString("Added %1 column to lfp database").arg(channel).toAscii());
		}
	}

	//Now that the tables are ready.  Do the insertion.
	//Start a transaction
	cacheDb.transaction();
	QSqlQuery query(cacheDb);
	QString queryString = QString("INSERT INTO lfp "
		"(dataid,correlation,timestamp,fittedtime%1) VALUES (?,?,?,?%2)").arg(channels).arg(chanVals);

	int rows = timestampList.size();
	//QMutexLocker locker(databaseWriteMutex_.data());
	QTime timer;
	timer.start();
	//executeWriteQuery(&query,"BEGIN TRANSACTION",);
	query.prepare(queryString.toAscii());
	query.addBindValue(dataIDList);
	query.addBindValue(correlationList);
	query.addBindValue(timestampList);
	query.addBindValue(fittedTimeList);
	foreach(QVariantList pots,potentialLists)
	{
		query.addBindValue(pots);
	}
	//DONT - I REPEAT - DONT PUT A STRING LIST INTO A QUERY BIND VALUE.  USE A VARIANT LIST.  IF YOU
	//USE A STRING LIST IT WILL TAKE SO MUCH LONGER THAT YOU WON'T EVEN HAVE THE PATIENCE TO TEST TO 
	//SEE HOW LONG IT DOES TAKE
	bool success = query.execBatch();
	query.finish();
	cacheDb.commit();
	Q_ASSERT_X(success,"SessionInfo::insertLFPData","Error: "+query.lastError().text().toAscii()+"\nPrepared query was: "+queryString.toAscii());
	//executeWriteQuery(&query,"END TRANSACTION");
	//locker.unlock();
	if(rows > 0)
		qDebug("Time per insert: " + QString::number(double(timer.elapsed())/(double(rows)*1000.0)).toAscii() + " seconds");
}

//! \brief inserts a state change record in the session database
void SessionInfo::insertStateData(Picto::StateDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	query.prepare("INSERT INTO statetransitions "
		"(dataid, machinepath, source, sourceresult, destination, time) "
		"VALUES(:dataid, :machinepath, :source, :sourceresult, :destination, :time) ");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":machinepath", data.getMachinePath());
	query.bindValue(":source", data.getSource()); 
	query.bindValue(":sourceresult",data.getSourceResult());
	query.bindValue(":destination",data.getDestination());
	query.bindValue(":time",data.getTime());
	executeWriteQuery(&query,"",false);	
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
		executeWriteQuery(&cacheQ,"",false);	
	}
}

void SessionInfo::insertRewardData(Picto::RewardDataStore data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	query.prepare("INSERT INTO rewards (dataid, duration, channel, time) "
		"VALUES (:dataid, :duration, :channel, :time)");
	query.bindValue(":dataid", data.getDataID());
	query.bindValue(":duration",data.getDuration());
	query.bindValue(":channel", data.getChannel());
	query.bindValue(":time",data.getTime());
	executeWriteQuery(&query,"",false);	
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
		"SELECT xpos, ypos, time FROM diskdb.behavioraldata WHERE time > :time2 ORDER BY time DESC LIMIT 1");
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
	//QMutexLocker locker(databaseWriteMutex_.data());
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
		"SELECT diskdb.framedata.frame,diskdb.framedata.time,diskdb.framedata.state FROM diskdb.framedata WHERE time > :time2 ORDER BY time DESC LIMIT 1");
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
	//QMutexLocker locker(databaseWriteMutex_.data());
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
		"diskdb.statetransitions.machinepath FROM diskdb.statetransitions WHERE time > :time2 ORDER BY time DESC LIMIT 1");
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
	//QMutexLocker locker(databaseWriteMutex_.data());
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
}

void SessionInfo::InitializeVariables()
{

	databaseWriteMutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	activity_ = true;
	ignoreComponents_ = false;
	timestampsAligned_ = (false);
	//CreateUUID
	if(uuid_ == QUuid())
	{
		uuid_ = QUuid::createUuid();
	}
	databaseVersion_ = "1.0";

#ifdef NO_AUTH_REQUIRED
	//Add a null QUuid so that everyone is considered an authorized user
	authorizedObservers_.append(QUuid());
#endif
	latestNeuralTimestamp_ = 0;
	latestBehavioralTimestamp_ = 0;

	//Define database
	tables_.push_back("sessioninfo");
	tableColumns_["sessioninfo"] = " key,value ";
	tableColumnTypes_["sessioninfo"] = " TEXT,TEXT ";

	tables_.push_back("componentinfo");
	tableColumns_["componentinfo"] = " componentid,address,name,type,active ";
	tableColumnTypes_["componentinfo"] = " TEXT,TEXT,TEXT,TEXT,INTEGER ";

	tables_.push_back("spikes");
	tableColumns_["spikes"] = " dataid,timestamp,fittedtime,correlation,channel,unit,waveform ";
	tableColumnTypes_["spikes"] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,REAL,TEXT,TEXT,TEXT ";
	tableDataProviders_["spikes"] = "PROXY";

	tables_.push_back("neuralalignevents");
	tableColumns_["neuralalignevents"] = " dataid,timestamp,aligncode,matched ";
	tableColumnTypes_["neuralalignevents"] = " INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER ";
	tableDataProviders_["neuralalignevents"] = "PROXY";

	tables_.push_back("lfp");
	tableColumns_["lfp"] = " dataid,correlation,timestamp,fittedtime ";
	tableColumnTypes_["lfp"] = " INTEGER,REAL,REAL UNIQUE ON CONFLICT IGNORE,REAL ";
	tableDataProviders_["lfp"] = "PROXY";
	
	tables_.push_back("behavioralalignevents");
	tableColumns_["behavioralalignevents"] = " dataid,timestamp,aligncode,aligneventnumber,matched ";
	tableColumnTypes_["behavioralalignevents"] = " INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER,INTEGER ";
	tableDataProviders_["behavioralalignevents"] = "DIRECTOR";

	tables_.push_back("alignevents");
	tableColumns_["alignevents"] = " aligneventnumber,aligncode,neuraltime,behavioraltime,jitter,correlation ";
	tableColumnTypes_["alignevents"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,REAL,REAL,REAL,REAL ";
	tableDataProviders_["alignevents"] = "DIRECTOR";

	tables_.push_back("behavioraldata");
	tableColumns_["behavioraldata"] = " dataid,xpos,ypos,time ";
	tableColumnTypes_["behavioraldata"] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,REAL ";
	tableDataProviders_["behavioraldata"] = "DIRECTOR";

	tables_.push_back("statetransitions");
	tableColumns_["statetransitions"] = " dataid,machinepath,source,sourceresult,destination,time ";
	tableColumnTypes_["statetransitions"] = " INTEGER UNIQUE ON CONFLICT IGNORE,TEXT,TEXT,TEXT,TEXT,REAL ";
	tableDataProviders_["statetransitions"] = "DIRECTOR";

	tables_.push_back("framedata");
	tableColumns_["framedata"] = " dataid,frame,time,state ";
	tableColumnTypes_["framedata"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,REAL,TEXT ";
	tableDataProviders_["framedata"] = "DIRECTOR";

	tables_.push_back("rewards");
	tableColumns_["rewards"] = " dataid,duration,channel,time ";
	tableColumnTypes_["rewards"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,INTEGER,REAL ";
	tableDataProviders_["rewards"] = "DIRECTOR";

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

	// If the database doesn't yet contain a session info table, we need to add the session info.
	bool insertSessionInfo = !baseSessionDbConnection_.tables().contains("sessioninfo");

	AddTablesToDatabase(&sessionQ);

	if(insertSessionInfo)
	{
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
}
void SessionInfo::CreateCacheDatabase(QString databaseName)
{
	//Create the cacheDB
	QString cacheDatabaseName = databaseName+"_cache";
	cacheDb_ = QSqlDatabase::addDatabase("QSQLITE",cacheDatabaseName);
	cacheDb_.setDatabaseName(":memory:");
	//baseCacheDbConnection_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + cacheDatabaseName + ".sqlite");
	bool success = cacheDb_.open();
	Q_ASSERT_X(success,"SessionInfo::CreateCacheDatabse","Error: Cache Database failed to open.");
	QSqlQuery cacheQ(cacheDb_);

	//If a NeuralAlignEvent or BehavioralAlignEvent is reinserted it means that the data was resent.  For this reason, in the cache table, we use ON CONFLICT IGNORE for dataid
	tableColumnTypes_["neuralalignevents"].replace("INTEGER UNIQUE ON CONFLICT REPLACE","INTEGER UNIQUE ON CONFLICT IGNORE");
	tableColumnTypes_["behavioralalignevents"].replace("INTEGER UNIQUE ON CONFLICT REPLACE","INTEGER UNIQUE ON CONFLICT IGNORE");
	AddTablesToDatabase(&cacheQ);
	//Restore original data type definitions
	tableColumnTypes_["neuralalignevents"].replace("INTEGER UNIQUE ON CONFLICT IGNORE","INTEGER UNIQUE ON CONFLICT REPLACE");
	tableColumnTypes_["behavioralalignevents"].replace("INTEGER UNIQUE ON CONFLICT IGNORE","INTEGER UNIQUE ON CONFLICT REPLACE");

	// Attach the disk backed database to the database connection so that we can easily access both from the same query.
	cacheQ.prepare("ATTACH DATABASE :databaseName AS diskdb");
	cacheQ.bindValue(":databaseName", baseSessionDbConnection_.databaseName());
	executeWriteQuery(&cacheQ);
}

void SessionInfo::AddTablesToDatabase(QSqlQuery* query)
{
	foreach(QString table, tables_)
	{
		QStringList tableColumns = tableColumns_[table].split(",");
		QStringList tableColumnTypes = tableColumnTypes_[table].split(",");
		QString columnDefString;
		int colLength = tableColumns.size();
		int typeLength = tableColumnTypes.size();
		for(int i=0;(i<colLength || i<typeLength);i++)
		{
			columnDefString.append(QString(",") + ((i<colLength)?tableColumns[i]:"") + " " + ((i<colLength)?tableColumnTypes[i]:"")); 
		}
		if(tableColumnConstraints_.contains(table))
			columnDefString.append(QString(",") + tableColumnConstraints_[table]);
		QString createTableString = "CREATE TABLE IF NOT EXISTS "+table+" (id  INTEGER PRIMARY KEY"+columnDefString+")";
		executeWriteQuery(query,createTableString);
	}
}

/*! \brief Adds optional debuggion to read query executions.  
 *	The caller should lock the databaseWriteMutex before calling this function, then finish() the query and unlock
 *	the mutex when done so as to avoid causing problems with commit() calls.  
 *	( See http://doc.qt.nokia.com/latest/qsqldatabase.html#commit )
 */
bool SessionInfo::executeReadQuery(QSqlQuery* query, QString optionalString, bool debug)
{
	bool success;
	if(optionalString != "")
		success = query->exec(optionalString);
	else
		success = query->exec();
	Q_ASSERT_X(!debug || success,"SessionInfo::executeWriteQuery","Error: "+query->lastError().text().toAscii());
	return success;
}

//! \brief Executes a Sql write query, making it threadsafe by locking a mutex during query use and including debug assertion
bool SessionInfo::executeWriteQuery(QSqlQuery* query, QString optionalString,bool lock,bool debug)
{
	if(lock)
		databaseWriteMutex_->lock();
	bool success;
	if(optionalString != "")
		success = query->exec(optionalString);
	else
		success = query->exec();
	query->finish();
	if(lock)
		databaseWriteMutex_->unlock();
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
	QMutexLocker locker(databaseWriteMutex_.data());
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
	// Go through the list.  Use the first alignable value for each aligncode, throw out any subsequent values for the same aligncode.
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
		behavTime = query.value(0).toDouble();
		neuralTime = query.value(1).toDouble();
		if((behavTime <= latestBehavioralTimestamp_)
			|| (neuralTime <= latestNeuralTimestamp_))
			// We already matched this aligncode
			continue;
		hadData = true;
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
	if( realignAll || (!timestampsAligned_ && hadData && (alignmentTool_->getCorrelationCoefficient() > 0)) )
	{
		recalculateFittedTimes();
		if(hadData && (alignmentTool_->getCorrelationCoefficient() > 0))
			timestampsAligned_ = true;
	}
	locker.unlock();

}

void SessionInfo::recalculateFittedTimes()
{
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(sessionDb);
	QSqlQuery query1(sessionDb);
	flushCache();
	QString queryString = "UPDATE spikes SET "+alignmentTool_->getSQLTimeConversionEquation("fittedtime","timestamp","correlation");
	executeWriteQuery(&query,queryString);
	queryString = "UPDATE lfp SET "+alignmentTool_->getSQLTimeConversionEquation("fittedtime","timestamp","correlation");
	executeWriteQuery(&query,queryString);
	//executeReadQuery(&query,"SELECT dataid,timestamps FROM lfp");
	//while(query.next())
	//{
	//	QStringList timestamps = query.value(1).toString().split(" ",QString::SkipEmptyParts);
	//	QString fittedTimes;
	//	foreach(QString time,timestamps)
	//	{
	//		fittedTimes.append(" ");
	//		fittedTimes.append(QString::number(alignmentTool_->convertToBehavioralTimebase(time.toDouble())));
	//	}
	//	query1.prepare("UPDATE lfp SET fittedtimes=:fittedtimes,correlation=:correlation WHERE dataid=:dataid");
	//	query1.bindValue(":fittedtimes",fittedTimes);
	//	query1.bindValue(":correlation",alignmentTool_->getCorrelationCoefficient());
	//	query1.bindValue(":dataid",query.value(0));
	//}
	////queryString = "UPDATE lfp SET "+alignmentTool_->getSQLTimeConversionEquation("fittedtime","timestamp","correlation");
	////executeWriteQuery(&query,queryString);
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
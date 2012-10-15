#include "SessionInfo.h"
#include "ConnectionManager.h"
#include "../../common/storage/experimentconfig.h"
#include "../../common/storage/SignalChannelInfo.h"

#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutexLocker>
#include <QStringList>
#include <QSqlRecord>
#include "../../common/memleakdetect.h"

#define FRAME_STATE_VAR_ID -1
#define TRANSITION_STATE_VAR_ID -2
#define REWARD_STATE_VAR_ID -3
#define MAX_SIG_CHAN_VAR_ID -4
#define NEURAL_DATA_BUFFER_SECS 60
//This turns on and off the authorized user permission setup on SessionInfo
//#define NO_AUTH_REQUIRED
QMap<QUuid,QWeakPointer<SessionInfo>> SessionInfo::loadedSessions_;
QSharedPointer<SessionInfo> SessionInfo::CreateSession(QString experimentName, QString directorName, QByteArray experimentXml, QByteArray experimentConfig, QUuid initialObserverId, QString password)
{
	//Creates a shared pointer for the object that will use the deleteSession function to actually delete
	//the object
	QSharedPointer<SessionInfo> returnVal(new SessionInfo(experimentName,directorName,experimentXml,experimentConfig,initialObserverId, password),&deleteSession);
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
	Q_ASSERT(session);
	if(loadedSessions_.contains(session->sessionId()))
		loadedSessions_.remove(session->sessionId());
	QString sessionId = session->sessionId().toString();
	QStringList connectionNames = session->openDatabaseConnections_;
	QString baseSessionDbConName = session->getSessionDb().connectionName();
	delete session;
	//The database connections must be removed from the QSqlDatabase internal driver after the actual
	//QSqlDatabase instances have been closed
	foreach(QString connectionName, connectionNames)
	{
		QSqlDatabase::removeDatabase(connectionName);
	}
	qDebug("Session: " + sessionId.toAscii() + " has been unloaded!");
}

SessionInfo::SessionInfo(QString experimentName, QString directorName, QByteArray experimentXml, QByteArray experimentConfig, QUuid initialObserverId, QString password):
	experimentXml_(experimentXml),
	experimentConfig_(experimentConfig),
	password_(password)
{
	InitializeVariables();
	
	//Add the initial observer (the session creator) to our list of authorized observers
	//This means that he can issue ENDSESSION and TASK commands.
	authorizedObservers_.append(initialObserverId);

	//Generate the base session DB name
	QDateTime dateTime = QDateTime::currentDateTime();
	QString databaseName = "Session_"+dateTime.toString("yyyy_MM_dd__hh_mm_ss");	
	//If there's already a file with this name, append _x until we have a unique name
	QString sessionPath = QCoreApplication::applicationDirPath()+"/../sessions/"+directorName+"/"+experimentName;
	QDir dir(sessionPath);
	if(!dir.exists())
	{
		dir.mkpath(sessionPath);
		dir = QDir(sessionPath);
	}
	while(dir.entryList().contains(databaseName))
	{
		databaseName.append("_x");
	}
	databaseName.append(".sqlite");

	//Setup databases
	LoadBaseSessionDatabase(sessionPath, databaseName);
	SetupBaseSessionDatabase();
	CreateCacheDatabase(databaseName);

	//file_.setFileName(databaseName + ".test");
	//file_.open(QIODevice::WriteOnly);
}

SessionInfo::SessionInfo(QString databaseFilePath)
{
	InitializeVariables();

	//Setup databases
	QStringList strList = databaseFilePath.split("/");
	QString databaseName = strList.takeLast();
	QString sessionPath = strList.join("/");
	LoadBaseSessionDatabase(sessionPath,databaseName);
	Q_ASSERT(baseSessionDbConnection_.tables().contains("sessioninfo"));

	SetupBaseSessionDatabase();
	//CreateCacheDatabase(databaseName);

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

	// Load SessionPassword
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"Password\"");
	if(sessionQ.next())
		password_ = sessionQ.value(0).toString();
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

	// Load signal channel variable ids.
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"Signal\"");
	int currId;
	while(sessionQ.next())
	{
		Picto::SignalChannelInfo sigInfo;
		sigInfo.fromXml(sessionQ.value(0).toString());
		currId = sigInfo.getStateVariableId();
		sigChanVarIDs_[sigInfo.getName()] = currId;
		if(currId < nextSigChanVarId_)
			nextSigChanVarId_ = currId - 1;

		//Add the cache table for this signal channel to the list since it was dynamically created during
		//the pre-disconnect run and will not have been added to the lists during InitializeVariables()
		QString tableName = QString("signal_%1").arg(sigInfo.getName());
		tables_.push_back(tableName);
		tableColumns_[tableName] = " dataid,offsettime,sampleperiod,data,frameid ";
		tableColumnTypes_[tableName] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,BLOB,INTEGER ";
		tableDataProviders_[tableName] = "DIRECTOR";
	}
	sessionQ.finish();

	// Load data source info.
	executeReadQuery(&sessionQ,"SELECT value FROM sessioninfo WHERE key=\"DataSource\"");
	while(sessionQ.next())
	{
		QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo());
		srcInfo->fromXml(sessionQ.value(0).toString());
		dataSourceInfoMap_[srcInfo->getName()] = srcInfo;
	}
	sessionQ.finish();

	// Load Session Data Pack
	executeReadQuery(&sessionQ,"SELECT runid,name,notes,firstframe,lastframe,saved FROM runs");
	while(sessionQ.next())
	{
		QSharedPointer<Picto::TaskRunDataUnit> taskRun(
			new Picto::TaskRunDataUnit(
				sessionQ.value(3).toString().toLongLong(),
				sessionQ.value(4).toString().toLongLong(),
				sessionQ.value(1).toString(),	
				sessionQ.value(2).toString(),
				sessionQ.value(5).toString().toInt()
			));
		taskRun->setDataID(sessionQ.value(0).toLongLong());
		currSessionDataPack_->setTaskRun(taskRun);
	}
	sessionQ.finish();

	CreateCacheDatabase(databaseName);

	// Load Current State
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQuery(cacheDb);
	executeReadQuery(&cacheQuery,"INSERT INTO currentstate(dataid, variableid, time, data) SELECT dataid, variableid, time, data FROM diskdb.currentstate",true);
	cacheQuery.finish();
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
	foreach(QString connectionName, openDatabaseConnections_)
	{
		QSqlDatabase connection = QSqlDatabase::database(connectionName);
		if(connection.isValid())
			connection.close();
	}
	//file_.close();
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
	flushEnabled_[component->getType()] = false;
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
 * If overwriteRedundantDirective is set (default) and the last directive type is the same as the new one, it will be overwritten.
 */
void SessionInfo::addPendingDirective(QString directive, QString componentType)
{
	QSharedPointer<ComponentInfo> component = getComponentByType(componentType);
	if(component.isNull())
		return;
	QStringList* directives = &pendingDirectives_[component->getUuid()];
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
	QSqlDatabase sessionDb = getSessionDb();

	//Add the end time to the session db
	QSqlQuery query(sessionDb);
	query.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"sessionend\", :time)");
	query.bindValue(":time", QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	executeWriteQuery(&query);
	
	
	//Realign timebases now that we have all the data
	alignTimeBases(true);

	//Create indeces in session database for quicker analysis
	createSessionIndeces();

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
 *	Adds the input observerId to the list of authorized ids if
 *  the correct password is input.
 *	Returns true if the password was correct and the observer is authorized now
 */
bool SessionInfo::addAuthorizedObserver(QUuid observerId, QString password)
{
	if(password == password_)
	{
		addAuthorizedObserver(observerId);
		return true;
	}
	return  false;
}

QString SessionInfo::getPassword()
{
	return password_;
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
		{
			qDebug("Failed to initiate transaction when flushing cache. Error was: " + cacheDb.lastError().text().toAscii() + "...Reattempting.");
			continue;
		}

		// Loop through all tables.  If this sourceType is responsible for that table, flush its contents to the diskdb and erase cache.
		// In the case of the neural/behavioralalignevents, don't erase unvisited, unmatched alignevents
		foreach(QString table,tables_)
		{
			if(tableDataProviders_.contains(table) && ((sourceType == "") || (tableDataProviders_.value(table)==sourceType)) )
			{
				QString queryString = "INSERT INTO diskdb.%1(%2) SELECT %2 FROM %1";
				executeWriteQuery(&cacheQuery,QString("INSERT INTO diskdb.%1(%2) SELECT %2 FROM %1").arg(table).arg(tableColumns_[table]));
				//The code below was used for a test where we write the SQL commands to a file then create the database afterwards
				//for the purposes of speed.  In this test, speed was increase by a factor of ~8 (ie. 50ms to flush cache as opposed
				//to 400ms). It does not yet work because binary data needs to be written to sql too which doesn't work cleanly in
				//a text file.  
				//executeReadQuery(&cacheQuery,QString("SELECT %2 FROM %1").arg(table).arg(tableColumns_[table]));
				//while(cacheQuery.next())
				//{
				//	QStringList  vals;
				//	for(int i=0;i<tableColumns_[table].split(",",QString::SkipEmptyParts).size();i++)
				//	{
				//		vals.append(cacheQuery.value(i).toString());
				//	}
				//	//Doesn't quite work because of Binary Data.  Think about this.
				//	writeToFile(QString("INSERT INTO %1(%2) VALUES ('%3');").arg(table).arg(tableColumns_[table]).arg(vals.join("','")));;
				//}

				if(table != "currentstate")
					executeWriteQuery(&cacheQuery,QString("DELETE FROM %1%2").arg(table).arg(table.contains("ralalignevents")?" WHERE matched<>0":""));
			}
		}

		// Store the latest "current table" entries
		cacheQuery.prepare(QString("INSERT INTO diskdb.currentstate (dataid, variableid, time, data) SELECT dataid, variableid, time, data FROM currentstate WHERE time > :last"));
		cacheQuery.bindValue(":last",latestWrittenStateVarTime_);
		executeWriteQuery(&cacheQuery,"",true);

		//Our latest written state var time has now caught up with the latest one in memory
		latestWrittenStateVarTime_ = latestStateVarTime_;
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
void SessionInfo::insertNeuralData(QSharedPointer<Picto::NeuralDataUnit> data)
{
	if(!dataSourceInfoMap_.contains("spikes"))
	{
		QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo("spikes","spikes",data->getResolution()));
		//Add the lfp data source info to the session database and our dataSourceInfoMap_ map
		QSqlQuery sessionQ(getSessionDb());
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"DataSource\", :value)");
		sessionQ.bindValue(":value", srcInfo->toXml());
		if(executeWriteQuery(&sessionQ))
			dataSourceInfoMap_["spikes"] = srcInfo;
		sessionQ.finish();
	}

	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	//! Use the alignment tool to fit the time
	data->setFittedtime(alignmentTool_->convertToBehavioralTimebase(data->getTimestamp()));
	data->setCorrelation(alignmentTool_->getCorrelationCoefficient());
	//locker.unlock();

	query.prepare("INSERT INTO spikes (dataid, timestamp, channel, unit, waveform) "
		"VALUES(:dataid, :timestamp, :channel, :unit, :waveform)");
	query.bindValue(":dataid", data->getDataID());
	query.bindValue(":timestamp", data->getTimestamp());
	query.bindValue(":channel", data->getChannel());
	query.bindValue(":unit", data->getUnit());
	query.bindValue(":waveform", data->getWaveformAsByteArray());
	executeWriteQuery(&query,"",false);

	setLatestNeuralData(data->getDataID(),data->toXml(),data->getFittedtime());
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertBehavioralData(QSharedPointer<Picto::BehavioralDataUnitPackage> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQ(cacheDb);
	//BECAUSE OF THIS LINE, NON "Position" CHANNELS ARE
	//IGNORED.  WHEN REMOVING THIS LINE, YOU ALSO HAVE TO WRITE
	//CODE TO RECORD EACH CHANNEL IN A SEPERATE TABLE AND ONLY
	//WRITE THE "Position" AS A STATE VARIABLE!!!!!!!!!!
	//qDebug("Received data from chan: " + data->getChannel().toAscii());
	//if(data->getChannel() != "Position")
	//	return;

	//Get this signal channels State Variable ID.
	//If it doesn't have one yet, add it to the map and store
	//it in the session database.
	QString sigChan = data->getChannel().toLower();
	QString tableName = QString("signal_%1").arg(sigChan);
	int stateVarId;
	if(sigChanVarIDs_.contains(sigChan))
		stateVarId = sigChanVarIDs_.value(sigChan);
	else
	{
		stateVarId = nextSigChanVarId_--;

		//Create the table for this signal channel.
		QSqlQuery sessionQ(getSessionDb());
		tables_.push_back(tableName);
		tableColumns_[tableName] = " dataid,offsettime,sampleperiod,data,frameid ";
		tableColumnTypes_[tableName] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,BLOB,INTEGER ";
		tableDataProviders_[tableName] = "DIRECTOR";

		AddTablesToDatabase(&sessionQ);
		AddTablesToDatabase(&cacheQ);

		//Add the signal channel info to the session database
		Picto::SignalChannelInfo sigInfo(	sigChan,
											tableName,
											stateVarId,
											data->getResolution(),
											data->getDescriptor().split(",",QString::SkipEmptyParts).size(),
											data->getDescriptor());

		//Add the signal channel info to the session database.
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"Signal\", :value)");
		sessionQ.bindValue(":value", sigInfo.toXml());
		if(executeWriteQuery(&sessionQ))
			sigChanVarIDs_[sigChan] = stateVarId;
		sessionQ.finish();
	}

	if(data->length())
	{
		cacheQ.prepare(QString("INSERT INTO %1 (dataid, offsettime, sampleperiod, data, frameid)"
			"VALUES(:dataid, :offsettime, :sampleperiod, :data, :frameid)").arg(tableName));
		cacheQ.bindValue(":dataid", data->getDataID());
		cacheQ.bindValue(":offsettime",data->getOffsetTime());
		cacheQ.bindValue(":sampleperiod",data->getResolution());
		cacheQ.bindValue(":data",data->getDataAsByteArray());
		cacheQ.bindValue(":frameid",data->getActionFrame());
		executeWriteQuery(&cacheQ,"",false);

		//Only write a behavioralDataUnitPackage with the last data unit
		//
		//Use the package text because that contains the signal channel name, time, sampleperiod.
		//Use the unit id because that was generated when the last data unit
		//was generated and will preserve the information generation order
		//in the stateVariable list.
		data->clearAllButLastDataPoints();
		if(data->length() == 1)
		{
			setStateVariable(data->getDataID(),stateVarId,data->toXml());
		}
	}
}

//! \brief inserts a behavioral data point in the cache database
void SessionInfo::insertPropertyData(QSharedPointer<Picto::PropertyDataUnitPackage> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQ(cacheDb);

	QSharedPointer<Picto::PropertyDataUnit> dataPoint;
	while(data->length() > 0)
	{
		dataPoint = data->takeFirstDataPoint();
		setStateVariable(dataPoint->getDataID(),dataPoint->index_,dataPoint->toXml());
		cacheQ.prepare("INSERT INTO properties (dataid, assetid, value, frameid)"
			"VALUES(:dataid, :assetid, :value, :frameid)");
		cacheQ.bindValue(":dataid", dataPoint->getDataID());
		cacheQ.bindValue(":assetid", dataPoint->index_);
		cacheQ.bindValue(":value",dataPoint->value_);
		cacheQ.bindValue(":frameid",dataPoint->getActionFrame());
		executeWriteQuery(&cacheQ,"",false);	
	}
}

//! \brief inserts an alignment data point in the database and attempts latest alignment
void SessionInfo::insertAlignmentData(QSharedPointer<Picto::AlignmentDataUnit> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);
	// If the alignevent comes with an alignNumber, it must be from the component providing the time baseline, 
	// put it in behavioralignevents and don't align its timestamps.
	if(data->hasAlignNumber())
	{
		query.prepare("INSERT INTO behavioralalignevents (dataid,timestamp,aligncode,aligneventnumber,matched)"
			"VALUES(:dataid, :timestamp, :aligncode, :aligneventnumber, 0)");
		query.bindValue(":dataid", data->getDataID());
		query.bindValue(":timestamp", data->getTimestamp());
		query.bindValue(":aligncode", data->getAlignCode());
		query.bindValue(":aligneventnumber", data->getAlignNumber());
		executeWriteQuery(&query,"",false);
		return;
	}
	//If we got here, its not coming from the component providing the time baseline.
	//Add the event code to the neuralalignevents table
	query.prepare("INSERT OR IGNORE INTO neuralalignevents (dataid, timestamp, aligncode, matched)"
		"VALUES(:dataid, :timestamp, :aligncode, 0)");
	query.bindValue(":dataid", data->getDataID());
	query.bindValue(":timestamp", data->getTimestamp());
	query.bindValue(":aligncode", data->getAlignCode());
	executeWriteQuery(&query,"",false);	

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
		alignTimeBases();
}

//! \brief inserts an LFP data point in the cache database
void SessionInfo::insertLFPData(QSharedPointer<Picto::LFPDataUnitPackage> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	//Get Fitted Times
	//QStringList timestamps = data->getTimes().split(" ",QString::SkipEmptyParts);
	double neuralTimestamp = data->getTimestamp().toDouble();
	//QString potentials = data->getPotentials();

	if(!dataSourceInfoMap_.contains("lfp"))
	{
		QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo("lfp","lfp",data->getResolution()));
		//Add the lfp data source info to the session database and our dataSourceInfoMap_ map
		QSqlQuery sessionQ(getSessionDb());
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"DataSource\", :value)");
		sessionQ.bindValue(":value", srcInfo->toXml());
		if(executeWriteQuery(&sessionQ))
			dataSourceInfoMap_["lfp"] = srcInfo;
		sessionQ.finish();
	}

	double correlation = alignmentTool_->getCorrelationCoefficient();
	double resolution = data->getResolution();
	qulonglong dataID = data->getDataID();
	double fittedTimestamp = alignmentTool_->convertToBehavioralTimebase(neuralTimestamp);
	data->setFittedTimestamp(fittedTimestamp);
	double fittedResolution = alignmentTool_->convertSamplePeriodToBehavioralTimebase(resolution);
	data->setFittedResolution(fittedResolution);
	int channel = data->getChannel();

	//Now that the tables are ready.  Do the insertion.
	QSqlQuery query(cacheDb);
	query.prepare("INSERT INTO lfp "
		"(dataid,timestamp,channel,data) VALUES (:dataid,:timestamp,:channel,:data)");

	query.bindValue(":dataid",dataID);
	query.bindValue(":timestamp",neuralTimestamp);
	query.bindValue(":channel",channel);
	query.bindValue(":data",data->getPotentialsAsByteArray());

	executeWriteQuery(&query);
	query.finish();
	
	setLatestNeuralData(data->getDataID(),data->toXml(),fittedTimestamp);
}

//! \brief inserts a state change record in the session database
void SessionInfo::insertStateData(QSharedPointer<Picto::StateDataUnitPackage> data)
{




	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	QSharedPointer<Picto::StateDataUnit> dataPoint;
	while(data->length() > 0)
	{
		dataPoint = data->takeFirstDataPoint();
		setStateVariable(dataPoint->getDataID(),TRANSITION_STATE_VAR_ID,dataPoint->toXml());
		query.prepare("INSERT INTO transitions "
		"(dataid, transid, frameid) "
		"VALUES(:dataid, :transid, :frameid) ");
		query.bindValue(":dataid", dataPoint->getDataID());
		query.bindValue(":transid", dataPoint->getTransitionID());
		query.bindValue(":frameid",dataPoint->getActionFrame());
		executeWriteQuery(&query,"",false);	
	}






	//setStateVariable(data->getDataID(),TRANSITION_STATE_VAR_ID,data->getTime(),data->toXml());
	//QSqlDatabase cacheDb = getCacheDb();
	//QSqlQuery query(cacheDb);

	//query.prepare("INSERT INTO transitions "
	//	"(dataid, transid, machinepath, source, sourceresult, destination, time) "
	//	"VALUES(:dataid, :transid, :machinepath, :source, :sourceresult, :destination, :time) ");
	//query.bindValue(":dataid", data->getDataID());
	//query.bindValue(":transid", data->getTransitionID());
	//query.bindValue(":machinepath", data->getMachinePath());
	//query.bindValue(":source", data->getSource()); 
	//query.bindValue(":sourceresult",data->getSourceResult());
	//query.bindValue(":destination",data->getDestination());
	//query.bindValue(":time",data->getTime());
	//executeWriteQuery(&query,"",false);	
}

void SessionInfo::insertTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data)
{
	QMutexLocker locker(&sessionDataMutex_);
	QSqlDatabase sessionDb = getSessionDb();

	QSqlQuery query(sessionDb);
	if(data->endFrame_ == 0)
	{	//This is the beginning of the run
		query.prepare("INSERT INTO runs(runid,name,notes,firstframe,lastframe,saved)"
			" VALUES (:runid,:name,:notes,:firstframe,:lastframe,:saved)");
		query.bindValue(":runid", data->getDataID());
		query.bindValue(":name",data->name_);
		query.bindValue(":notes",data->notes_);
		query.bindValue(":firstframe", data->startFrame_);
		query.bindValue(":lastframe", data->endFrame_);
		query.bindValue(":saved", false);
		currSessionDataPack_->setTaskRun(data);
	}
	else
	{	//This is the end of the run, just update the endFrame column
		query.prepare("UPDATE runs SET lastframe=:lastframe WHERE runid=:runid");
		query.bindValue(":lastframe", data->endFrame_);
		query.bindValue(":runid", data->getDataID());
		QSharedPointer<Picto::TaskRunDataUnit> run = currSessionDataPack_->getTaskRunByRunId(data->getDataID());
		if(run)
			run->endFrame_ = data->endFrame_;
	}
	executeWriteQuery(&query);
}

void SessionInfo::modifyTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data)
{
	QMutexLocker locker(&sessionDataMutex_);
	QSharedPointer<Picto::TaskRunDataUnit> run = currSessionDataPack_->getTaskRunByRunId(data->getDataID());
	if(run)
	{
		run->name_ = data->name_;
		run->notes_ = data->notes_;
		run->saved_ = data->saved_;
	}

	QSqlDatabase sessionDb = getSessionDb();
	//Add the component to the session db
	QSqlQuery query(sessionDb);

	query.prepare("UPDATE runs SET name=:name,notes=:notes,saved=:saved WHERE runid=:runid");
	query.bindValue(":name",data->name_);
	query.bindValue(":notes",data->notes_);
	query.bindValue(":saved", data->saved_);
	query.bindValue(":runid", data->getDataID());

	executeWriteQuery(&query);
}

//! \brief Inserts the passed in frame data into the cache database
void SessionInfo::insertFrameData(QSharedPointer<Picto::FrameDataUnitPackage> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQ(cacheDb);
	QSharedPointer<Picto::FrameDataUnit> framedata;
	bool hadData = false;
	while(data->length() > 0)
	{
		hadData = true;
		framedata = data->takeFirstDataPoint();
		cacheQ.prepare("INSERT INTO frames (dataid, time)"
			"VALUES(:dataid, :time)");
		cacheQ.bindValue(":dataid", framedata->getDataID());
		cacheQ.bindValue(":time",framedata->time);
		executeWriteQuery(&cacheQ,"",false);	
	}
	//Currently we require that frame data be sent to the server as soon as all 
	//data that was valid for that frame has been sent and before any other data arrives.
	//This means that as soon as we insert frame data, its time to flushStateBufferToCurrentState
	//so that any workstations reading in current state data will have a picture of the latest valid
	//state
	if(hadData)
	{	
		QMutexLocker locker(databaseWriteMutex_.data());
		setStateVariable(framedata->getDataID(),FRAME_STATE_VAR_ID,framedata->toXml());
		updateCurrentStateTable(framedata->time);
	}
}

void SessionInfo::insertRewardData(QSharedPointer<Picto::RewardDataUnit> data)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);

	query.prepare("INSERT INTO rewards (dataid, duration, channel, time) "
		"VALUES (:dataid, :duration, :channel, :time)");
	query.bindValue(":dataid", data->getDataID());
	query.bindValue(":duration",data->getDuration());
	query.bindValue(":channel", data->getChannel());
	query.bindValue(":time",data->getTime());
	executeWriteQuery(&query,"",false);
	setStateVariable(data->getDataID(),REWARD_STATE_VAR_ID,data->toXml());
	
	QSharedPointer<ComponentInfo> sourceComponent = getComponentByType("DIRECTOR");
	//When the experiment is not running, Update the current state
	//table right away.
	if(sourceComponent && (sourceComponent->getStatus() != ComponentStatus::running))
	{
		updateCurrentStateTable(data->getTime());
	}
}

//We use a single table to track the latest state of all variables pertinant to running
//a picto workstation.  Using this table, we can assure requests for the latest picto
//state will have constant runtime independant of the previous runtime of the current
//experiment.  New variable values always update existing values with the same id if 
//their timestamp is greater than that of their predecessor.  Calling selectStateVariables
//will return a string that can be sent to a workstation.  This string contains all of the
//stateVariables that have changed after (not including) the input time.  The format of 
//the returned variables is just the serialization that the director used when sending
//them to the server.
QString SessionInfo::selectStateVariables(QString fromTime)
{
	QString result;
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery query(cacheDb);
	query.prepare("SELECT data FROM currentstate WHERE time > :time");
	query.bindValue(":time",fromTime);
	if(!databaseWriteMutex_->tryLock())
		return result;
	executeReadQuery(&query,"",true);
	while(query.next())
	{
		result.append(query.value(0).toString());
	}
	query.finish();
	databaseWriteMutex_->unlock();
	return result;
}

QString SessionInfo::selectLatestNeuralData(QString fromDataId)
{
	QString result;
	int afterDataId = fromDataId.toInt();
	if(!latestNeuralDataMutex_.tryLock())
		return result;
	QLinkedList<NeuralVariable>::iterator iter;
	//Find the afterDataId entry and append everything after that.
	//Don't just look for when DataID's are higher, because in the case
	//of lfp blocks dataIDs might be out of order.
	//If the afterDataID isn't found, just append everything.
	bool startAppending = false;
	for(iter = latestNeuralData_.end()-1;iter != latestNeuralData_.begin();iter--)
	{
		if((*iter).dataid == afterDataId)
		{
			iter++;
			break;
		}
	}
	for(;iter != latestNeuralData_.end();iter++)
	{
		result.append((*iter).serial);
	}
	latestNeuralDataMutex_.unlock();
	return result;
}

QString SessionInfo::selectSessionDataPackage()
{
	QString result;
	if(!sessionDataMutex_.tryLock())
		return result;
	result = currSessionDataPack_->toXml();
	sessionDataMutex_.unlock();
	return result;
}

void SessionInfo::InitializeVariables()
{

	databaseWriteMutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	activity_ = true;
	ignoreComponents_ = false;
	latestStateVarTime_ = "0.0";
	latestWrittenStateVarTime_ = "0.0";
	//CreateUUID
	if(uuid_ == QUuid())
	{
		uuid_ = QUuid::createUuid();
	}
	databaseVersion_ = "1.0";
	nextSigChanVarId_ = MAX_SIG_CHAN_VAR_ID;

#ifdef NO_AUTH_REQUIRED
	//Add a null QUuid so that everyone is considered an authorized user
	authorizedObservers_.append(QUuid());
#endif
	latestNeuralTimestamp_ = 0;
	latestBehavioralTimestamp_ = 0;
	currSessionDataPack_ = QSharedPointer<Picto::SessionDataPackage>(new Picto::SessionDataPackage());

	//Define database
	tables_.push_back("sessioninfo");
	tableColumns_["sessioninfo"] = " key,value ";
	tableColumnTypes_["sessioninfo"] = " TEXT,TEXT ";

	tables_.push_back("componentinfo");
	tableColumns_["componentinfo"] = " componentid,address,name,type,active ";
	tableColumnTypes_["componentinfo"] = " TEXT,TEXT,TEXT,TEXT,INTEGER ";

	tables_.push_back("runs");
	tableColumns_["runs"] = " runid,name,notes,firstframe,lastframe,saved ";
	tableColumnTypes_["runs"] = " INTEGER UNIQUE ON CONFLICT REPLACE,TEXT,TEXT,INTEGER,INTEGER,INTEGER ";

	tables_.push_back("spikes");
	tableColumns_["spikes"] = " dataid,timestamp,channel,unit,waveform ";
	tableColumnTypes_["spikes"] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,INTEGER,INTEGER,BLOB ";
	tableDataProviders_["spikes"] = "PROXY";

	tables_.push_back("neuralalignevents");
	tableColumns_["neuralalignevents"] = " dataid,timestamp,aligncode,matched ";
	tableColumnTypes_["neuralalignevents"] = " INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER ";
	tableDataProviders_["neuralalignevents"] = "PROXY";

	tables_.push_back("lfp");
	tableColumns_["lfp"] = " dataid,timestamp,channel,data ";
	tableColumnTypes_["lfp"] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,INTEGER,BLOB ";
	tableDataProviders_["lfp"] = "PROXY";
	
	tables_.push_back("behavioralalignevents");
	tableColumns_["behavioralalignevents"] = " dataid,timestamp,aligncode,aligneventnumber,matched ";
	tableColumnTypes_["behavioralalignevents"] = " INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER,INTEGER ";
	tableDataProviders_["behavioralalignevents"] = "DIRECTOR";

	tables_.push_back("alignevents");
	tableColumns_["alignevents"] = " aligneventnumber,aligncode,neuraltime,behavioraltime,jitter,correlation ";
	tableColumnTypes_["alignevents"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,REAL,REAL,REAL,REAL ";
	tableDataProviders_["alignevents"] = "DIRECTOR";

	tables_.push_back("transitions");
	tableColumns_["transitions"] = " dataid,transid,frameid ";
	tableColumnTypes_["transitions"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,INTEGER";
	tableDataProviders_["transitions"] = "DIRECTOR";
	tableIndexedColumns_["transitions"] = " transid,frameid ";

	tables_.push_back("transitionlookup");
	tableColumns_["transitionlookup"] = " assetid,parent,source,sourceresult,destination ";
	tableColumnTypes_["transitionlookup"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,TEXT,TEXT,TEXT ";
	tableIndexedColumns_["transitionlookup"] = " parent,source,sourceresult,destination ";

	tables_.push_back("properties");
	tableColumns_["properties"] = " dataid,assetid,value,frameid ";
	tableColumnTypes_["properties"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,TEXT,INTEGER ";
	tableDataProviders_["properties"] = "DIRECTOR";
	tableIndexedColumns_["properties"] = " assetid,frameid ";

	tables_.push_back("propertylookup");
	tableColumns_["propertylookup"] = " assetid,name,parent ";
	tableColumnTypes_["propertylookup"] = " INTEGER UNIQUE ON CONFLICT IGNORE,TEXT,INTEGER ";
	tableIndexedColumns_["propertylookup"] = " name,parent ";

	tables_.push_back("elementlookup");
	tableColumns_["elementlookup"] = " assetid,path ";
	tableColumnTypes_["elementlookup"] = " INTEGER UNIQUE ON CONFLICT IGNORE,TEXT ";
	tableIndexedColumns_["elementlookup"] = " path ";

	tables_.push_back("frames");
	tableColumns_["frames"] = " dataid,time ";
	tableColumnTypes_["frames"] = " INTEGER UNIQUE ON CONFLICT IGNORE,REAL ";
	tableDataProviders_["frames"] = "DIRECTOR";

	tables_.push_back("rewards");
	tableColumns_["rewards"] = " dataid,duration,channel,time ";
	tableColumnTypes_["rewards"] = " INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,INTEGER,REAL ";
	tableDataProviders_["rewards"] = "DIRECTOR";

	//This tables is used internally by the session and accessed by workstations to get
	//the latest experimental state.
	//A sql INSERT query is constantly built up with the latest state info.  Whenever a frame input is
	//received, the query gets performed creating contains a valid state that
	//was in place at the time that frame was presented.
	tables_.push_back("currentstate");
	tableColumns_["currentstate"] = " dataid,variableid,time,data ";
	tableColumnTypes_["currentstate"] = " INTEGER,INTEGER UNIQUE ON CONFLICT REPLACE,REAL,TEXT ";

	//create alignment tool
	alignToType_ = "";
	alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool());
}
void SessionInfo::LoadBaseSessionDatabase(QString path, QString databaseName)
{
	baseSessionDbConnection_ = QSqlDatabase::addDatabase("QSQLITE",databaseName);
	openDatabaseConnections_.append(databaseName);
baseSessionDbFilepath_ = path + "/" + databaseName;
	baseSessionDbConnection_.setDatabaseName(baseSessionDbFilepath_);
	baseSessionDbConnection_.open();
}
void SessionInfo::SetupBaseSessionDatabase()
{
	Q_ASSERT(baseSessionDbConnection_.isOpen());
	QSqlQuery sessionQ(baseSessionDbConnection_);

	//Set the synchronous setting: 
	//FULL is fully protected from application crash and system failure - This is the Sqlite default
	//NORMAL is fully protected from application crash and most system failure case won't corrupt
	//			the database.  In practice, a catastrophic disk or hardware fault is more likely to
	//			occur.
	//OFF is fully protected from application crash.  System failures could corrupt data.
	executeWriteQuery(&sessionQ,"PRAGMA synchronous = OFF");
	//The DELETE (default) journaling mode is the normal behavior. In the DELETE mode, the rollback 
	//journal is deleted at the conclusion of each transaction. Indeed, the delete operation is the 
	//action that causes the transaction to commit. (See the documented titled Atomic Commit In SQLite 
	//for additional detail.)
	//The WAL journaling mode uses a write-ahead log instead of a rollback journal to implement 
	//transactions. The WAL journaling mode is persistent; after being set it stays in effect across 
	//multiple database connections and after closing and reopening the database. A database in WAL 
	//journaling mode can only be accessed by SQLite version 3.7.0 or later. This mode can be significantly
	//faster than DELETE in cases where there are significantly more writes than reads.  This mode uses
	//three files, which makes it distastful for using sqlite as a file type.
	executeWriteQuery(&sessionQ,"PRAGMA journal_mode = DELETE");
	//Set the temp_store setting: 
	//The temp_store values specifies the type of database back-end to use for temporary files. 
	//The choices are DEFAULT (0), FILE (1), and MEMORY (2). The use of a memory database for 
	//temporary tables can produce signifigant savings. DEFAULT specifies the compiled-in default, 
	//which is FILE unless the source has been modified.
	executeWriteQuery(&sessionQ,"PRAGMA temp_store = 2");

	// If the database doesn't yet contain a session info table, we need to add the session info.
	bool insertSessionInfo = !baseSessionDbConnection_.tables().contains("sessioninfo");
	bool insertTransLookup = !baseSessionDbConnection_.tables().contains("transitionlookup");
	bool insertPropLookup = !baseSessionDbConnection_.tables().contains("propertylookup");
	bool insertElemLookup = !baseSessionDbConnection_.tables().contains("elementlookup");
	QSharedPointer<Picto::ExperimentConfig> expConfig;

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
		//Add the password
		sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"Password\", :password)");
		sessionQ.bindValue(":password", password_);
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

	//Create Experiment Lookup tables
	if(insertTransLookup || insertPropLookup || insertElemLookup)
	{	//Only build the experimentConfig object if you need it.
		expConfig = QSharedPointer<Picto::ExperimentConfig>(new Picto::ExperimentConfig());
		expConfig->fromXml(experimentConfig_);
	}
	if(expConfig)
	{
		// If the database doesn't yet contain a transition lookup table, we need to add it.
		QList<Picto::TransInfo> transInfo;
		QList<Picto::PropInfo> propInfo;
		QList<Picto::AssetInfo> elemInfo;
		if(insertTransLookup)
			transInfo = expConfig->getTransitionInfo();
		// If the database doesn't yet contain a property lookup table, we need to add it.
		if(insertPropLookup)
			propInfo = expConfig->getPropertyInfo();
		// If the database doesn't yet contain an element lookup table, we need to add it.
		if(insertElemLookup)
			elemInfo = expConfig->getElementInfo();

		bool success;
		do
		{
			//Start transaction
			success = baseSessionDbConnection_.transaction();
			if(!success)
			{
				qDebug("Failed to initiate transaction when writing lookup tables. Error was: " + sessionQ.lastError().text().toAscii() + "...Reattempting.");
				continue;
			}
			foreach(Picto::TransInfo infounit, transInfo)
			{
				sessionQ.prepare("INSERT INTO transitionlookup(assetid,parent,source,sourceresult,destination) VALUES (:assetid,:parent,:source,:sourceresult,:destination) ");
				sessionQ.bindValue(":assetid",infounit.id);
				sessionQ.bindValue(":parent",infounit.parent);
				sessionQ.bindValue(":source",infounit.source);
				sessionQ.bindValue(":sourceresult",infounit.result);
				sessionQ.bindValue(":destination",infounit.dest);
				executeWriteQuery(&sessionQ,"",true);
			}
			foreach(Picto::PropInfo infounit, propInfo)
			{
				sessionQ.prepare("INSERT INTO propertylookup(assetid,name,parent) VALUES (:assetid,:name,:parent) ");
				sessionQ.bindValue(":assetid",infounit.id);
				sessionQ.bindValue(":name",infounit.name);
				sessionQ.bindValue(":parent",infounit.parent);
				executeWriteQuery(&sessionQ,"",true);
			}
			foreach(Picto::AssetInfo infounit, elemInfo)
			{
				sessionQ.prepare("INSERT INTO elementlookup(assetid,path) VALUES (:assetid,:path) ");
				sessionQ.bindValue(":assetid",infounit.id);
				sessionQ.bindValue(":path",infounit.path);
				executeWriteQuery(&sessionQ,"",true);
			}
			sessionQ.clear();
			//End Transaction
			success = baseSessionDbConnection_.commit();
			//If Transaction failed, rollback and try again.
			if(!success)
			{
				qDebug("writing lookup tables failed for session: " + sessionId().toString().toAscii() + " rolling back transaction and reattempting.");
				baseSessionDbConnection_.rollback();
			}
		}while(!success);
	}

}
void SessionInfo::CreateCacheDatabase(QString databaseName)
{
	//Create the cacheDB
	QString cacheDatabaseName = databaseName+"_cache";
	cacheDb_ = QSqlDatabase::addDatabase("QSQLITE",cacheDatabaseName);
	openDatabaseConnections_.append(cacheDatabaseName);
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
		QString createTableString = "CREATE TABLE IF NOT EXISTS "+table+" (id  INTEGER PRIMARY KEY"+columnDefString+");";
		
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

//bool SessionInfo::writeToFile(QString line)
//{
//	databaseWriteMutex_->lock();
//	file_.write((line+"\n").toAscii());
//	databaseWriteMutex_->unlock();
//	return true;
//}

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
	//Note: We compare the behavioral align event to only the bottom 7 digits of the neural align event.  This way
	//systems can bind the top hardware bit to high if need be (ie. For -onesided mode) and it won't affect alignment
	executeReadQuery(&query,"SELECT behavioralalignevents.timestamp, neuralalignevents.timestamp, behavioralalignevents.id, neuralalignevents.id, behavioralalignevents.aligncode, behavioralalignevents.aligneventnumber "
		"FROM behavioralalignevents JOIN neuralalignevents ON behavioralalignevents.aligncode=(neuralalignevents.aligncode & 127) WHERE behavioralalignevents.matched=0 AND neuralalignevents.matched=0 "
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
	
	//Update stored alignment coefficients
	double nOffset = alignmentTool_->getNeuralOffsetTime();
	double nFactor = alignmentTool_->getNeuralTemporalFactor();
	QSharedPointer<Picto::AlignmentInfo> inf;
	if(alignmentInfoMap_.contains("Neural"))
	{
		inf = alignmentInfoMap_["Neural"];
		inf->setOffsetTime(nOffset);
		inf->setTemporalFactor(nFactor);
	}
	else
	{
		alignmentInfoMap_["Neural"] = inf = QSharedPointer<Picto::AlignmentInfo>(new Picto::AlignmentInfo(nOffset,nFactor));
	}

	//Replace the previous alignment info with updated data.
	QSqlQuery sessionQ(getSessionDb());
	sessionQ.prepare("DELETE FROM sessioninfo WHERE key=\"AlignmentInfo\"");
	executeWriteQuery(&sessionQ);
	sessionQ.finish();
	sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"AlignmentInfo\", :value)");
	sessionQ.bindValue(":value", inf->toXml());
	executeWriteQuery(&sessionQ);
	sessionQ.finish();

	locker.unlock();

}

void SessionInfo::createSessionIndeces()
{
	Q_ASSERT(baseSessionDbConnection_.isOpen());
	QSqlQuery query(baseSessionDbConnection_);

	QStringList createIndexQueries;
	foreach(QString table, tables_)
	{
		QStringList tableIndeces;
		if(tableIndexedColumns_.contains(table))
			tableIndeces = tableIndexedColumns_[table].split(",",QString::SkipEmptyParts);

		foreach(QString index,tableIndeces)
		{
			executeWriteQuery(&query,"CREATE INDEX IF NOT EXISTS "+table+"_"+index.trimmed()+"_index ON "+table+"("+index+");");
		}
	}

}

//We use a single table to track the latest state of all variables pertinant to running
//a picto workstation.  Using this table, we can assure requests for the latest picto
//state will have constant runtime independant of the previous runtime of the current
//experiment.  New variable values always update existing values with the same id if 
//their timestamp is greater than that of their predecessor.  
void SessionInfo::setStateVariable(int dataid, int varid, QString serializedValue)
{
	Variable var;
	var.dataid = dataid;
	var.varid = varid;
	var.serial = serializedValue;
	currentStateQuery_.append(var);
}

void SessionInfo::setLatestNeuralData(int dataid, QString serializedValue, double fittedTime)
{
	NeuralVariable newVar;
	newVar.dataid = dataid;
	newVar.serial = serializedValue;
	newVar.fittedTime = fittedTime;
	latestNeuralDataMutex_.lock();
	latestNeuralData_.append(newVar);
	latestNeuralDataMutex_.unlock();

	bool removed = false;
	while(latestNeuralData_.first().fittedTime < latestNeuralData_.last().fittedTime-NEURAL_DATA_BUFFER_SECS)
	{
		if(!removed)
			latestNeuralDataMutex_.lock();
		removed = true;
		latestNeuralData_.removeFirst();
	}
	if(removed)
		latestNeuralDataMutex_.unlock();
}

void SessionInfo::updateCurrentStateTable(QString updateTime)
{
	QSqlDatabase cacheDb = getCacheDb();
	QSqlQuery cacheQuery(cacheDb);
	//Since we are encapsulating all of these operations in a single transaction, we put the whole thing in a mutex.
	QMutexLocker locker(databaseWriteMutex_.data());
	bool success = false;
	do
	{
		//Start transaction
		success = cacheDb.transaction();
		if(!success)
		{
			qDebug("Failed to initiate transaction when writing current state. Error was: " + cacheDb.lastError().text().toAscii() + "...Reattempting.");
			continue;
		}

		// Loop through all tables.  If this sourceType is responsible for that table, flush its contents to the diskdb and erase cache.
		// In the case of the neural/behavioralalignevents, don't erase unvisited, unmatched alignevents
		foreach(Variable queryVars,currentStateQuery_)
		{
			//Make sure that the row is there.
			cacheQuery.prepare("INSERT OR IGNORE INTO currentstate (dataid,variableid,time,data) VALUES (-1,:variableid, -1, '')");
			cacheQuery.bindValue(":variableid",queryVars.varid);
			executeWriteQuery(&cacheQuery,"",true);
			//Update the value of the row if the time increased.
			cacheQuery.prepare("UPDATE currentstate SET time = :time, data = :data, dataid = :dataid WHERE variableid = :variableid AND time <= :time1 AND dataid < :dataid1");
			cacheQuery.bindValue(":time",updateTime);
			cacheQuery.bindValue(":data",queryVars.serial);
			cacheQuery.bindValue(":dataid",queryVars.dataid);
			cacheQuery.bindValue(":variableid",queryVars.varid);
			cacheQuery.bindValue(":time1",updateTime);
			cacheQuery.bindValue(":dataid1",queryVars.dataid);
			executeWriteQuery(&cacheQuery,"",true);
		}
		//Update latest written state variable.
		if(updateTime.toDouble() > latestStateVarTime_.toDouble())
			latestStateVarTime_ = updateTime;

		cacheQuery.clear();
		//End Transaction
		success = cacheDb.commit();
		//If Transaction failed, rollback and try again.
		if(!success)
		{
			qDebug("writing current state failed for session: " + sessionId().toString().toAscii() + " rolling back transaction and reattempting.");
			cacheDb.rollback();
		}
	}while(!success);
	currentStateQuery_.clear();
	locker.unlock();
	return;
}

void SessionInfo::addAuthorizedObserver(QUuid observerId)
{
	if(isAuthorizedObserver(observerId))
		return;
	Q_ASSERT(baseSessionDbConnection_.isOpen());
	QSqlQuery sessionQ(baseSessionDbConnection_);

	sessionQ.prepare("INSERT INTO sessioninfo(key, value) VALUES (\"AuthObserverID\", :id)");
	sessionQ.bindValue(":id", observerId.toString());
	executeWriteQuery(&sessionQ);
	authorizedObservers_.append(observerId);
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
		openDatabaseConnections_.append(connectionName);
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
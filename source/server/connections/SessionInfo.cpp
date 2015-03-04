#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutexLocker>
#include <QStringList>
#include <QSqlRecord>

#include "SessionInfo.h"
#include "ConnectionManager.h"
#include "../../common/storage/DesignConfig.h"
#include "../../common/storage/SignalChannelInfo.h"
#include "SerialSessionData.h"
#include "../../common/memleakdetect.h"

#define FRAME_STATE_VAR_ID -1
#define TRANSITION_STATE_VAR_ID -2
#define REWARD_STATE_VAR_ID -3
#define MAX_SIG_CHAN_VAR_ID -12
#define PROP_INIT_START_VAR_ID (INT_MAX/2)
#define NEURAL_DATA_BUFFER_SECS 60
#define BEHAV_MATCHED_COLUMN 4
#define NEURAL_MATCHED_COLUMN 3
#define BEHAV_TIMESTAMP_COLUMN 1
#define NEURAL_TIMESTAMP_COLUMN 1
#define BEHAV_EVENT_CODE_COLUMN 2
#define NEURAL_EVENT_CODE_COLUMN 2
#define DISK_WRITE_FRAC_COEFF 10.0
//This turns on and off the authorized user permission setup on SessionInfo
//#define NO_AUTH_REQUIRED
QMap<QUuid,QWeakPointer<SessionInfo>> SessionInfo::loadedSessions_;

/*! \brief Creates a new SessionInfo object.
 *	@param designName The name of the design that will be used in the Session.
  *	@param directorName The name of the Director that will be used in the Session.
 *	@param designXml The contents of the Design file that will be used in the Session.
 *	@param DesignConfig The serialized DesignConfig object that manages the Design used in this session.  This contains 
 *		information for identifying Assets from the AssetIds and is useful for building Element/Transition/etc lookup 
 *		tables in the session file.
 *	@param initialObserverId The Uuid of the workstation that is starting this Session.  It will be considered an Authorized
 *		observer (ie. It is authorized to make state changes to the Session).
 *	@param password	The password that gives workstations authorization to change the Session state.
*/
QSharedPointer<SessionInfo> SessionInfo::CreateSession(QString designName, QString directorName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password)
{
	//Creates a shared pointer for the object that will use the deleteSession function to actually delete
	//the object
	QSharedPointer<SessionInfo> returnVal(new SessionInfo(designName,directorName,designXml,DesignConfig,initialObserverId, password),&deleteSession);
	loadedSessions_[returnVal->sessionId()] = QWeakPointer<SessionInfo>(returnVal);
	return returnVal;
}

/*! \brief Loads the Session with the input sessionId and the Sqlite Session file at the input databaseFilePath and returns it.
 *	\details Internally, if the SessionInfo object for this session is already loaded, the function just returns it.  If it isn't
 *	loaded, a new SessionInfo object is created and all of the data from the Sqlite Session file is loaded into that object.
 */
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
		//Note: deleteSession() will be called with the SessionInfo pointer when it is deleted by its shared pointer.
		returnVal = QSharedPointer<SessionInfo>(new SessionInfo(databaseFilePath),&deleteSession);
	return returnVal;
}

/*! \brief Handles session deletion while maintaining a list of loaded sessions
 *	\details If a session is removed from the openSessions list by the ConnectionManager but
 *	still hasn't been deleted since something with a shared pointer is stuck in a long
 *	operation, we want to make sure that nothing else reopens the session from file
 *	before the loaded session actually gets deleted.  To do this, we maintain a 
 *	loadedSessions_ list that contains QWeakPointers to all currently loaded sessions.  
 *	When LoadSession is called, this list is checked first and if a session is found 
 *	with the input sessionID, the weak reference is converted to a regular shared pointer
 *	and returned.  Viola!  No duplicate copies of sessions.
 *
 *	This function takes care of actually removing the SessionInfo weak pointer from the
 *	list when the last SessionInfo strong smart pointer is cleared.
 *	\sa LoadSession()
 */
void SessionInfo::deleteSession(SessionInfo* session)
{
	Q_ASSERT(session);
	if(loadedSessions_.contains(session->sessionId()))
		loadedSessions_.remove(session->sessionId());
	QString sessionId = session->sessionId().toString();
	//QStringList connectionNames = session->openDatabaseConnections_;
	//QString baseSessionDbConName = session->getSessionDb().connectionName();
	delete session;
	//The database connections must be removed from the QSqlDatabase internal driver after the actual
	//QSqlDatabase instances have been closed
	//foreach(QString connectionName, connectionNames)
	//{
	//	QSqlDatabase::removeDatabase(connectionName);
	//}
	qDebug("Session: " + sessionId.toLatin1() + " has been unloaded!");
}

/*! \brief Constructs a new SessionInfo object.
 *	@param designName The name of the design that will be used in the Session.
 *	@param directorName The name of the Director that will be used in the Session.
 *	@param designXml The contents of the Design file that will be used in the Session.
 *	@param DesignConfig The serialized DesignConfig object that manages the Design used in this session.  This contains 
 *		information for identifying Assets from the AssetIds and is useful for building Element/Transition/etc lookup 
 *		tables in the session file.
 *	@param initialObserverId The Uuid of the workstation that is starting this Session.  It will be considered an Authorized
 *		observer (ie. It is authorized to make state changes to the Session).
 *	@param password	The password that gives workstations authorization to change the Session state.
 *	\details Apart from the actual object construction, this function handles variable initialization by using InitializeVariables().
 *	It also calls LoadBaseSessionDatabase() to create neccessary SessionData objects and SetupBaseSessionDatabase() to setup those
 *	objects.
 */
SessionInfo::SessionInfo(QString designName, QString directorName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password):
	designXml_(designXml),
	DesignConfig_(DesignConfig),
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
	QString sessionPath = QCoreApplication::applicationDirPath()+"/../sessions/"+directorName+"/"+designName;
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

	//file_.setFileName(databaseName + ".test");
	//file_.open(QIODevice::WriteOnly);
}

/*! \brief Constructs a new SessionInfo object based on the contents of an old timed out Sqlite Session file at the input databaseFilePath.
 *	\details Apart from the actual object construction, this function handles variable initialization by using InitializeVariables().
 *	It also calls LoadBaseSessionDatabase() to create neccessary SessionData objects and SetupBaseSessionDatabase() to setup those
 *	objects.  Once all of the SessionData objects are setup, this constructor extracts all data from the Sqlite session file that needs to 
 *	be in RAM  (using a StoredSessionData object) and enters it into the relevant variables and SessionData objects.
 */
SessionInfo::SessionInfo(QString databaseFilePath)
{
	InitializeVariables();

	//Setup databases
	QStringList strList = databaseFilePath.split("/");
	QString databaseName = strList.takeLast();
	QString sessionPath = strList.join("/");
	LoadBaseSessionDatabase(sessionPath,databaseName);
	//Q_ASSERT(baseSessionDbConnection_.tables().contains("sessioninfo"));

	SetupBaseSessionDatabase();

	//Load Session Info from previously saved session
	QList<QVariantList> sessionInfoList = storedSessionData_->peekData(SESSION_INFO_TYPE);
	foreach(QVariantList varList,sessionInfoList)
	{
		int dataId = varList[0].toInt();
		if(dataId > sessionInfoDataId_)
			sessionInfoDataId_ = dataId;
		QString key = varList[1].toString();
		QVariant val = varList[2];
		if(key == "DatabaseVersion")
		{
			databaseVersion_ = val.toString();
		}
		else if(key == "SessionID")
		{
			uuid_ = QUuid(val.toString());
			storedSessionData_->setSessionId(uuid_);
		}
		else if(key == "Password")
		{
			password_ = val.toString();
		}
		else if(key == "DesignXML")
		{
			designXml_ = val.toByteArray();
		}
		else if(key == "sessionstart")
		{
			timeCreated_ = val.toString();
		}
		else if(key == "AuthObserverID")
		{
			authorizedObservers_.append(QUuid(val.toString()));
		}
		else if(key == "AlignToComponent")
		{
			alignToType_ = val.toString();
		}
		else if(key == "Signal")
		{
			Picto::SignalChannelInfo sigInfo;
			sigInfo.fromXml(val.toString());
			int currId = sigInfo.getStateVariableId();
			sigChanVarIDs_[sigInfo.getName()] = currId;
			if(currId < nextSigChanVarId_)
				nextSigChanVarId_ = currId - 1;

			//Add a table for this signal channel to storedSessionData since it was dynamically created during
			//the pre-disconnect run and will not have been added to the lists during InitializeVariables()
			QString tableName = QString("signal_%1").arg(sigInfo.getName());
			sigChanTypes_[tableName] = FIRST_SIGNAL_CHANNEL_TYPE + sigChanTypes_.size();
			storedSessionData_->setTableInfo(sigChanTypes_[tableName],tableName," dataid,offsettime,sampleperiod,data,frameid "," INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,BLOB,INTEGER ");
		}
		else if(key == "DataSource")
		{
			QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo());
			srcInfo->fromXml(val.toString());
			dataSourceInfoMap_[srcInfo->getName()] = srcInfo;
		}
		else if(key == "AlignmentInfo")
		{
			QSharedPointer<Picto::AlignmentInfo> inf(new Picto::AlignmentInfo());
			inf->fromXml(val.toString());
			alignmentInfoMap_["Neural"] = inf;
		}
	}

	// Load Components from previously saved session
	QList<QVariantList> componentInfoList = storedSessionData_->peekData(COMPONENT_INFO_TYPE);
	QSharedPointer<ComponentInfo> component;
	foreach(QVariantList varList,componentInfoList)
	{
		component = QSharedPointer<ComponentInfo>(new ComponentInfo());
		component->setUuid(QUuid(varList[0].toString()));
		component->setAddress(varList[1].toString());
		component->setName(varList[2].toString());
		component->setType(varList[3].toString());
		component->setStatus(ComponentStatus::notFound);
		components_[component->getType()] = component;
		componentActivity_[component->getUuid()] = varList[4].toBool();
	}

	// Load Session Data Pack
	QList<QVariantList> runsList = storedSessionData_->peekData(RUNS_TYPE);
	foreach(QVariantList varList,runsList)
	{
		QSharedPointer<Picto::TaskRunDataUnit> taskRun(
			new Picto::TaskRunDataUnit(
				varList[3].toString().toLongLong(),
				varList[4].toString().toLongLong(),
				varList[1].toString(),	
				varList[2].toString(),
				varList[5].toString().toInt()
			));
		taskRun->setDataID(varList[0].toLongLong());
		currSessionDataPack_->setTaskRun(taskRun);
	}

	// Load Current State
	QList<QVariantList> currStateList = storedSessionData_->peekData(CURRENT_STATE_TYPE);
	foreach(QVariantList varList,currStateList)
	{
		currentSessionData_->updateVariable(varList[0].toInt(),varList[1].toInt(),varList[2].toString(),varList[3].toString());
	}
}

SessionInfo::~SessionInfo()
{
}

/*! \brief Adds a Component represented by the input ComponentInfo object to the current session (ie. Director or Proxy)
*/
void SessionInfo::AddComponent(QSharedPointer<ComponentInfo> component)
{
	storedSessionData_->insertData(
										COMPONENT_INFO_TYPE,QVariantList() 
											<< component->getUuid().toString() 
											<< component->getAddress()
											<< component->getName()
											<< component->getType()
											<< false
										);

	components_[component->getType()] = component;
	flushEnabled_[component->getType()] = false;
}

/*! \brief Called periodically to update the stored information about the Session's Components in the Session database.
 *	\details If the component is still attached to the Server, it is set as either Active or Inactive in this Session 
 *	depending on whether it reports that it is attached to the current Session or to some other Session.
 */
void SessionInfo::UpdateComponentActivity()
{
	//Add the component to the stored data
	foreach(QSharedPointer<ComponentInfo> component,components_)
	{
		//If the component isn't found, it may be temporarily disconnected but still running, so don't change anything.
		if(component->getStatus() != ComponentStatus::notFound)
		{
			bool active = component->getSessionID() == sessionId();
			if(componentActivity_[component->getUuid()] != active)
			{
				storedSessionData_->insertData(
										COMPONENT_INFO_TYPE,QVariantList() 
											<< component->getUuid().toString() 
											<< component->getAddress()
											<< component->getName()
											<< component->getType()
											<< active
										);
				componentActivity_[component->getUuid()] = active;
			}
		}
	}
}

/*! \brief Returns true if any of the Components associated with this Session are currently active (ie. Attached to the Server and
 *	reporting that they are working on this Session.
 *	\note If ignoreComponents() has been called, this will always return false.
 *	\sa UpdateComponentActivity()
 */
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

/*! \brief Returns the ComponentInfo object representing the Component of the input type (ie. "DIRECTOR", "PROXY") that is associated with this Session.
*/
QSharedPointer<ComponentInfo> SessionInfo::getComponentByType(QString type)
{
	if(components_.contains(type))
		return components_[type];
	return QSharedPointer<ComponentInfo>();
}

/*! \brief Returns true if the Component with the input componentId is one of the one's handling this Session.
*/
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
 *	\details Currently, the input should always be "DIRECTOR".
 */
void SessionInfo::alignTimestampsTo(QString componentType)
{
	Q_ASSERT(!getComponentByType(componentType).isNull());

	storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList() << ++sessionInfoDataId_ << "AlignToComponent" << componentType);
	alignToType_ = componentType;
}


/*! Returns the Directive that should be sent to the Component with the input componentId.
 *	\details Internally, there is a queue of pending directives.  This function removes the first one from the queue and
 *	returns it.
*/
QString SessionInfo::pendingDirective(QUuid componentID)
{
	if(!pendingDirectives_.contains(componentID) || pendingDirectives_[componentID].isEmpty())
		return QString();
	else
		return pendingDirectives_[componentID].takeFirst();
}

/*!	\brief Adds the input directive to the pendingDirectives list for the component with the input type.
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
 *	 -  Realign timebases now that the entire Session's worth of data is available.
 *	 -  Create the various indeces in the Sqlite Session file that will speed up queries now that there is no danger of them
 *		slowing down data entry.
 *	 -	Flush all final cached data to the Sqlite file (flushCache()).
 */
bool SessionInfo::endSession()
{
	
	//Add the end time to the session db
	storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList() << ++sessionInfoDataId_ <<"sessionend" << QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm"));
	
	//Realign timebases now that we have all the data
	alignTimeBases(true);

	//Create indeces in session database for quicker analysis
	createSessionIndeces();

	//Flush the database cache
	flushCache();
	qDebug("Session: " + sessionId().toString().toLatin1() + " Ended!");
	return true;
}


/*!	\brief Determines if the observer with the passed in ID is authorized to issue commands and change the State of the Session.
 *
 *	This is a really simple means of setting permissions on the session.  Effectively there
 *	are two levels of user: authorized, and unauthorized.  This allows us to limit who is 
 *	allowed to do things like issuing TASK and ENDSESSION commands and changing Experimental runtime Properties.
 *
 *	The authorized observers list contains all observers who are authorized at the higher 
 *	permission level.  However, there is a special case.  If the list contains a null Uuid,
 *	then all observers (including those using null IDs) are considered to be at the authorized
 *	level.
 *
 *	\warning This is by no means a secure method of assigning permissions.  Since all network communication
 *	is unencrypted, it would be trivial to intercept a valid observer ID, and then append it to
 *	spoofed commands.  The password is also not encrypted on the Server or Workstation.
 *	If you want real security, you'll need to rewrite this.
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

/*! \brief Adds the input observerId to the list of authorized ids if
 *  the correct password is input.
 *	\details Returns true if the password was correct and the observer is authorized now
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

/*! \brief Returns the password that will give a Workstation access to change things about the Session and be more than a simple viewer.
*/
QString SessionInfo::getPassword()
{
	return password_;
}

/*! \brief Marks the time that the latest data was written to this Session by the input source type (ie. "DIRECTOR" or "PROXY").
*/
void SessionInfo::markLastDataTime(QString source)
{
	dataTimeBySource_[source] = totalSessionTime_.elapsed();
}
/*! \brief Returns whether the last data written from the input source preceded the beginning of the latest flush of RAM data to Disk.
*/
bool SessionInfo::lastDataPrecededFlush(QString source)
{
	if(!dataTimeBySource_.contains(source))
		return false;
	QMutexLocker locker(&flushTimingMutex_);
	if(dataTimeBySource_[source] < lastFlushStartTime_)
	{
		return true;
	}
	return false;
}
/*! \brief Returns whether the last data written from the input source followed the end of the latest flush of RAM data to Disk.
*/
bool SessionInfo::lastDataFollowedFlush(QString source)
{
	if(!dataTimeBySource_.contains(source))
		return false;
	QMutexLocker locker(&flushTimingMutex_);
	if(dataTimeBySource_[source] > lastFlushEndTime_)
	{
		return true;
	}
	return false;
}

/*! \brief Dumps the RAM cached data into the file based Session database
 *
 *	@param sourceType The type of data source whose data will be written to the disk backed data base.  Options are currently "DIRECTOR", "PROXY"
 *		or "".  In the case of the empty "" string, data from both the Director and the Proxy will be written to disk.
 *
 *	\details We buffer several seconds of data at a time in RAM for the purpose of avoiding the large SQL overhead every time a SQL write query
 *	is performed.  We then save it to disk to that we won't lose data if Picto crashes.  This function 
 *	moves the contents of the RAM data to the file based session database.  
 *
 *	This works closely with the ProtocolResponse system.  That system tells Picto Components which of their commmands' data has been saved
 *	to disk (See ProtocolResponse::RegisteredResponseType).  When a long time goes by without a Component hearing that its data has been written
 *	to disk, it sends it again.  This way, we are sure that a Server can crash and restart without any data being lost in the Session.
 *	
 *	\note The table of current Property, etc Values is also written to disk so that we can correctly pick up where we left off if the Server
 *	crashes and restarts.
 *
 *	\note The time at which the flush starts and ends are also carefully marked for use with lastDataPrecededFlush() and lastDataFollowedFlush()
 *	which are necessary to be sure we tell the Picto components exactly which data has been written to disk in this multi-threaded environment.
 *
 *	Returns true on success or false if nothing happened.
 */
bool SessionInfo::flushCache(QString sourceType)
{
	//if(!flushingCache_.tryLock())
	//	return false;
	flushTimingMutex_.lock();
	lastFlushStartTime_ = totalSessionTime_.elapsed();
	flushTimingMutex_.unlock();
	QTime timer;
	timer.start();

	//QThread::sleep(1);
	//qDebug("Fake flush done: " + QString::number(timer.elapsed()).toLatin1() + "ms");
	if(sourceType == "" || sourceType == "DIRECTOR")
	{
		//timer.start();
		cachedDirectorSessionData_->moveDataTo(storedSessionData_.data());
		//qDebug("Time to flush cachedDirectorSessionData_: " + QString::number(timer.elapsed()).toLatin1() + "ms");
		//timer.start();
		behavAlignSessionData_->moveDataTo(storedSessionData_.data());
		//qDebug("Time to flush behavAlignSessionData_: " + QString::number(timer.elapsed()).toLatin1() + "ms");
	}
	if(sourceType == "" || sourceType == "PROXY")
	{
		//timer.start();
		cachedProxySessionData_->moveDataTo(storedSessionData_.data());
		//qDebug("Time to flush cachedProxySessionData_: " + QString::number(timer.elapsed()).toLatin1() + "ms");
		//timer.start();
		neuralAlignSessionData_->moveDataTo(storedSessionData_.data());
		//qDebug("Time to flush neuralAlignSessionData_: " + QString::number(timer.elapsed()).toLatin1() + "ms");
	}
	
	//timer.start();
	stateVarTimeMutex_.lock();
	QString latestStateVarTime = latestStateVarTime_;
	QString latestWrittenStateVarTime = latestWrittenStateVarTime_;
	stateVarTimeMutex_.unlock();
	currentSessionData_->copyDataTo(storedSessionData_.data(),latestWrittenStateVarTime);
	//qDebug("Time to flush currentSessionData_: " + QString::number(timer.elapsed()).toLatin1() + "ms");

	stateVarTimeMutex_.lock();
	//Our latest written state var time has now caught up with the latest one in memory
	latestWrittenStateVarTime_ = latestStateVarTime;
	stateVarTimeMutex_.unlock();

	double totalTime = totalSessionTime_.elapsed();
	timeInFlush_ += timer.elapsed();
	if(totalTime > 0)
	{
		qDebug("Time spent writing data to disk in session:" + QString::number(100.0 * timeInFlush_/totalTime).toLatin1() + "%");
	}
	//flushingCache_.unlock();
	flushTimingMutex_.lock();
	lastFlushEndTime_ = totalSessionTime_.elapsed();
	flushTimingMutex_.unlock();
	return true;
}

/*! \brief Inserts a NeuralDataUnit record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertNeuralData(QSharedPointer<Picto::NeuralDataUnit> data)
{
	if(!dataSourceInfoMap_.contains("spikes"))
	{
		QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo("spikes","spikes",data->getResolution()));
		//Add the lfp data source info to the session database and our dataSourceInfoMap_ map
		storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList() << ++sessionInfoDataId_ << "DataSource" << srcInfo->toXml());
		dataSourceInfoMap_["spikes"] = srcInfo;
	}

	//! Use the alignment tool to fit the time
	data->setFittedtime(alignmentTool_->convertToBehavioralTimebase(data->getTimestamp()));
	data->setCorrelation(alignmentTool_->getCorrelationCoefficient());

	cachedProxySessionData_->insertData(SPIKES_TYPE,data->getDataID(),QVariantList() 
																		<< data->getDataID()
																		<< data->getTimestamp()
																		<< data->getChannel()
																		<< data->getUnit()
																		<< data->getWaveformAsByteArray());
	setLatestNeuralData(data->getDataID(),data->toXml(),data->getFittedtime());
}

/*! \brief Inserts a BehavioralDataUnitPackage record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertBehavioralData(QSharedPointer<Picto::BehavioralDataUnitPackage> data)
{
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

		sigChanTypes_[tableName] = FIRST_SIGNAL_CHANNEL_TYPE + sigChanTypes_.size();
		storedSessionData_->setTableInfo(sigChanTypes_[tableName],tableName," dataid,offsettime,sampleperiod,data,frameid "," INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,BLOB,INTEGER ");

		//Add the signal channel info to the session database
		Picto::SignalChannelInfo sigInfo(	sigChan,
											tableName,
											stateVarId,
											data->getResolution(),
											data->getDescriptor().split(",",QString::SkipEmptyParts).size(),
											data->getDescriptor());

		//Add the signal channel info to the session database.
		storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList()<< ++sessionInfoDataId_ << "Signal" << sigInfo.toXml());
		sigChanVarIDs_[sigChan] = stateVarId;
	}

	if(data->length())
	{
		cachedDirectorSessionData_->insertData(sigChanTypes_[tableName],data->getDataIDOfLastUnit(),QVariantList()
														<< data->getDataIDOfLastUnit()
														<< data->getOffsetTime()
														<< data->getResolution()
														<< data->getDataAsByteArray()
														<< data->getActionFrame());

		//Only write a behavioralDataUnitPackage with the last data unit
		//
		//Use the package text because that contains the signal channel name, time, sampleperiod.
		//Use the unit id because that was generated when the last data unit
		//was generated and will preserve the information generation order
		//in the stateVariable list.
		data->clearAllButLastDataPoints();
		if(data->length() == 1)
		{
			setStateVariable(data->getDataIDOfLastUnit(),stateVarId,data->toXml());
		}
	}
}

/*! \brief Inserts a InputDataUnitPackage record into the CachedSessionData and the record of the current Session state.
*	The CachedSessionData will later be flushed to disk in flushCache().
*/
void SessionInfo::insertInputData(QSharedPointer<Picto::InputDataUnitPackage> data)
{
	//Get this signal channels State Variable ID.
	//If it doesn't have one yet, add it to the map and store
	//it in the session database.
	QString sigChan = data->getChannel().toLower();
	QString tableName = QString("signal_%1").arg(sigChan);
	int stateVarId;
	if (sigChanVarIDs_.contains(sigChan))
		stateVarId = sigChanVarIDs_.value(sigChan);
	else
	{
		stateVarId = nextSigChanVarId_--;

		sigChanTypes_[tableName] = FIRST_SIGNAL_CHANNEL_TYPE + sigChanTypes_.size();
		storedSessionData_->setTableInfo(sigChanTypes_[tableName], tableName, " dataid,offsettime,sampleperiod,data,frameid ", " INTEGER UNIQUE ON CONFLICT IGNORE,REAL,REAL,BLOB,INTEGER ");

		//Add the signal channel info to the session database
		Picto::SignalChannelInfo sigInfo(sigChan,
			tableName,
			stateVarId,
			data->getResolution(),
			data->getDescriptor().split(",", QString::SkipEmptyParts).size(),
			data->getDescriptor());

		//Add the signal channel info to the session database.
		storedSessionData_->insertData(SESSION_INFO_TYPE, QVariantList() << ++sessionInfoDataId_ << "Signal" << sigInfo.toXml());
		sigChanVarIDs_[sigChan] = stateVarId;
	}

	if (data->length())
	{
		cachedDirectorSessionData_->insertData(sigChanTypes_[tableName], data->getDataIDOfLastUnit(), QVariantList()
			<< data->getDataIDOfLastUnit()
			<< data->getOffsetTime()
			<< data->getResolution()
			<< data->getDataAsByteArray()
			<< data->getActionFrame());

		//Only write a InputDataUnitPackage with the last data unit
		//
		//Use the package text because that contains the signal channel name, time, sampleperiod.
		//Use the unit id because that was generated when the last data unit
		//was generated and will preserve the information generation order
		//in the stateVariable list.
		data->clearAllButLastDataPoints();
		if (data->length() == 1)
		{
			setStateVariable(data->getDataIDOfLastUnit(), stateVarId, data->toXml());
		}
	}
}

/*! \brief Inserts a PropertyDataUnitPackage record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertPropertyData(QSharedPointer<Picto::PropertyDataUnitPackage> data)
{
	QSharedPointer<Picto::PropertyDataUnit> dataPoint;
	while(data->length() > 0)
	{
		dataPoint = data->takeFirstDataPoint();
		if(dataPoint->initValue_)
		{
			setStateVariable(dataPoint->getDataID(),PROP_INIT_START_VAR_ID+dataPoint->index_,dataPoint->toXml());
			cachedDirectorSessionData_->insertData(INIT_PROPERTIES_TYPE,dataPoint->getDataID(),QVariantList()
																	<< dataPoint->getDataID()
																	<< dataPoint->index_
																	<< dataPoint->value_
																	<< dataPoint->getActionFrame());
		}
		else
		{
			setStateVariable(dataPoint->getDataID(),dataPoint->index_,dataPoint->toXml());
			cachedDirectorSessionData_->insertData(PROPERTIES_TYPE,dataPoint->getDataID(),QVariantList()
																	<< dataPoint->getDataID()
																	<< dataPoint->index_
																	<< dataPoint->value_
																	<< dataPoint->getActionFrame());
		}

	}
}

/*! \brief Adds an AlignmentDataUnit data point to the appropriate SessionData object attempts alignment with the
 *	latest data using alignTimeBases().
*/
void SessionInfo::insertAlignmentData(QSharedPointer<Picto::AlignmentDataUnit> data)
{
	// If the alignevent comes with an alignNumber, it must be from the component providing the time baseline, 
	// put it in behavioralignevents and don't align its timestamps.
	if(data->hasAlignNumber())
	{
		behavAlignSessionData_->insertData(data->getDataID(),QVariantList() 
															<< data->getDataID()
															<< data->getTimestamp()
															<< data->getAlignCode()
															<< data->getAlignNumber()
															<< 0);
		return;
	}
	//If we got here, its not coming from the component providing the time baseline.
	//Add the event code to the neuralalignevents table
	neuralAlignSessionData_->insertData(data->getDataID(),QVariantList() 
															<< data->getDataID()
															<< data->getTimestamp()
															<< data->getAlignCode()
															<< 0);

	// Check if this session requires alignment.  If so, do it.
	if(alignToType_ != "")
		alignTimeBases();
}

/*! \brief Inserts a LFPDataUnitPackage record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertLFPData(QSharedPointer<Picto::LFPDataUnitPackage> data)
{
	//Get Fitted Times
	//QStringList timestamps = data->getTimes().split(" ",QString::SkipEmptyParts);
	double neuralTimestamp = data->getTimestamp().toDouble();
	//QString potentials = data->getPotentials();

	if(!dataSourceInfoMap_.contains("lfp"))
	{
		QSharedPointer<Picto::DataSourceInfo> srcInfo(new Picto::DataSourceInfo("lfp","lfp",data->getResolution()));
		//Add the lfp data source info to the session database and our dataSourceInfoMap_ map
		storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList() << ++sessionInfoDataId_ << "DataSource" << srcInfo->toXml());
		dataSourceInfoMap_["lfp"] = srcInfo;
	}

	//double correlation = alignmentTool_->getCorrelationCoefficient();
	double resolution = data->getResolution();
	qulonglong dataID = data->getDataID();
	double fittedTimestamp = alignmentTool_->convertToBehavioralTimebase(neuralTimestamp);
	data->setFittedTimestamp(fittedTimestamp);
	double fittedResolution = alignmentTool_->convertSamplePeriodToBehavioralTimebase(resolution);
	data->setFittedResolution(fittedResolution);
	int channel = data->getChannel();

	//Now that the tables are ready.  Do the insertion.
	cachedProxySessionData_->insertData(LFP_TYPE,dataID,QVariantList() << dataID << neuralTimestamp << channel << data->getPotentialsAsByteArray());
	
	setLatestNeuralData(data->getDataID(),data->toXml(),fittedTimestamp);
}

/*! \brief Inserts a StateDataUnitPackage (Transition traversal) record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertStateData(QSharedPointer<Picto::StateDataUnitPackage> data)
{

	QSharedPointer<Picto::StateDataUnit> dataPoint;
	while(data->length() > 0)
	{
		dataPoint = data->takeFirstDataPoint();
		setStateVariable(dataPoint->getDataID(),TRANSITION_STATE_VAR_ID,dataPoint->toXml());

		cachedDirectorSessionData_->insertData(TRANSITIONS_TYPE,dataPoint->getDataID(),QVariantList()
																			<< dataPoint->getDataID()
																			<< dataPoint->getTransitionID()
																			<< dataPoint->getActionFrame());
	}	
}

/*! \brief Inserts a TaskRunDataUnit record into the appropriate SessionData objects.  Also takes care of adding an end time
 *	to an already existing TaskRunDataUnit record if the Task Run has ended.
 */
void SessionInfo::insertTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data)
{
	QMutexLocker locker(&sessionDataMutex_);
	//QSqlDatabase sessionDb = getSessionDb();

	//QSqlQuery query(sessionDb);
	if(data->endFrame_ == 0)
	{	//This is the beginning of the run
		currSessionDataPack_->setTaskRun(data);
		storedSessionData_->insertData(RUNS_TYPE,QVariantList() 
										<< data->getDataID()
										<< data->name_
										<< data->notes_
										<< data->startFrame_
										<< data->endFrame_
										<< false);	//saved
	}
	else
	{	//This is the end of the run, just update the endFrame column
		QSharedPointer<Picto::TaskRunDataUnit> run = currSessionDataPack_->getTaskRunByRunId(data->getDataID());
		if(run)
		{
			run->endFrame_ = data->endFrame_;
			storedSessionData_->insertData(RUNS_TYPE,QVariantList() 
										<< run->getDataID()
										<< run->name_
										<< run->notes_
										<< run->startFrame_
										<< run->endFrame_
										<< run->saved_);	//saved
		}
	}
}

/*! \brief Modifies an existing TaskRunDataUnit database entry.  This is used by the Workstation to change the Run name, notes, or saved value.
*/
void SessionInfo::modifyTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data)
{
	QMutexLocker locker(&sessionDataMutex_);
	QSharedPointer<Picto::TaskRunDataUnit> run = currSessionDataPack_->getTaskRunByRunId(data->getDataID());
	if(run)
	{
		run->name_ = data->name_;
		run->notes_ = data->notes_;
		run->saved_ = data->saved_;

		storedSessionData_->insertData(RUNS_TYPE,QVariantList() 
										<< run->getDataID()
										<< run->name_
										<< run->notes_
										<< run->startFrame_
										<< run->endFrame_
										<< run->saved_);	//saved
	}
}

/*! \brief Inserts a FrameDataUnitPackage record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertFrameData(QSharedPointer<Picto::FrameDataUnitPackage> data)
{
	//QSqlDatabase cacheDb = getCacheDb();
	//QSqlQuery cacheQ(cacheDb);
	QSharedPointer<Picto::FrameDataUnit> framedata;
	bool hadData = false;
	while(data->length() > 0)
	{
		hadData = true;
		framedata = data->takeFirstDataPoint();
		cachedDirectorSessionData_->insertData(FRAMES_TYPE,framedata->getDataID(),QVariantList()<<framedata->getDataID()<<framedata->time);
	}
	//Currently we require that frame data be sent to the server as soon as all 
	//data that was valid for that frame has been sent and before any other data arrives.
	//This means that as soon as we insert frame data, its time to flushStateBufferToCurrentState
	//so that any workstations reading in current state data will have a picture of the latest valid
	//state
	if(hadData)
	{	
		setStateVariable(framedata->getDataID(),FRAME_STATE_VAR_ID,framedata->toXml());
		updateCurrentStateTable(framedata->time);
	}
}

/*! \brief Inserts a RewardDataUnit record into the CachedSessionData and the record of the current Session state.  
 *	The CachedSessionData will later be flushed to disk in flushCache().
 */
void SessionInfo::insertRewardData(QSharedPointer<Picto::RewardDataUnit> data)
{
	//QSqlDatabase cacheDb = getCacheDb();
	//QSqlQuery query(cacheDb);
	cachedDirectorSessionData_->insertData(REWARDS_TYPE,data->getDataID(),QVariantList()
														<< data->getDataID()
														<< data->getDuration()
														<< data->getChannel()
														<< data->getTime());

	setStateVariable(data->getDataID(),REWARD_STATE_VAR_ID,data->toXml());
	
	QSharedPointer<ComponentInfo> sourceComponent = getComponentByType("DIRECTOR");
	//When the experiment is not running, Update the current state
	//table right away.
	if(sourceComponent && (sourceComponent->getStatus() != ComponentStatus::running))
	{
		updateCurrentStateTable(data->getTime());
	}
}

/*! \brief Returns a serialized version of all State Variables that have changed since the input time.
 *	\details We use a single table to track the latest state of all variables pertinant to running
 *	a picto workstation.  Using this table, we can assure requests for the latest picto
 *	state will have constant runtime independant of the previous runtime of the current
 *	experiment.  New variable values always update existing values with the same id if 
 *	their timestamp is greater than that of their predecessor.  Calling selectStateVariables
 *	will return a string that can be sent to a workstation.  This string contains all of the
 *	stateVariables that have changed after (not including) the input time.  The format of 
 *	the returned variables is just the serialization that the director used when sending
 *	them to the server.
 */
QString SessionInfo::selectStateVariables(QString fromTime)
{
	QString result;
	SerialSessionData serialSessionData;
	currentSessionData_->copyDataTo(&serialSessionData,fromTime);
	result = serialSessionData.takeSerializedData();
	return result;
}

/*! \brief Returns a serialized version of all Neural data since the input time.
 *	\sa selectStateVariables()
 */
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

/*! \brief Returns the latest SessionDataPackage for this Session in XML serial form.
 *	\details The SessionDataPackage includes data about all Task Runs that have occured 
 *	in this Session.
 */
QString SessionInfo::selectSessionDataPackage()
{
	QString result;
	if(!sessionDataMutex_.tryLock())
		return result;
	result = currSessionDataPack_->toXml();
	sessionDataMutex_.unlock();
	return result;
}

/*! \brief Initializes all SessionInfo member variables for a new SessionInfo object.
*/
void SessionInfo::InitializeVariables()
{
	databaseWriteMutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	activity_ = true;
	ignoreComponents_ = false;
	latestStateVarTime_ = "0.0";
	latestWrittenStateVarTime_ = "0.0";
	lastFlushStartTime_ = 0;
	lastFlushEndTime_ = 0;
	totalSessionTime_.start();
	timeInFlush_ = 0;
	sessionInfoDataId_ = 0;
	aligmentInfoDataId_ = 0;
	//CreateUUID
	if(uuid_ == QUuid())
	{
		uuid_ = QUuid::createUuid();
	}
	databaseVersion_ = PICTOVERSION;	//As of Pictoversion 1.0.28, we just put pictoversion into the database version field.  Before this it was 1.0.
										//In this version, we changed the SessionInfo table such that it now has a dataId field that can be used to
										//update individual rows.
	nextSigChanVarId_ = MAX_SIG_CHAN_VAR_ID;

#ifdef NO_AUTH_REQUIRED
	//Add a null QUuid so that everyone is considered an authorized user
	authorizedObservers_.append(QUuid());
#endif
	latestNeuralTimestamp_ = 0;
	latestBehavioralTimestamp_ = 0;
	currSessionDataPack_ = QSharedPointer<Picto::SessionDataPackage>(new Picto::SessionDataPackage());

	//create alignment tool
	alignToType_ = "";
	alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool());
}

/*! \brief Creates all SessionData object necessary for storing Session data in this SessionInfo object.
 *	\details The name of this function is historical and should probably be changed to something more
 *	suitable.  Alternatively, this can all just be moved to InitializeVariables().
 */
void SessionInfo::LoadBaseSessionDatabase(QString path, QString databaseName)
{
	storedSessionData_ = QSharedPointer<StoredSessionData>(new StoredSessionData(path,databaseName));
	cachedDirectorSessionData_ = QSharedPointer<CachedSessionData>(new CachedSessionData());
	cachedProxySessionData_ = QSharedPointer<CachedSessionData>(new CachedSessionData());
	behavAlignSessionData_ = QSharedPointer<AlignSessionData>(new AlignSessionData(BEHAVIORAL_ALIGN_EVENTS_TYPE,BEHAV_MATCHED_COLUMN));
	neuralAlignSessionData_ = QSharedPointer<AlignSessionData>(new AlignSessionData(NEURAL_ALIGN_EVENTS_TYPE,NEURAL_MATCHED_COLUMN));
	currentSessionData_ = QSharedPointer<CurrentSessionData>(new CurrentSessionData(CURRENT_STATE_TYPE));
	baseSessionDbFilepath_ = path + "/" + databaseName;
}

/*! \brief Sets up the SessionData objects created in LoadBaseSessionDatabase().  In the case where this is
 *	a previously timed out Session, data in the existing Sqlite database needed to initialize this SessionInfo
 *	object is loaded into it.
 */
void SessionInfo::SetupBaseSessionDatabase()
{
	//This was here when we used to handle all the SQL stuff directly in this file.  I have left
	//it here for reference.
	////Set the synchronous setting: 
	////FULL is fully protected from application crash and system failure - This is the Sqlite default
	////NORMAL is fully protected from application crash and most system failure case won't corrupt
	////			the database.  In practice, a catastrophic disk or hardware fault is more likely to
	////			occur.
	////OFF is fully protected from application crash.  System failures could corrupt data.
	//executeWriteQuery(&sessionQ,"PRAGMA synchronous = OFF");
	////The DELETE (default) journaling mode is the normal behavior. In the DELETE mode, the rollback 
	////journal is deleted at the conclusion of each transaction. Indeed, the delete operation is the 
	////action that causes the transaction to commit. (See the documented titled Atomic Commit In SQLite 
	////for additional detail.)
	////The WAL journaling mode uses a write-ahead log instead of a rollback journal to implement 
	////transactions. The WAL journaling mode is persistent; after being set it stays in effect across 
	////multiple database connections and after closing and reopening the database. A database in WAL 
	////journaling mode can only be accessed by SQLite version 3.7.0 or later. This mode can be significantly
	////faster than DELETE in cases where there are significantly more writes than reads.  This mode uses
	////three files, which makes it distastful for using sqlite as a file type.
	//executeWriteQuery(&sessionQ,"PRAGMA journal_mode = DELETE");
	////Set the temp_store setting: 
	////The temp_store values specifies the type of database back-end to use for temporary files. 
	////The choices are DEFAULT (0), FILE (1), and MEMORY (2). The use of a memory database for 
	////temporary tables can produce signifigant savings. DEFAULT specifies the compiled-in default, 
	////which is FILE unless the source has been modified.
	//executeWriteQuery(&sessionQ,"PRAGMA temp_store = 2");






	//Define database
	storedSessionData_->setTableInfo(SESSION_INFO_TYPE,"sessioninfo", " dataid,key,value "," INTEGER UNIQUE ON CONFLICT REPLACE,TEXT,TEXT ");
	storedSessionData_->setTableInfo(COMPONENT_INFO_TYPE,"componentinfo", " componentid,address,name,type,active "," TEXT,TEXT,TEXT,TEXT,INTEGER ");
	storedSessionData_->setTableInfo(RUNS_TYPE,"runs", " runid,name,notes,firstframe,lastframe,saved "," INTEGER UNIQUE ON CONFLICT REPLACE,TEXT,TEXT,INTEGER,INTEGER,INTEGER ");
	storedSessionData_->setTableInfo(SPIKES_TYPE,"spikes", " dataid,timestamp,channel,unit,waveform "," INTEGER UNIQUE ON CONFLICT IGNORE,REAL,INTEGER,INTEGER,BLOB ");
	storedSessionData_->setTableInfo(NEURAL_ALIGN_EVENTS_TYPE,"neuralalignevents", " dataid,timestamp,aligncode,matched "," INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER ");
	storedSessionData_->setTableInfo(LFP_TYPE,"lfp", " dataid,timestamp,channel,data "," INTEGER UNIQUE ON CONFLICT IGNORE,REAL,INTEGER,BLOB ");
	storedSessionData_->setTableInfo(BEHAVIORAL_ALIGN_EVENTS_TYPE,"behavioralalignevents", " dataid,timestamp,aligncode,aligneventnumber,matched "," INTEGER UNIQUE ON CONFLICT REPLACE,REAL,INTEGER,INTEGER,INTEGER ");
	storedSessionData_->setTableInfo(ALIGN_EVENTS_TYPE,"alignevents", " aligneventnumber,aligncode,neuraltime,behavioraltime,jitter,correlation "," INTEGER UNIQUE ON CONFLICT REPLACE,INTEGER,REAL,REAL,REAL,REAL ");
	storedSessionData_->setTableInfo(TRANSITIONS_TYPE,"transitions", " dataid,transid,frameid "," INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,INTEGER"," transid,frameid ");
	storedSessionData_->setTableInfo(TRANSITION_LOOKUP_TYPE,"transitionlookup", " assetid,parent,source,sourceresult,destination "," INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,TEXT,TEXT,TEXT "," parent,source,sourceresult,destination ");
	storedSessionData_->setTableInfo(PROPERTIES_TYPE,"properties", " dataid,assetid,value,frameid "," INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,TEXT,INTEGER "," assetid,frameid ");
	storedSessionData_->setTableInfo(INIT_PROPERTIES_TYPE,"initproperties", " dataid,assetid,value,frameid "," INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,TEXT,INTEGER "," assetid,frameid ");
	storedSessionData_->setTableInfo(PROPERTY_LOOKUP_TYPE,"propertylookup"," assetid,name,parent "," INTEGER UNIQUE ON CONFLICT IGNORE,TEXT,INTEGER "," name,parent ");
	storedSessionData_->setTableInfo(ELEMENT_LOOKUP_TYPE,"elementlookup", " assetid,path "," INTEGER UNIQUE ON CONFLICT IGNORE,TEXT "," path ");
	storedSessionData_->setTableInfo(FRAMES_TYPE,"frames", " dataid,time "," INTEGER UNIQUE ON CONFLICT IGNORE,REAL ");
	storedSessionData_->setTableInfo(REWARDS_TYPE,"rewards", " dataid,duration,channel,time "," INTEGER UNIQUE ON CONFLICT IGNORE,INTEGER,INTEGER,REAL ");
	storedSessionData_->setTableInfo(CURRENT_STATE_TYPE,"currentstate", " dataid,variableid,time,data "," INTEGER,INTEGER UNIQUE ON CONFLICT REPLACE,REAL,TEXT ");

	//Now that tables have been initialized.  Lets go through initialization data, and set whatever
	//data they don't have yet.
	QList<QVariantList> currSessionInfo = storedSessionData_->peekData(SESSION_INFO_TYPE);
	QList<QVariantList> currTransLookup = storedSessionData_->peekData(TRANSITION_LOOKUP_TYPE);
	QList<QVariantList> currPropLookup = storedSessionData_->peekData(PROPERTY_LOOKUP_TYPE);
	QList<QVariantList> currElemLookup = storedSessionData_->peekData(ELEMENT_LOOKUP_TYPE);

	QSharedPointer<Picto::DesignConfig> designConfig;
	QSharedPointer<CachedSessionData> tempCachedSessionData(new CachedSessionData());
	if(currSessionInfo.isEmpty())
	{
		//Add the Database Version
		tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "DatabaseVersion" << databaseVersion_);
		
		//Add the current time
		timeCreated_ = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
		tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "sessionstart" << timeCreated_);

		//Add the session ID
		tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "SessionID" << uuid_.toString());
		storedSessionData_->setSessionId(uuid_);

		//Add the password
		tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "Password" << password_);

		//Add authorized observers
		foreach(QUuid observer, authorizedObservers_)
		{
			tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "AuthObserverID" << observer.toString());
		}

		//Add the DesignXML
		tempCachedSessionData->insertData(SESSION_INFO_TYPE,++sessionInfoDataId_,QVariantList() << sessionInfoDataId_ << "DesignXML" << QString(designXml_));
	}

	//Create Experiment Lookup tables
	if(currTransLookup.isEmpty() || currPropLookup.isEmpty() || currElemLookup.isEmpty())
	{	//Only build the DesignConfig object if you need it.
		designConfig = QSharedPointer<Picto::DesignConfig>(new Picto::DesignConfig());
		designConfig->fromXml(DesignConfig_);
	}
	if(designConfig)
	{
		// If the database doesn't yet contain a transition lookup table, we need to add it.
		QList<Picto::TransInfo> transInfo;
		QList<Picto::PropInfo> propInfo;
		QList<Picto::AssetInfo> elemInfo;
		if(currTransLookup.isEmpty())
			transInfo = designConfig->getTransitionInfo();
		// If the database doesn't yet contain a property lookup table, we need to add it.
		if(currPropLookup.isEmpty())
			propInfo = designConfig->getPropertyInfo();
		// If the database doesn't yet contain an element lookup table, we need to add it.
		if(currElemLookup.isEmpty())
			elemInfo = designConfig->getElementInfo();

		foreach(Picto::TransInfo infounit, transInfo)
		{
			tempCachedSessionData->insertData(TRANSITION_LOOKUP_TYPE,++sessionInfoDataId_,QVariantList() << infounit.id << infounit.parent << infounit.source << infounit.result << infounit.dest);
		}
		foreach(Picto::PropInfo infounit, propInfo)
		{
			tempCachedSessionData->insertData(PROPERTY_LOOKUP_TYPE,++sessionInfoDataId_,QVariantList() << infounit.id << infounit.name << infounit.parent );
		}
		foreach(Picto::AssetInfo infounit, elemInfo)
		{
			tempCachedSessionData->insertData(ELEMENT_LOOKUP_TYPE,++sessionInfoDataId_,QVariantList() << infounit.id << infounit.path);
		}
	}
	
	//Now that everything has been written to the tempCachedSessionData object.  Move it all to the StoredSessionData object in one
	//big transaction
	tempCachedSessionData->moveDataTo(storedSessionData_.data());
}

/*! \brief Should be used with SessionData values of BEHAVIORAL_ALIGN_EVENTS_TYPE.  Returns
 *	true if the event represented by vList1 happened before the event represented by vList2.
 */
bool behavAlignLessThan(const QVariantList& vList1, const QVariantList& vList2)
{
	return vList1[BEHAV_TIMESTAMP_COLUMN].toDouble() < vList2[BEHAV_TIMESTAMP_COLUMN].toDouble();
}

/*! \brief Should be used with SessionData values of NEURAL_ALIGN_EVENTS_TYPE.  Returns
 *	true if the event represented by vList1 happened before the event represented by vList2.
 */
bool neuralAlignLessThan(const QVariantList& vList1, const QVariantList& vList2)
{
	return vList1[NEURAL_TIMESTAMP_COLUMN].toDouble() < vList2[NEURAL_TIMESTAMP_COLUMN].toDouble();
}

/*! \brief Runs the alignment algorithm to calculate the offset and linear coefficient that aligns
 *	the "PROXY" and "DIRECTOR" timestreams.
 *	\details Whenever this function is called, it checks which behavioral and neural alignment events
 *	have not yet been matched, then goes through and attempts to make the match.  When a match is 
 *	found, AlignmentTool::updateCoefficients() is used to improven the precision of the alignment parameters
 *	using a least squares algorithm.
 *
 *	When realignAll is set to true, all alignment events from the entire session are rematched and the
 *	least squares timestamp alignment parameters are recalculated.  This takes care of possible losses in
 *	precision that may arise if there is a connection problem and some alignment events arrive out of order
 *	during the Session.
 */
void SessionInfo::alignTimeBases(bool realignAll)
{
	if(realignAll)
	{
		// Make sure that everything is written to file
		flushCache();
		//Clear Neural and Behavioral align session data objects in case somethign wasn't yet matched (meaning that flushCache wouldn't
		//cause that data to be erased) and refill them with all align events from the entire experiment
		behavAlignSessionData_->clearData();
		neuralAlignSessionData_->clearData();

		//Get all align values from entire session for behavioral and neural
		QList<QVariantList> behavAlignEvents = storedSessionData_->peekData(BEHAVIORAL_ALIGN_EVENTS_TYPE);
		QList<QVariantList> neuralAlignEvents = storedSessionData_->peekData(NEURAL_ALIGN_EVENTS_TYPE);

		//Reset all matched values to zero (ummatched) and put them back into their respective align data object
		foreach(QVariantList varList,behavAlignEvents)
		{
			varList[BEHAV_MATCHED_COLUMN] = 0;
			behavAlignSessionData_->insertData(varList[0].toInt(),varList);
		}
		foreach(QVariantList varList,neuralAlignEvents)
		{
			varList[NEURAL_MATCHED_COLUMN] = 0;
			neuralAlignSessionData_->insertData(varList[0].toInt(),varList);
		}

		//Reset latest aligned timestamp values for behavioral and neural to zero
		latestBehavioralTimestamp_ = 0;
		latestNeuralTimestamp_ = 0;

		//Reset alignment tool coefficients to initial conditions
		alignmentTool_->resetValues();
	}

	//Get behavioral and neural align events
	QList<QVariantList> behavAlignEvents = behavAlignSessionData_->peekData(BEHAVIORAL_ALIGN_EVENTS_TYPE);
	QList<QVariantList> neuralAlignEvents = neuralAlignSessionData_->peekData(NEURAL_ALIGN_EVENTS_TYPE);

	//Sort both lists by timestamp
	qSort(behavAlignEvents.begin(),behavAlignEvents.end(),behavAlignLessThan);
	qSort(neuralAlignEvents.begin(),neuralAlignEvents.end(),neuralAlignLessThan);

	//Set everything that's unmatched in lists below last aligned time to be ignored (we ignore align codes that are sent out of order)
	for(QList<QVariantList>::Iterator it = behavAlignEvents.begin();it != behavAlignEvents.end();it++)
	{
		if((*it)[BEHAV_TIMESTAMP_COLUMN].toDouble() > latestBehavioralTimestamp_)
			break;
		if((*it)[BEHAV_MATCHED_COLUMN] == 1)
			continue;
		(*it)[BEHAV_MATCHED_COLUMN] = -1;
	}
	for(QList<QVariantList>::Iterator it = neuralAlignEvents.begin();it != neuralAlignEvents.end();it++)
	{
		if((*it)[NEURAL_TIMESTAMP_COLUMN].toDouble() > latestNeuralTimestamp_)
			break;
		if((*it)[NEURAL_MATCHED_COLUMN] == 1)
			continue;
		(*it)[NEURAL_MATCHED_COLUMN] = -1;
	}

	//Move through behavioral and neural lists, find every aligncode that matches.  Where they match,
	//set their matched values and put a new alignment event row in the cachedDirectorSessionData_
	QList<QVariantList>::Iterator bIt = behavAlignEvents.begin();
	QList<QVariantList> newAlignEvents;
	if(bIt != behavAlignEvents.end())
	{
		do
		{
			//If this behavioral value is not matchable, move onto the next value
			if((*bIt)[BEHAV_MATCHED_COLUMN].toInt() != 0)
			{
				continue;
			}
			//Find the next matchable neural value who's event code matches the behavioral event code
			QList<QVariantList>::Iterator nIt = neuralAlignEvents.begin();
			if(nIt != neuralAlignEvents.end())
			{
				do
				{
					//If this neural value is not matchable, move onto the next value
					if((*nIt)[NEURAL_MATCHED_COLUMN].toInt() != 0)
					{
						continue;
					}
					//Check if the neural and behavioral align codes match
					//Note: We compare the behavioral align event to only the bottom 7 digits of the neural align event.  This way
					//systems can bind the top hardware bit to high if need be (ie. For -onesided mode) and it won't affect alignment
					//int neuralEventCode = (*nIt)[NEURAL_EVENT_CODE_COLUMN].toInt() & 0x7F;
					//int behavEventCode = (*bIt)[BEHAV_EVENT_CODE_COLUMN].toInt() & 0x7F;
					if(((*nIt)[NEURAL_EVENT_CODE_COLUMN].toInt() & 0x7F) == ((*bIt)[BEHAV_EVENT_CODE_COLUMN].toInt() & 0x7F))
					{	//They Match!
						//Set both alignment events' matched values
						(*nIt)[NEURAL_MATCHED_COLUMN] = 1;
						(*bIt)[BEHAV_MATCHED_COLUMN] = 1;
						//Set the "latest matched align code" timestamps
						latestBehavioralTimestamp_ = (*bIt)[BEHAV_TIMESTAMP_COLUMN].toDouble();
						latestNeuralTimestamp_ = (*nIt)[NEURAL_TIMESTAMP_COLUMN].toDouble();
						//Update coefficients in the alignment tool
						alignmentTool_->updateCoefficients(latestBehavioralTimestamp_,latestNeuralTimestamp_);
						//Add a new align event to our newAlignEvents list
						QVariantList alignEvent;
						alignEvent	<< (*bIt)[3]//aligneventnumber
									<< (*bIt)[BEHAV_EVENT_CODE_COLUMN]//aligncode
									<< 	latestNeuralTimestamp_//neuraltime
									<< 	latestBehavioralTimestamp_//behavioraltime
									<< 	alignmentTool_->getJitter(latestBehavioralTimestamp_,latestNeuralTimestamp_)//jitter
									<< 	alignmentTool_->getCorrelationCoefficient();//correlation
						newAlignEvents.append(alignEvent);
						//Move to the next neural align event
						nIt++;
						//break out of loop
						break;
					}
				} while(++nIt != neuralAlignEvents.end());
			}
		} while(++bIt != behavAlignEvents.end());
	}

	// Update all jitter and correlation values in our new align events list with the latest calculated alignment coefficients
	// then put the events into the cachedDirectorSessionData
	for(QList<QVariantList>::Iterator it = newAlignEvents.begin();it != newAlignEvents.end();it++)
	{
		(*it)[4] = alignmentTool_->getJitter((*it)[3].toDouble(),(*it)[2].toDouble());//Jitter
		(*it)[5] = alignmentTool_->getCorrelationCoefficient();//CorrelationCoefficient
		//put the new alignment event row in the cachedDirectorSessionData_
		cachedDirectorSessionData_->insertData(ALIGN_EVENTS_TYPE,(*it)[0].toInt(),(*it));
	}

	//Update AlignSessionData objects with the latest values.
	foreach(QVariantList varList,behavAlignEvents)
	{
		behavAlignSessionData_->insertData(varList[0].toInt(),varList);
	}
	foreach(QVariantList varList,neuralAlignEvents)
	{
		neuralAlignSessionData_->insertData(varList[0].toInt(),varList);
	}

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

	//Replace the previous SessionInfo alignment info with updated data.
	if(aligmentInfoDataId_ <= 0)
	{	//Figure out what the alignment info dataId is.
		QList<QVariantList> sessionInfoList = storedSessionData_->peekData(SESSION_INFO_TYPE);
		foreach(QVariantList varList,sessionInfoList)
		{
			if(varList[1].toString() == "AlignmentInfo")
			{
				aligmentInfoDataId_ = varList[0].toInt();
				break;
			}
		}
		if(aligmentInfoDataId_ <=0)
		{	//Looks like it hasn't been initialized.  Do that now.
			aligmentInfoDataId_ = ++sessionInfoDataId_;
		}
	}
	cachedDirectorSessionData_->insertData(SESSION_INFO_TYPE,1,QVariantList()<<aligmentInfoDataId_<<"AlignmentInfo"<<inf->toXml());//This would cause us to end up with multipole "AlignmentInfo" values.

	//Put all updated values into database
	if(realignAll)
		flushCache();
}

/*! \brief Builds all table indeces that have been set in StoredSessionData to improve Session read query speed.
 *	\details This should be done only at the end of a session since extra unnecessary indeces slow down data write
 *	speed in SQL significantly.
 */
void SessionInfo::createSessionIndeces()
{
	storedSessionData_->buildTableIndeces();
}

/*! \brief Adds a new Picto current state variable to a list that will be added to a CurrentSessionData object
 *	for the purpose of updating the Picto Workstation with the current experimental state.  The list's data is
 *	copied to the CurrentSessionData object in updateCurrentStateTable().
 *	\details We use a single CurrentSessionData object to track the latest state of all variables needed to run
 *	a remote session on a Picto Workstation.  Using this object, we can assure requests for the latest Picto
 *	state will have constant runtime independant of how long the the current experiment has been running.
 *	New variable values always update existing values with the same id if their timestamp is greater than 
 *	that of their predecessor, such that the CurrentSessionData object always contains the most up to date
 *	picture of the current Experimental state.  
*/
void SessionInfo::setStateVariable(int dataid, int varid, QString serializedValue)
{
	Variable var;
	var.dataid = dataid;
	var.varid = varid;
	var.serial = serializedValue;
	currentStateQuery_.append(var);
}

/*! \brief Adds the input neural data to the latestNeuralData buffer.  
 *	\details The buffer stays NEURAL_DATA_BUFFER_SECS long, and if the input data has a timestamp of more 
 *	than NEURAL_DATA_BUFFER_SECS from the beginning of the buffer, values from the beginning are removed 
 *	accordingly.
 *
 *	This buffer is used to send neural data to Picto Workstations monitoring the current session
 *	so that the neural data can be displayed to the operator.
 */
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

/*! \brief Adds all of the experimental state variables from setStateVariable() to the CurrentSessionData object.
 *	\details We don't just do this when the values come in so that we can always be sure that all of the data for
 *	a given time is in the CurrentSessionData object when a Workstation requests it.  Otherwise, the Workstation 
 *	might miss some data if it requests data for a certain time, misses some of the final data for that time because 
 *	it hasn't come in yet and then requests data for AFTER that first time later, such that the missed data is 
 *	never read.
 *	
 *	updateTime is the time of the latest state variable to be read.
 *	\note We should look into why updateTime can't just be read from the currestStateQuery_ list.
 */
void SessionInfo::updateCurrentStateTable(QString updateTime)
{
	QList<QVariantList> currStateData;
	foreach(Variable queryVars,currentStateQuery_)
	{
		currStateData.append(QVariantList() << queryVars.dataid<<queryVars.varid<<updateTime<<queryVars.serial);
	}
	currentSessionData_->updateVariables(currStateData);
		
	//Update latest written state variable.
	if(updateTime.toDouble() > latestStateVarTime_.toDouble())
	{
		stateVarTimeMutex_.lock();
		latestStateVarTime_ = updateTime;
		stateVarTimeMutex_.unlock();
	}
	//Empty currentStateQuery_ list now that we put it in the currentSessionData_ object
	currentStateQuery_.clear();
}

/*! \brief Adds the Workstation with the input Uuid to the authorized observer list.  
 *	\details This means that Workstation will be able to use commands to change the Session 
 *	state and not just watch what is happening in the Session.
 */
void SessionInfo::addAuthorizedObserver(QUuid observerId)
{
	if(isAuthorizedObserver(observerId))
		return;

	storedSessionData_->insertData(SESSION_INFO_TYPE,QVariantList() << ++sessionInfoDataId_ << "AuthObserverID" << observerId.toString());
	authorizedObservers_.append(observerId);
}
#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include "../datacollection/alignmenttool.h"
#include "../network/serverthread.h"
#include "../../common/storage/NeuralDataUnit.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/InputDataUnitPackage.h"
#include "../../common/storage/PropertyDataUnitPackage.h"
#include "../../common/storage/StateDataUnitPackage.h"
#include "../../common/storage/FrameDataUnitPackage.h"
#include "../../common/storage/RewardDataUnit.h"
#include "../../common/storage/AlignmentDataUnit.h"
#include "../../common/storage/LFPDataUnitPackage.h"
#include "../../common/storage/DataSourceInfo.h"
#include "../../common/storage/AlignmentInfo.h"
#include "../../common/storage/TaskRunDataUnit.h"
#include "../../common/storage/SessionDataPackage.h"
#include "StoredSessionData.h"
#include "CachedSessionData.h"
#include "CurrentSessionData.h"
#include "AlignSessionData.h"
#include "ComponentInfo.h"

#include <QSharedPointer>
#include <QWeakPointer>
#include <QSqlDatabase>
#include <QUuid>
#include <QStringList>
#include <QMutex>
#include <QLinkedList>

/*!	\brief Manages data for a single Session.  Stores data needed for Session monitoring in RAM and Session data
 *	needed for future Analysis on Disk.  
 *
 *	\details The Picto Server is designed to handle multiple concurrent sessions and to allow Sessions to timeout
 *	and be reloaded later.  SessionInfo objects help with this task by keeping track of a single Session and its 
 *	associated data.  They take care of things like:
 *	- Writing data coming in from the Director and Proxy to disk.  
 *	- Storing the current values of all Properties in a Session design so that the workstation can quickly display 
 *	the most up to date Experimental state
 *	- Managing timestamp alignment to synchonize Neural and Behavioral timestreams
 *	- Generating a SessionId
 *	- Loading data from a previously timed out Session
 *	- etc.
 *	The SessionInfo class works closely with the SessionData class and its descendants since objects of these classes
 *	store the bulk of the data that comes into the SessionInfo object.  This is a very important class.  It is
 *	fairly central to the function of the Picto Server.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SessionInfo
{
public:
	static QSharedPointer<SessionInfo> CreateSession(QString designName, QString directorName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password);
	static QSharedPointer<SessionInfo> LoadSession(QString sessionID, QString databaseFilePath);
	static void deleteSession(SessionInfo* session);
	virtual ~SessionInfo();

	void AddComponent(QSharedPointer<ComponentInfo> component);
	void UpdateComponentActivity();
	bool hasActiveComponents();
	/*! \brief After this function is called, hasActiveComponents will return false.*/
	void ignoreComponents(){ignoreComponents_ = true;}
	QSharedPointer<ComponentInfo> getComponentByType(QString type);
	bool hasComponent(QUuid componentID);
	void alignTimestampsTo(QString componentType);
	bool endSession();

	bool flushCache(QString sourceType = "");
	//void insertTrialEvent(double time, int eventCode, int trialNum, QString sourceType, qulonglong dataID );
	void insertNeuralData(QSharedPointer<Picto::NeuralDataUnit> data);
	void insertPropertyData(QSharedPointer<Picto::PropertyDataUnitPackage> data);
	void insertBehavioralData(QSharedPointer<Picto::BehavioralDataUnitPackage> data);
	void insertInputData(QSharedPointer<Picto::InputDataUnitPackage> data);
	void insertAlignmentData(QSharedPointer<Picto::AlignmentDataUnit> data);
	void insertLFPData(QSharedPointer<Picto::LFPDataUnitPackage> data);
	void insertFrameData(QSharedPointer<Picto::FrameDataUnitPackage> data);
	void insertRewardData(QSharedPointer<Picto::RewardDataUnit> data);
	void insertStateData(QSharedPointer<Picto::StateDataUnitPackage> data);
	void insertTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data);
	void modifyTaskRunData(QSharedPointer<Picto::TaskRunDataUnit> data);

	QString selectStateVariables(QString fromTime);
	QString selectLatestNeuralData(QString fromDataId);
	QString selectSessionDataPackage();

	//getters/setters
	/*! \brief Returns the Uuid used to identify this Session.*/
	QUuid sessionId() { return uuid_; };
	/*! \brief Returns the filepath of the Sqlite file storing all of this Session's data.*/
	QString dataBaseFilePath() { return baseSessionDbFilepath_; };
	/*! \brief Return the time that this Session was created.*/
	QString timeCreated() { return timeCreated_; };
	/*! \brief Returns a pointer to the AlignmentTool that calculates the alignment
	 *	parameters used to synchronize neural and behavioral data.
	 */
	QSharedPointer<AlignmentTool> alignmentTool() { return alignmentTool_; };
	/*! \brief Returns the XML that defines the design used in this Session.*/
	QByteArray experimentXml() { return designXml_; };

	QString pendingDirective(QUuid componentID);
	void addPendingDirective(QString directive, QString componentType);

	/*! \brief Clears the state of activity and returns it.
	*/
	bool clearActivity() {bool temp = activity_; activity_ = false; return temp; };
	/*! \brief Marks this Session as active.  This indicates that the Components that are
	 *	part of this Session are still communicating with the Server and the Session has not
	 *	been ended.
	 */
	void setActivity() { activity_ = true; };

	bool isAuthorizedObserver(QUuid observerId);

	bool addAuthorizedObserver(QUuid observerId, QString password);
	QString getPassword();
	void markLastDataTime(QString source);
	bool lastDataPrecededFlush(QString source);
	bool lastDataFollowedFlush(QString source);

	friend class ConnectionManager;

private:
	enum SessionTableType{	SESSION_INFO_TYPE,
							COMPONENT_INFO_TYPE,
							RUNS_TYPE,
							SPIKES_TYPE,
							NEURAL_ALIGN_EVENTS_TYPE,
							LFP_TYPE,
							BEHAVIORAL_ALIGN_EVENTS_TYPE,
							ALIGN_EVENTS_TYPE,
							TRANSITIONS_TYPE,
							TRANSITION_LOOKUP_TYPE,
							PROPERTIES_TYPE,
							INIT_PROPERTIES_TYPE,
							PROPERTY_LOOKUP_TYPE,
							ELEMENT_LOOKUP_TYPE,
							FRAMES_TYPE,
							REWARDS_TYPE,
							CURRENT_STATE_TYPE,
							FIRST_SIGNAL_CHANNEL_TYPE};
	QMap<QString,int> sigChanTypes_;	//!< Store the SessionTableType of the signal channel indexed by its name
	SessionInfo(QString designName, QString directorName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password);
	SessionInfo(QString databaseFilePath);
	void InitializeVariables();
	void LoadBaseSessionDatabase(QString path, QString databaseName);
	void SetupBaseSessionDatabase();
	void CreateCacheDatabase(QString databaseName);
	void AddTablesToDatabase(QSqlQuery* query);
	void alignTimeBases(bool realignAll = false);
	void createSessionIndeces();
	void setStateVariable(int dataid, int varid, QString serializedValue);
	void setLatestNeuralData(int dataid, QString serializedValue, double fittedTime); 

	void updateCurrentStateTable(QString updateTime);
	void addAuthorizedObserver(QUuid observerId);

	static QMap<QUuid,QWeakPointer<SessionInfo>> loadedSessions_;
	QUuid uuid_;
	QString password_;
	QSqlDatabase baseSessionDbConnection_;
	QSqlDatabase cacheDb_;

	QSharedPointer<StoredSessionData> storedSessionData_;
	QSharedPointer<CachedSessionData> cachedDirectorSessionData_;
	QSharedPointer<CachedSessionData> cachedProxySessionData_;
	QSharedPointer<AlignSessionData> neuralAlignSessionData_;
	QSharedPointer<AlignSessionData> behavAlignSessionData_;
	QSharedPointer<CurrentSessionData> currentSessionData_;

	QSharedPointer<AlignmentTool> alignmentTool_;
	QMutex latestNeuralDataMutex_;
	QSharedPointer<QMutex> databaseWriteMutex_;
	QTimer timeoutTimer_;
	QMap<QUuid,QStringList> pendingDirectives_; //!< Uuid is the Uuid of the component who's pending directives are stored in the QStringList
	QMap<QString,QSharedPointer<ComponentInfo>> components_;	//!< QString is the type of the component (only one of each component type can be attached).
	QMap<QUuid,bool> componentActivity_;
	QString alignToType_;	//!< The component type who's timeframe should be used as a baseline in timing alignment.
	bool activity_;
	bool ignoreComponents_;
	int sessionInfoDataId_;
	int aligmentInfoDataId_;
	QMap<QString,bool> flushEnabled_;
	QByteArray designXml_;
	QByteArray DesignConfig_;
	QString baseSessionDbFilepath_;
	QString timeCreated_;
	double latestNeuralTimestamp_;
	double latestBehavioralTimestamp_;
	QString databaseVersion_;
	QList<QString> tables_;
	QMap<QString,QString> tableColumns_;
	QMap<QString,QString> tableColumnTypes_;
	QMap<QString,QString> tableColumnConstraints_;
	QMap<QString,QString> tableDataProviders_;
	QMap<QString,QString> tableIndexedColumns_;
	struct Variable{int dataid;int varid;QString serial;};
	QList<Variable> currentStateQuery_;
	struct NeuralVariable{int dataid;QString serial;double fittedTime;};
	QLinkedList<NeuralVariable> latestNeuralData_;
	QString latestStateVarTime_;
	QString latestWrittenStateVarTime_;
	QStringList openDatabaseConnections_;

	qulonglong maxReceivedDataID_;

	QList<QUuid> authorizedObservers_;
	QMap<QString,int> sigChanVarIDs_;
	QMap<QString,QSharedPointer<Picto::DataSourceInfo>> dataSourceInfoMap_;
	QMap<QString,QSharedPointer<Picto::AlignmentInfo>> alignmentInfoMap_;
	int nextSigChanVarId_;
	QSharedPointer<Picto::SessionDataPackage> currSessionDataPack_;
	QMutex sessionDataMutex_;
	QMutex stateVarTimeMutex_;
	QMutex flushingCache_;
	QTime totalSessionTime_;
	double timeInFlush_;
	QHash<QString,double> dataTimeBySource_;
	QMutex flushTimingMutex_;
	double lastFlushStartTime_;
	double lastFlushEndTime_;


};

#endif


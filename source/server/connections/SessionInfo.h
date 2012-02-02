#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include "../datacollection/alignmenttool.h"
#include "../network/serverthread.h"
#include "../../common/storage/NeuralDataUnit.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/PropertyDataUnitPackage.h"
#include "../../common/storage/StateDataUnitPackage.h"
#include "../../common/storage/FrameDataUnitPackage.h"
#include "../../common/storage/RewardDataUnit.h"
#include "../../common/storage/AlignmentDataUnit.h"
#include "../../common/storage/LFPDataUnitPackage.h"
#include "ComponentInfo.h"

#include <QSharedPointer>
#include <QWeakPointer>
#include <QSqlDatabase>
#include <QUuid>
#include <QStringList>
#include <QMutex>
#include <QLinkedList>

/*!	\brief Contains info about a single session
 *
 *	The Server will likely be handling multiple concurrent sessions,
 *	so we need to be able to keep track of them.  The SessionInfo
 *	object is used by the SessionManager to keep track of everything.
 *
 *	Initially this will be more of a struct than an object (although I am
 *	keeping the member variables private).  I am creating it as a seperate object
 *	since it seems possible that we may need to add functionality at a 
 *	later date.
 *
 *	Since we don't want random objects changing the values of the contained variables
 *	I am not providing setter functions.  Instead, this is a friend of the 
 *	ConnectionManager class, so it will be the only object able to set these
 *	values.  As a result, it is perfectly safe to pass these objects around
 *	(ideally we should pass around pointers to them).
 *
 *	The SessionInfo object handles all access to the Session databases (both the one
 *	on disk, and the in-memory cache database.  Database connections can't be used from 
 *	different threads, so we create a unique connection for each thread.  This is somewhat
 *	wasteful, but it's easy and gauranteed to work.  An alternative approach would be to
 *	create a special thread that handles db access through some sort of queued interface.
 *	The cache database is limited to a single connection (since it's a :memory database),
 *	so we're not controlling access to it.  This may be risky.
 *
 *	Since database access here is generally messed up, I have placed Q_ASSERTs on all
 *	calls to QSqlQuery::exec().  This will let us know as soon as there is a problem.
 */

class SessionInfo
{
public:
	static QSharedPointer<SessionInfo> CreateSession(QByteArray experimentXml, QByteArray experimentConfig, QUuid initialObserverId, QString password);
	static QSharedPointer<SessionInfo> LoadSession(QString sessionID, QString databaseFilePath);
	static void deleteSession(SessionInfo* session);
	virtual ~SessionInfo();

	void AddComponent(QSharedPointer<ComponentInfo> component);
	void UpdateComponentActivity();
	bool hasActiveComponents();
	//! After this function is called, hasActiveComponents will return false
	void ignoreComponents(){ignoreComponents_ = true;}
	QSharedPointer<ComponentInfo> getComponentByType(QString type);
	bool hasComponent(QUuid componentID);
	void alignTimestampsTo(QString componentType);
	bool endSession();

	void flushCache(QString sourceType = "");
	//void insertTrialEvent(double time, int eventCode, int trialNum, QString sourceType, qulonglong dataID );
	void insertNeuralData(QSharedPointer<Picto::NeuralDataUnit> data);\
	void insertPropertyData(QSharedPointer<Picto::PropertyDataUnitPackage> data);
	void insertBehavioralData(QSharedPointer<Picto::BehavioralDataUnitPackage> data);
	void insertAlignmentData(QSharedPointer<Picto::AlignmentDataUnit> data);
	void insertLFPData(QSharedPointer<Picto::LFPDataUnitPackage> data);
	void insertFrameData(QSharedPointer<Picto::FrameDataUnitPackage> data);
	void insertRewardData(QSharedPointer<Picto::RewardDataUnit> data);
	void insertStateData(QSharedPointer<Picto::StateDataUnitPackage> data);

	QString selectStateVariables(QString fromTime);
	QString selectLatestNeuralData(QString fromDataId);

	//getters/setters
	QUuid sessionId() { return uuid_; };
	QString dataBaseFilePath() { return baseSessionDbFilepath_; };
	QString timeCreated() { return timeCreated_; };
	QSharedPointer<AlignmentTool> alignmentTool() { return alignmentTool_; };
	QByteArray experimentXml() { return experimentXml_; };

	QString pendingDirective(QUuid componentID);
	void addPendingDirective(QString directive, QString componentType);

	void enableFlush(QString componentType){flushEnabled_[componentType] = true;};
	bool needsFlush(QString componentType){bool r = flushEnabled_[componentType]; flushEnabled_[componentType]=false; return r;};

	//! clears the state of activity and returns it.
	bool clearActivity() {bool temp = activity_; activity_ = false; return temp; };
	void setActivity() { activity_ = true; };

	bool isAuthorizedObserver(QUuid observerId);

	bool addAuthorizedObserver(QUuid observerId, QString password);
	QString getPassword();

	friend class ConnectionManager;

private:
	SessionInfo(QByteArray experimentXml, QByteArray experimentConfig, QUuid initialObserverId, QString password);
	SessionInfo(QString databaseFilePath);
	void InitializeVariables();
	void LoadBaseSessionDatabase(QString databaseName);
	void SetupBaseSessionDatabase();
	void CreateCacheDatabase(QString databaseName);
	void AddTablesToDatabase(QSqlQuery* query);
	bool executeReadQuery(QSqlQuery* query, QString optionalString = "",bool debug = false);
	bool executeWriteQuery(QSqlQuery* query, QString optionalString = "",bool lock = true,bool debug = true);
	void alignTimeBases(bool realignAll = false);
	void createSessionIndeces();
	void recalculateFittedTimes();
	void setStateVariable(int dataid, int varid, QString serializedValue);
	void setLatestNeuralData(int dataid, QString serializedValue, double fittedTime); 

	void updateCurrentStateTable(QString updateTime);
	void addAuthorizedObserver(QUuid observerId);
	QSqlDatabase getSessionDb();
	QSqlDatabase getCacheDb();

	static QMap<QUuid,QWeakPointer<SessionInfo>> loadedSessions_;
	QUuid uuid_;
	QString password_;
	QSqlDatabase baseSessionDbConnection_;
	QSqlDatabase cacheDb_;

	QSharedPointer<AlignmentTool> alignmentTool_;
	bool timestampsAligned_;	//Indicates whether initial timestamp alignment has occured
	QMutex latestNeuralDataMutex_;
	QSharedPointer<QMutex> databaseWriteMutex_;
	QTimer timeoutTimer_;
	QMap<QUuid,QStringList> pendingDirectives_; //Uuid is the Uuid of the component who's pending directives are stored in the QStringList
	QMap<QString,QSharedPointer<ComponentInfo>> components_;	//QString is the type of the component (only one of each component type can be attached).
	QMap<QUuid,bool> componentActivity_;
	QString alignToType_;	//The component type who's timeframe should be used as a baseline in timing alignment.
	bool activity_;
	bool ignoreComponents_;
	QMap<QString,bool> flushEnabled_;
	QByteArray experimentXml_;
	QByteArray experimentConfig_;
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
};

#endif


#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include "../datacollection/alignmenttool.h"
#include "../network/serverthread.h"
#include "../../common/storage/NeuralDataStore.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../../common/storage/StateDataStore.h"
#include "../../common/storage/FrameDataStore.h"
#include "../../common/storage/RewardDataStore.h"
#include "../../common/storage/AlignmentDataStore.h"
#include "ComponentInfo.h"

#include <QSharedPointer>
#include <QSqlDatabase>
#include <QUuid>
#include <QStringList>
#include <QMutex>

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
	SessionInfo(QByteArray experimentXml, QUuid initialObserverId);
	SessionInfo(QString databaseFilePath);
	~SessionInfo();

	void AddComponent(QSharedPointer<ComponentInfo> component);
	QSharedPointer<ComponentInfo> getComponentByType(QString type);
	bool hasComponent(QUuid componentID);
	void alignTimestampsTo(QString componentType);
	void endSession();

	void flushCache();
	void insertTrialEvent(double time, int eventCode, int trialNum, QString sourceType, qulonglong dataID );
	void insertNeuralData(Picto::NeuralDataStore data);
	void insertBehavioralData(Picto::BehavioralDataStore data);
	void insertAlignmentData(Picto::AlignmentDataStore data);
	void insertFrameData(Picto::FrameDataStore data);
	void insertRewardData(Picto::RewardDataStore data);

	Picto::BehavioralDataStore selectBehavioralData(double timestamp);
	void insertStateData(Picto::StateDataStore data);
	QList<Picto::StateDataStore> selectStateData(double timestamp);
	Picto::FrameDataStore selectFrameData(double timestamp);

	//getters/setters
	QUuid sessionId() { return uuid_; };
	QString dataBaseFilePath() { return baseSessionDbFilepath_; };
	QString timeCreated() { return timeCreated_; };
	QSharedPointer<AlignmentTool> alignmentTool() { return alignmentTool_; };
	QByteArray experimentXml() { return experimentXml_; };

	QString pendingDirective(QUuid componentID);
	void addPendingDirective(QString directive, QString componentType);

	//! clears the state of activity and returns it.
	bool clearActivity() {bool temp = activity_; activity_ = false; return temp; };
	void setActivity() { activity_ = true; };

	bool isAuthorizedObserver(QUuid observerId);

	friend class ConnectionManager;

private:
	void InitializeVariables();
	void LoadBaseSessionDatabase(QString databaseName);
	void SetupBaseSessionDatabase();
	void CreateCacheDatabase(QString databaseName);
	double LoadMaxDataID(QString tableName);
	QSqlDatabase getSessionDb();

	QUuid uuid_;
	QSqlDatabase baseSessionDbConnection_;
	QSqlDatabase cacheDb_;

	QSharedPointer<AlignmentTool> alignmentTool_;
	bool timestampsAligned_;	//Indicates whether initial timestamp alignment has occured
	QMutex alignmentMutex_;
	QTimer timeoutTimer_;
	QMap<QUuid,QStringList> pendingDirectives_; //Uuid is the Uuid of the component who's pending directives are stored in the QStringList
	QMap<QString,QSharedPointer<ComponentInfo>> components_;	//QString is the type of the component (only one of each component type can be attached).
	QString alignToType_;	//The component type who's timeframe should be used as a baseline in timing alignment.
	bool activity_;
	QByteArray experimentXml_;
	QString baseSessionDbFilepath_;
	QString timeCreated_;

	qulonglong maxReceivedDataID_;

	QList<QUuid> authorizedObservers_;
};

#endif


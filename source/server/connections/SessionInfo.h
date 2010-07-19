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
 *	I am not providing setter functions.  Instead, this is a fried of the 
 *	SessionManager class, so it will be the only object able to set these
 *	values.  As a result, it is perfectly safe to pass these objects around
 *	(ideally we should pass around pointers to them).
 */

#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include "../datacollection/alignmenttool.h"
#include "../datacollection/neuraldatacollector.h"
#include "../network/serverthread.h"
#include "../../common/storage/BehavioralDataStore.h"
#include "../../common/storage/StateDataStore.h"
#include "../../common/storage/FrameDataStore.h"
#include "../../common/storage/RewardDataStore.h"

#include <QSharedPointer>
#include <QSqlDatabase>
#include <QUuid>
#include <QStringList>

class SessionInfo
{
public:
	SessionInfo(QString directorAddr, int proxyId);
	~SessionInfo();

	void endSession();

	void flushCache();
	void insertTrialEvent(double time, int eventCode, int trialNum);
	void insertBehavioralData(Picto::BehavioralDataStore data);
	void insertFrameData(Picto::FrameDataStore data);
	void insertRewardData(Picto::RewardDataStore data);

	Picto::BehavioralDataStore selectBehavioralData(double timestamp);
	void insertStateData(Picto::StateDataStore data);
	QList<Picto::StateDataStore> selectStateData(double timestamp);
	Picto::FrameDataStore selectFrameData(double timestamp);

	//getters/setters
	//QSqlDatabase sessionDb() { return sessionDb_; };
	//QSqlDatabase cacheDb() { return cacheDb_; };
	QUuid sessionId() { return uuid_; };
	QSharedPointer<AlignmentTool> alignmentTool() { return alignmentTool_; };
	QString directorAddr() { return directorAddr_; };

	QString pendingDirective();
	void addPendingDirective(QString directive) { pendingDirectives_.append(directive); };

	//! clears the state of activity and returns it.
	bool clearActivity() {bool temp = activity_; activity_ = false; return temp; };
	void setActivity() { activity_ = true; };

	friend class ConnectionManager;

private:
	QSqlDatabase getSessionDb();

	QUuid uuid_;
	QString databaseName_;
	//QSqlDatabase sessionDb_;
	QSqlDatabase cacheDb_;
	QSharedPointer<AlignmentTool> alignmentTool_;
	//QSharedPointer<NeuralDataCollector> ndc_;

	int proxyId_;
	NeuralDataCollector *ndc_;
	QTimer timeoutTimer_;
	QStringList pendingDirectives_;
	QString directorAddr_;
	bool activity_;
};

#endif


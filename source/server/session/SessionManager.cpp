#include "SessionManager.h"

#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>

QMap<QUuid, QSharedPointer<SessionInfo> > SessionManager::openSessions_;


SessionManager::SessionManager()
{
}

QSharedPointer<SessionInfo> SessionManager::getSessionInfo(QUuid uuid)
{
	if(openSessions_.contains(uuid))
	{
		return openSessions_[uuid];
	}
	else
	{
		return QSharedPointer<SessionInfo>();
	}
}

//! Creates a new session and returns a pointer to the SessinoInfo object
QSharedPointer<SessionInfo> SessionManager::createSession()
{
	QSharedPointer<SessionInfo> sessInfo(new SessionInfo);

	//CreateUUID
	QUuid uuid = QUuid::createUuid();
	sessInfo->uuid_ = uuid;

	//create alignment tool and neuraldatacollector
	sessInfo->alignmentTool_ = QSharedPointer<AlignmentTool>(new AlignmentTool(sessInfo->sessionDb_));
	sessInfo->ndc_ = QSharedPointer<NeuralDataCollector>(new NeuralDataCollector(sessInfo->sessionDb_,50));

	//Create a new Session Database
	QDateTime dateTime = QDateTime::currentDateTime();
	QString databaseName="Session_"+dateTime.toString("yyyy_MM_dd_hh:mm:ss");
	
	//! \todo come up with a better place to store session data...
	QDir dir(QCoreApplication::applicationDirPath());
	while(dir.entryList().contains(databaseName))
	{
		databaseName.append("_x");
	}

	sessInfo->sessionDb_ = QSqlDatabase::addDatabase("QSQLITE",databaseName);
	sessInfo->sessionDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + databaseName + ".sqlite");
	Q_ASSERT(sessInfo->sessionDb_.open());

	QSqlQuery sessionQ(sessInfo->sessionDb_);

	sessionQ.exec("CREATE TABLE sessioninfo(id INTEGER PRIMARY KEY,"
		"key TEXT, value TEXT)");

	sessionQ.exec("CREATE TABLE spikes "
		"(id INTEGER PRIMARY KEY, timestamp REAL, fittedtime REAL,"
		"correlation REAL, channel TEXT, unit TEXT, waveform TEXT)");

	sessionQ.exec("CREATE TABLE neuraltrials "
		"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER,"
		"matched INTEGER)");

	sessionQ.exec("CREATE TABLE behavioraltrials "
		"(id INTEGER PRIMARY KEY, timestamp REAL, aligncode INTEGER"
		"trialnumber INTEGER, matched INTEGER)");

	sessionQ.exec("CREATE TABLE trials (id INTEGER PRIMARY KEY,trialnumber INTEGER,"
		"aligncode INTEGER, neuralstart REAL, neuralend REAL, "
		"behavioralstart REAL, behavioralend REAL, startjitter REAL, "
		"endjitter REAL, correlation REAL)");

	//set the Timeout interval to 30 seconds
	sessInfo->setTimeoutInterval(30000);
	connect(sessInfo.data(), SIGNAL(timeout()), this, SLOT(sessionTimedOut()));

	openSessions_[uuid] = sessInfo;
	return sessInfo;
}

//! This slot is called when a session times out
void SessionManager::sessionTimedOut()
{
	//figure out which session timed out, and remove it from our list.
	//! todo Do we need to do something differen to allow for recovery?
	SessionInfo *info = (SessionInfo*)QObject::sender();
	openSessions_.remove(info->uuid_);
}

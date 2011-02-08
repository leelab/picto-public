#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QUuid>
#include <QMutex>

#include "SessionInfo.h"
#include "DirectorInfo.h"

class ServerThread;
class QTimer;

/*! \brief	Keeps track of all server connections
 *
 *	There are 3 different components that the server talks to:
 *		PictoDirector (client)
 *		PictoWorkstation (client)
 *		PictoProxyServer (server)
 *
 *	Certain commands are used to link together these components, so the 
 *	connection manager attempts to keep track of all connections and categorize them.
 *	It also provides functions for finding specific instances, and keeping track
 *	of timed out connections.
 *
 *	The ConnectionManager is a singleton object.   The actual object will be held by the
 *	server, but we can grab an instance of it anywhere we need it (such as in the 
 *	command handlers).  
 *
 *	The multi-threaded nature of the server may present issues:
 *	- We'll use signals to start QTimers
 *	- We'll protect a lot of the data with Mutexes.
 */

class ConnectionManager : public QObject
{
	Q_OBJECT
public:

	static ConnectionManager* Instance();

	//Director related functions
	void updateDirector(QUuid uuid, QHostAddress addr, QString name, DirectorStatus::DirectorStatus status);
	void updateDirector(QUuid uuid, QHostAddress addr, QString name);
	//void removeDirector(QUuid uuid);

	DirectorStatus::DirectorStatus getDirectorStatus(QUuid uuid);
	DirectorStatus::DirectorStatus getDirectorStatus(QString uuidStr);
	DirectorStatus::DirectorStatus getDirectorStatusBySession(QUuid sessionId);
	void setDirectorStatus(QUuid sessionId, DirectorStatus::DirectorStatus status);

	QString getDirectorList();

	QString getProxyList();

	//ServerThread related functions
	//void addServerThread(ServerThread* thread);
	//bool removeServerThread(ServerThread* thread);

	//Session related functions
	QSharedPointer<SessionInfo> createSession(QUuid directorID, int proxyId, QByteArray experimentXml, QUuid initialObserverId);
	QSharedPointer<SessionInfo> getSessionInfoByDirector(QString directorID);
	QSharedPointer<SessionInfo> getSessionInfoByDirector(QUuid directorID);
	QSharedPointer<SessionInfo> getSessionInfo(QUuid uuid);
	void endSession(QUuid sessionId);
	QUuid pendingSession(QUuid directorID);

private slots:
	void checkForTimeouts();

private:
	ConnectionManager();
	ConnectionManager(ConnectionManager const&){};	//empty private copy constructor
	ConnectionManager& operator=(ConnectionManager const&){} //empty private assignment operator

	//The single allowed instance
	static ConnectionManager* conMan_;

	//List of threads
	//QMutex serverThreadListMutex_;
	//QList<ServerThread*> threads_;

	//Mutex - We are using a single mutex to protect access to everything (I tried using multiple
	//mutexes earlier and managed to deadlock the server, so even though this is slightly less
	//efficient, it's a lot safer.)
	QMutex *mutex_;

	//Map of Directors
	QMap<QUuid,QSharedPointer<DirectorInfo> > directors_;  //QUuid = director's unique user Id delivered in its messages

	//List of Sessions
	QMap<QUuid, QSharedPointer<SessionInfo> > openSessions_;
	QMap<QUuid, QUuid> pendingSessions_; //First QUuid is the directors ID, second is the Session ID

	QTimer *timeoutTimer_;

};


#endif
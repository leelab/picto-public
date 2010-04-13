/*!	\brief	keeps track of all server connections
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

class ConnectionManager : public QObject
{
	Q_OBJECT
public:

	static ConnectionManager* Instance();

	//Director related functions
	void updateDirector(QHostAddress addr, QString name, DirectorStatus::DirectorStatus status);
	//void removeDirector(QHostAddress addr);

	DirectorStatus::DirectorStatus getDirectorStatus(QHostAddress addr);
	DirectorStatus::DirectorStatus getDirectorStatus(QString addr);
	DirectorStatus::DirectorStatus getDirectorStatus(QUuid sessionId);

	QString getDirectorList();

	//ServerThread related functions
	//void addServerThread(ServerThread* thread);
	//bool removeServerThread(ServerThread* thread);

	//Session related functions
	QSharedPointer<SessionInfo> createSession(QString directorAddr);

	QSharedPointer<SessionInfo> getSessionInfo(QUuid uuid);

	QUuid pendingSession(QHostAddress directorAddr);

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
	QMap<QString,QSharedPointer<DirectorInfo> > directors_;  //QString = director address as string

	//List of Sessions
	QMap<QUuid, QSharedPointer<SessionInfo> > openSessions_;
	QMap<QString, QUuid> pendingSessions_;

	QTimer *timeoutTimer_;

};


#endif
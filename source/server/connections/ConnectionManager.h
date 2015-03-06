#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QUuid>
#include <QMutex>

#include "SessionInfo.h"
#include "ComponentInfo.h"

class ServerThread;
class QTimer;

/*! \brief	Keeps track of all active Components (Director/Proxy) as well as all active and recently timed out Sessions. 
 *	The Components used with each Session, active or timed out, are tracked as well.
 *
 *	The server talks to the Picto Director, Proxy and Workstation applications over the network.  These applications
 *	are related in that they may or may not be involved with the same Session.  The Connection Manager keeps track of 
 *	all of these components and categorize them by their activity, the session to which they are attached and whether
 *	it is active.  It also provides functions for updating information about specific components.
 *
 *	The ConnectionManager is a singleton object that is accessed by multiple threads.   The actual object will be held 
 *	by the Server, but we can grab an instance of it anywhere we need it (such as in the 
 *	command handlers).  
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ConnectionManager : public QObject
{
	Q_OBJECT
public:

	static ConnectionManager* Instance();
	~ConnectionManager();

	//Director related functions
	void updateComponent(QUuid uuid, QHostAddress addr, QUuid sessionId, QString name, QString type, ComponentStatus::ComponentStatus status, QString details);

	ComponentStatus::ComponentStatus getComponentStatus(QUuid uuid);
	ComponentStatus::ComponentStatus getComponentStatus(QString uuidStr);
	ComponentStatus::ComponentStatus getComponentStatusBySession(QUuid sessionId, QString componentType);
	void setComponentStatus(QUuid sessionId, QString componentType, ComponentStatus::ComponentStatus status);

	QUuid GetComponentsSessionId(QUuid componentId);
	QUuid GetComponentsSessionId(QString componentId);

	QString getDirectorList();

	QString getProxyList();

	//Session related functions
	QSharedPointer<SessionInfo> createSession(QUuid directorID, QUuid proxyID, QString designName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password);
	QSharedPointer<SessionInfo> loadSession(QString sessionId, QString filePath);
	QSharedPointer<SessionInfo> getSessionInfoByComponent(QString componentID);
	QSharedPointer<SessionInfo> getSessionInfoByComponent(QUuid componentID);
	QSharedPointer<SessionInfo> getSessionInfo(QUuid uuid);
	bool sessionIsValid(QString sessionId);
	bool endSession(QUuid sessionId);
	QUuid pendingSession(QUuid componentID);

private slots:
	void checkForTimeouts();

private:
	ConnectionManager();
	ConnectionManager(ConnectionManager const&){};	//!< empty private copy constructor
	ConnectionManager& operator=(ConnectionManager const&){} //!< empty private assignment operator
	void checkForDroppedSessionTimeouts();

	//! The single allowed instance of a ConnectionManager object.
	static ConnectionManager* conMan_;

	/*! Mutex - We are using a single mutex to protect access to everything (I tried using multiple
	 *mutexes earlier and managed to deadlock the server, so even though this is slightly less
	 *efficient, it's a lot cleaner.)
	 */
	QMutex *mutex_;

	//Map of passive components (ie. Directors and Proxies)
	QMap<QUuid,QSharedPointer<ComponentInfo> > components_;  //!< QUuid = component's unique user Id delivered in its messages

	//List of Sessions
	QMap<QUuid, QSharedPointer<SessionInfo> > openSessions_;
	QMap<QUuid, QUuid> pendingSessions_; //!< First QUuid is the directors ID, second is the Session ID

	QTimer *timeoutTimer_;

};


#endif
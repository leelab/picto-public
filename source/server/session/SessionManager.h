/* \Brief Keeps track of all active sessions on the server
 *
 *	The server is likely to have many active sessions at any given time,
 *	so the session manager is responsible for keeping track of them.  Each
 *	session has an associated QSharedPointer<SessionInfo> which is used to 
 *	keep track of everything related to the session (database, UUID, etc)
 *
 *	Sessions are uniquely identified by a UUID.
 */

#ifndef _SESSION_MANAGER_H_
#define _SESSION_MANAGER_H_

#include "SessionInfo.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QUuid>

class SessionManager : public QObject
{
	Q_OBJECT
public:
	SessionManager();
	QSharedPointer<SessionInfo> createSession();

	QSharedPointer<SessionInfo> getSessionInfo(QUuid uuid);

	//! \TODO create a function for recovering a session after it has timed out

private slots:
	void sessionTimedOut();

private:
	static QMap<QUuid, QSharedPointer<SessionInfo> > openSessions_;
};

#endif
#include "ConnectionManager.h"

#include "ServerConfig.h"

#include <QXmlStreamWriter>
#include <QMutexLocker>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QApplication>
#include <QTimer>

ConnectionManager* ConnectionManager::conMan_ = 0;

//! Private constructor
ConnectionManager::ConnectionManager()
{
	timeoutTimer_ = new QTimer;
	timeoutTimer_->setInterval(3000);
	connect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
	timeoutTimer_->start();

	mutex_ = new QMutex(QMutex::Recursive);
}

/*! \brief Returns the ConnectionManager instance
 *
 *	Since this is a singleton, there is only one ConnectionManager
 *	instance.  This function creates that instance if needed, and then
 *	returns it.
 */
ConnectionManager* ConnectionManager::Instance()
{
	if(!conMan_)
		conMan_ = new ConnectionManager;

	return conMan_;
}

/*!	\brief Checks for timed out connections.
 *
 *	The timeout timer calls this function on a fixed interval.  The
 *	function looks at all of the director instances and sessions and
 *	checks to see if they have timed out.  If they have, then they are
 *	removed from their respective list.
 */
void ConnectionManager::checkForTimeouts()
{
	//First check for director timeouts.  This is done by calling the
	//clearActivity function which clears the activity bool and returns
	//its value.  If there has been no activity since the last time we 
	//checked, the director instance should be removed from our list.
	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<DirectorInfo> directorInfo, directors_)
	{
		if(!directorInfo->clearActivity())
		{
			directors_.remove(directorInfo->uuid_);
			printf("Director timed out!!!!!!!\n");
		}
	}

	//Next check for session timeouts.  These are checked in the same way as
	//the director timeouts
	QMutexLocker locker2(mutex_);
	foreach(QSharedPointer<SessionInfo> sessionInfo, openSessions_)
	{
		if(!sessionInfo->clearActivity())
		{
			openSessions_.remove(sessionInfo->uuid_);
			printf("Session timed out!!!!!\n");
		}
	}
}

/*!	\brief Updates the list of directors with this director
 *
 *	Everytime the DIRECTORUPDATE command is received by the server, this 
 *	function gets called.  The function checks to see if the director instance referenced
 *	in our list, and if not adds it.  If the instance already exists, the function makes
 *	any needed changes.  Note that the Uuid is being used as a primary key here.
 */
void ConnectionManager::updateDirector(QUuid uuid, QHostAddress addr, QString name, DirectorStatus::DirectorStatus status)
{
	QMutexLocker locker(mutex_);

	if(directors_.contains(uuid))
	{
		QSharedPointer<DirectorInfo> info = directors_[uuid];
		info->name_ = name;	
		info->status_ = status;
		info->setActivity();
	}
	else
	{
		QSharedPointer<DirectorInfo> info(new DirectorInfo);

		info->uuid_ = uuid;
		info->addressStr_ = addr.toString();
		info->name_ = name;
		info->status_ = status;

		directors_[uuid] = info;
	}
}

/*!	\brief Updates the list of directors with this director (overloaded)
 *
 *	This version of updateDirector doesn't update the status.  This is important, because 
 *	sometimes (for example during a PUTDATA command) the status isn't available.
 */
void ConnectionManager::updateDirector(QUuid uuid, QHostAddress addr, QString name)
{
	QMutexLocker locker(mutex_);

	if(directors_.contains(uuid))
	{
		QSharedPointer<DirectorInfo> info = directors_[uuid];
		info->name_ = name;	
		info->setActivity();
	}
	else
	{
		QSharedPointer<DirectorInfo> info(new DirectorInfo);

		info->uuid_ = uuid;
		info->addressStr_ = addr.toString();
		info->name_ = name;

		directors_[uuid] = info;
	}
}

//! Removes a director instance from the list
/*void ConnectionManager::removeDirector(QUuid uuid)
{

	QMutexLocker locker(&directorListMutex_);
	directors_.remove(uuid);
}*/

/*! Returns an XML list of active proxy servers
 *
 *	NOTE: It would be reasonable to question the logic of storing the proxy servers
 *	in the configuration database while the Director instances are being stored in memory.
 *	There is in fact no explanation for this, other than the fact that the two components
 *	were developed at compoletely different times, and hence chose different data storage
 *	approaches.  Feel free to modify things...
 *
 *	In an attempt to hide this difference, the getProxyList function has been created here
 *	even though there's no reason not to simply create a ServerConfig object and call it 
 *	directly.
 */
QString ConnectionManager::getProxyList()
{
	ServerConfig config;
	return config.listProxyServers();
}

/*!	\brief Returns a list of director instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *	<DirectorInstances>
 *		<Director>
 *			<Address>192.168.3.51</Address>
 *			<Name>PictoDirector Room 408</Name>
 *			<Status>Idle</Status>
 *		</Director>
 *		<Director>
 *			<Address>192.168.3.164</Address>
 *			<Name>PictoDirector Room 407</Name>
 *			<Status>Running</Status>
 *		</Director>
 *	</DirectorInstances>
 */
QString ConnectionManager::getDirectorList()
{
	QString xmlFragment;
	QXmlStreamWriter xmlWriter(&xmlFragment);

	xmlWriter.writeStartElement("DirectorInstances");

	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<DirectorInfo> director, directors_)
	{
		xmlWriter.writeStartElement("Director");
		xmlWriter.writeTextElement("Address",director->addressStr_);
		xmlWriter.writeTextElement("Id",director->uuid_.toString());
		xmlWriter.writeTextElement("Name", director->name_);
		if(director->status_ == DirectorStatus::idle)
			xmlWriter.writeTextElement("Status","Idle");
		else if(director->status_ == DirectorStatus::stopped)
			xmlWriter.writeTextElement("Status","Stopped");
		else if(director->status_ == DirectorStatus::paused)
			xmlWriter.writeTextElement("Status","Paused");
		else if(director->status_ == DirectorStatus::running)
			xmlWriter.writeTextElement("Status","Running");
		xmlWriter.writeEndElement(); //Director
	}
	xmlWriter.writeEndElement();	//DirectorInstances

	return xmlFragment;
}


//! Returns the status of the Director instance with the passed in Unique User ID
DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatus(QUuid uuid)
{
	QMutexLocker locker(mutex_);
	if(directors_.contains(uuid))
	{
		//checking the status is a form of activity
		directors_[uuid]->setActivity();
		return directors_[uuid]->status_;
	}
	else
	{
		return DirectorStatus::notFound;
	}
}

//! Returns the status of the Director instance with the passed in Unique User ID
DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatus(QString uuidStr)
{
	QUuid uuid(uuidStr);
	return getDirectorStatus(uuid);
}

DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatusBySession(QUuid sessionId)
{
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QUuid directorID = sessInfo->directorUuid_;
		return getDirectorStatus(directorID);
	}
	else
	{
		return DirectorStatus::notFound;
	}

}

//! Sets the director status for the session id
void ConnectionManager::setDirectorStatus(QUuid sessionId, DirectorStatus::DirectorStatus status)
{
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QString directorUuid = sessInfo->directorUuid_;

		directors_[directorUuid]->setActivity();
		directors_[directorUuid]->status_ = status;
	}

}

//! Returns the session info for the session attached to the director with the given ID
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfoByDirector(QUuid directorID)
{
	QMutexLocker locker(mutex_);

	//Iterate through the open sessions looking for the session info with the 
	//matching director ID
	foreach(QSharedPointer<SessionInfo> session, openSessions_)
	{
		if(session->directorUuid_ == directorID)
			return getSessionInfo(session->uuid_);
	}

	//If we made it this far, there is no session with the specified director,
	//so return a null object
	return QSharedPointer<SessionInfo>();
}

//! Returns the session info for the session attached to the director with the given ID
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfoByDirector(QString directorID)
{
	QUuid uuid(directorID);
	return getSessionInfoByDirector(uuid);
}

//! Returns the session info for the passed in session id
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfo(QUuid uuid)
{
	QMutexLocker locker(mutex_);

	if(openSessions_.contains(uuid))
	{
		//anytime we're asking for info about a session, this would
		//indicate activity on the session
		openSessions_[uuid]->setActivity();
		return openSessions_[uuid];
	}
	else
	{
		return QSharedPointer<SessionInfo>();
	}
}
/*! /Brief returns the session UUID if the director needs to start a new session
 *
 *	Since all communication with Director instances is indirect (we have to
 *	wait for an incoming DIRECTOUPDATE command), there is a delay between starting
 *	session and telling the director what it's session ID is.  This function
 *	is used by the DIRECTORUPDATE command handler to figure out if the director
 *	instance needs to start a new session.
 */
QUuid ConnectionManager::pendingSession(QUuid directorID)
{
	if(pendingSessions_.contains(directorID))
	{
		QUuid sessionId = pendingSessions_[directorID];
		pendingSessions_.remove(directorID);
		return sessionId;
	}
	else
	{
		return QUuid();
	}
}

//! Creates a new session and returns a pointer to the SessinoInfo object
QSharedPointer<SessionInfo> ConnectionManager::createSession(QUuid directorID, int proxyId, QByteArray experimentXml, QUuid initialObserverId)
{
	QMutexLocker locker(mutex_);
	QString directorAddr;
	if(directors_.contains(directorID))
		directorAddr = directors_[directorID]->addressStr_;
	else
		return QSharedPointer<SessionInfo>();

	QSharedPointer<SessionInfo> sessInfo(new SessionInfo(directorID, directorAddr, proxyId, experimentXml,initialObserverId));

	pendingSessions_[directorID] = sessInfo->uuid_;
	openSessions_[sessInfo->uuid_] = sessInfo;

	return sessInfo;
}

//!Ends a currently running session
void ConnectionManager::endSession(QUuid sessionId)
{
	openSessions_[sessionId]->endSession();
	openSessions_.remove(sessionId);
}
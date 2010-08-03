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
			directors_.remove(directorInfo->addressStr_);
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
 *	any needed changes.  Note that the IP address is being used as a primary key here.
 */
void ConnectionManager::updateDirector(QHostAddress addr, QString name, DirectorStatus::DirectorStatus status)
{
	QMutexLocker locker(mutex_);

	if(directors_.contains(addr.toString()))
	{
		QSharedPointer<DirectorInfo> info = directors_[addr.toString()];
		info->name_ = name;	
		info->status_ = status;
		info->setActivity();
	}
	else
	{
		QSharedPointer<DirectorInfo> info(new DirectorInfo);

		info->addressStr_ = addr.toString();
		info->name_ = name;
		info->status_ = status;

		directors_[addr.toString()] = info;
	}
}

/*!	\brief Updates the list of directors with this director (overloaded)
 *
 *	This version of updateDirector doesn't update the status.  This is important, because 
 *	sometimes (for example during a PUTDATA command) the status isn't available.
 */
void ConnectionManager::updateDirector(QHostAddress addr, QString name)
{
	QMutexLocker locker(mutex_);

	if(directors_.contains(addr.toString()))
	{
		QSharedPointer<DirectorInfo> info = directors_[addr.toString()];
		info->name_ = name;	
		info->setActivity();
	}
	else
	{
		QSharedPointer<DirectorInfo> info(new DirectorInfo);

		info->addressStr_ = addr.toString();
		info->name_ = name;

		directors_[addr.toString()] = info;
	}
}

//! Removes a director instance from the list
/*void ConnectionManager::removeDirector(QHostAddress addr)
{

	QMutexLocker locker(&directorListMutex_);
	directors_.remove(addr.toString());
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


//! Returns the status of the Director instance with the passed in address
DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatus(QHostAddress addr)
{
	QString addrStr = addr.toString();
	return getDirectorStatus(addrStr);
}

//! Returns the status of the Director instance with the passed in address
DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatus(QString addr)
{
	QMutexLocker locker(mutex_);
	if(directors_.contains(addr))
	{
		//checking the status is a form of activity
		directors_[addr]->setActivity();
		return directors_[addr]->status_;
	}
	else
	{
		return DirectorStatus::notFound;
	}
}

DirectorStatus::DirectorStatus ConnectionManager::getDirectorStatus(QUuid sessionId)
{
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QString directorAddr = sessInfo->directorAddr_;
		return getDirectorStatus(directorAddr);
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
		QString directorAddr = sessInfo->directorAddr_;

		directors_[directorAddr]->setActivity();
		directors_[directorAddr]->status_ = status;
	}

}

//! Returns the session info for the session attached to the director with the given address
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfo(QString directorAddr)
{
	QMutexLocker locker(mutex_);

	//Iterate through the open sessions looking for the session info with the 
	//matching address
	foreach(QSharedPointer<SessionInfo> session, openSessions_)
	{
		if(session->directorAddr_ == directorAddr)
			return getSessionInfo(session->uuid_);
	}

	//If we made it this far, there is no session with the specified director,
	//so return a null object
	return QSharedPointer<SessionInfo>();
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
/*! /Brief returns the UUID if the director needs to start a new session
 *
 *	Since all communication with Director instances is indirect (we have to
 *	wait for an incoming DIRECTOUPDATE command), there is a delay between starting
 *	session and telling the director what it's session ID is.  This function
 *	is used by the DIRECTORUPDATE command handler to figure out if the director
 *	instance needs to start a new session.
 */
QUuid ConnectionManager::pendingSession(QHostAddress directorAddr)
{
	if(pendingSessions_.contains(directorAddr.toString()))
	{
		QUuid sessionId = pendingSessions_[directorAddr.toString()];
		pendingSessions_.remove(directorAddr.toString());
		return sessionId;
	}
	else
	{
		return QUuid();
	}
}

//! Creates a new session and returns a pointer to the SessinoInfo object
QSharedPointer<SessionInfo> ConnectionManager::createSession(QString directorAddr, int proxyId, QByteArray experimentXml, QUuid initialObserverId)
{
	QMutexLocker locker(mutex_);
	
	QSharedPointer<SessionInfo> sessInfo(new SessionInfo(directorAddr, proxyId, experimentXml,initialObserverId));

	pendingSessions_[directorAddr] = sessInfo->uuid_;
	openSessions_[sessInfo->uuid_] = sessInfo;

	return sessInfo;
}

//!Ends a currently running session
void ConnectionManager::endSession(QUuid sessionId)
{
	openSessions_[sessionId]->endSession();
	openSessions_.remove(sessionId);
}
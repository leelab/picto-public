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
#include <QtConcurrentRun>
#include "ServerConfig.h"
#include "../../common/memleakdetect.h"

#define DROPPED_SESSION_TIMEOUT_DAYS 1
ConnectionManager* ConnectionManager::conMan_ = 0;

/*! \brief Private constructor
 *	\details Starts a timeout timer that is used to flush cached data to disk and
 *	check to see if any components have stopped talking to the server.
 */
ConnectionManager::ConnectionManager()
{
	ServerConfig serverConfig;

	timeoutTimer_ = new QTimer;
	timeoutTimer_->setInterval(5000);
	connect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
	timeoutTimer_->start();

	mutex_ = new QMutex(QMutex::Recursive);
}

/*! \brief Returns the ConnectionManager instance
 *
 *	\details Since this is a singleton, there is only one ConnectionManager
 *	instance.  This function creates that instance if needed, and then
 *	returns it.
 */
ConnectionManager* ConnectionManager::Instance()
{
	if(!conMan_)
		conMan_ = new ConnectionManager;

	return conMan_;
}

ConnectionManager::~ConnectionManager()
{
	delete timeoutTimer_;
	delete mutex_;
	//This should probably also set conMan_ to NULL.
}

/*!	\brief Checks for timed out connections and flushes cached session data to disk.
 *
 *	\details The timeout timer calls this function on a fixed interval.  Whenever it is
 *	called, first check for session timeouts.  This is done by calling the 
 *	clearActivity function which clears the activity bool and returns
 *	its value.  If there has been no activity since the last time we
 *	checked, check if any components are still active in this session
 *	(ie. Still providing data).  If they are, the session instance should 
 *	be removed from our list and kept in the opensessions list in case we 
 *	just lost a connection with one of the components and it is going to try to reconnect.
 *	If all components are no longer active, we should end the session.
 *	
 *	If there has been activity on the session, we should set all of
 *	that sessions components active, since we don't want any of them timing
 *	out if there are any that haven't timed out.  This is also where we initiate 
 *	a flush of cached data to disk.  The actual SessionInfo::flushCache() is
 *	performed in a separate thread though so as to not block this function.
 */
void ConnectionManager::checkForTimeouts()
{
	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<SessionInfo> sessionInfo, openSessions_)
	{
		if(!sessionInfo->clearActivity())
		{
			// Check if anything is still active in this session.
			sessionInfo->UpdateComponentActivity();
			if(sessionInfo->hasActiveComponents())
			{
				//Some components may still be running this session.  Time it out, but keep it available.
				sessionInfo->flushCache();
				ServerConfig serverConfig;
				serverConfig.setActivity(sessionInfo->uuid_.toString(),false);
				openSessions_.remove(sessionInfo->uuid_);
				printf("Session timed out!!!!!\n");
			}
			else
			{
				//Since endSession may take a long time and we don't want to create thread safety issues.  Whenever we end
				//a session, we just unlock the mutex first and leave the function when we're done.  The other sessions will
				//miss one round of timeout checking.  But that doesn't make any significant difference.
				locker.unlock();
				endSession(sessionInfo->sessionId());
				return;
			}
		}
		else
		{
			QSharedPointer<ComponentInfo> component = sessionInfo->getComponentByType("DIRECTOR");
			if(!component.isNull())
			{
				component->setActivity();
				//sessionInfo->enableFlush("DIRECTOR");
			}
			component = sessionInfo->getComponentByType("PROXY");
			if(!component.isNull())
			{
				component->setActivity();
				//sessionInfo->enableFlush("PROXY");
			}
			//sessionInfo->flushCache();
			QFuture<bool> future = QtConcurrent::run(sessionInfo.data(),&SessionInfo::flushCache,QString(""));
		}
	}

	//Now check for component timeouts.
	foreach(QSharedPointer<ComponentInfo> componentInfo, components_)
	{
		if(!componentInfo->clearActivity())
		{
			componentInfo->setStatus(ComponentStatus::notFound);
			components_.remove(componentInfo->getUuid());
			printf(componentInfo->getType().append("timed out!!!!!!!\n").toLatin1());
		}
	}
}

/*!	\brief Updates the list of connected Components with the input Component information
 *
 *	\details Everytime the COMPONENTUPDATE command is received by the server, this 
 *	function gets called.  The function checks to see if the component instance referenced
 *	is in our list, and if not adds it.  If the instance already exists, the function makes
 *	any needed changes.  This function also calls setActivity() for the component
 *	to indicate that communication with that component is active.
 *	\note The component's Uuid is being used as a primary key here.
 */
void ConnectionManager::updateComponent(QUuid uuid, QHostAddress addr, QUuid sessionId, QString name, QString type, ComponentStatus::ComponentStatus status, QString details)
{
	QMutexLocker locker(mutex_);
	QSharedPointer<ComponentInfo> info;
	if(components_.contains(uuid))
	{
		info = components_[uuid];
	}
	else
	{
		// Maybe its running from a session that was dropped.  The call below will check and add that session back to the ilst if necessary.
		QSharedPointer<SessionInfo> sessInfo = getSessionInfo(sessionId);	
		if(!sessInfo.isNull())
		{
			info = sessInfo->getComponentByType(type);
		}
		else
		{
			//If there's any session that thinks this component is attached to it, it's not anymore.
			//Load that session and associate this component with it, so that it can see that this
			//component has moved on.  That way, when it times out, if its other components have also
			//moved on, it will know that it has to end.
			sessInfo = getSessionInfoByComponent(uuid);
			if(!sessInfo.isNull())
			{
				info = sessInfo->getComponentByType(type);
			}
			else
			{
				//Create a new component.
				info = QSharedPointer<ComponentInfo>(new ComponentInfo);
			}
		}

		info->setUuid(uuid);
		info->setAddress(addr.toString());
		components_[uuid] = info;
	}
	info->setName(name);	
	info->setStatus(status);
	if(!details.isEmpty())
		info->setDetails(details);
	info->setType(type);
	info->setSessionID(sessionId);
	info->setActivity();
	QSharedPointer<SessionInfo> sessInfo = getSessionInfo(sessionId);	
	if(!sessInfo.isNull())
		sessInfo->setActivity();
}

/*!	\brief Returns a list of proxy instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *	\code
 	<ProxyInstances>
 		<Proxy>
 			<Address>192.168.3.51</Address>
 			<Id>{550e8400-e29b-41d4-a716-446655440000}</Id>
 			<Name>Proxy Room 408</Name>
 			<Status>Idle</Status>
 		</Proxy>
 		<Proxy>
 			<Address>192.168.3.164</Address>
 			 <Id>{5650e8400-e29b-41d4-a716-446655340000}</Id>
 			<Name>Proxy Room 407</Name>
 			<Status>Running</Status>
 		</Proxy>
 	</ProxyInstances>
	\endcode
 */
QString ConnectionManager::getProxyList()
{
	QString xmlFragment;
	QXmlStreamWriter xmlWriter(&xmlFragment);

	xmlWriter.writeStartElement("ProxyInstances");

	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<ComponentInfo> component, components_)
	{
		if(component->getType() != "PROXY")
			continue;
		xmlWriter.writeStartElement("Proxy");
		xmlWriter.writeTextElement("Address",component->getAddress());
		xmlWriter.writeTextElement("Id",component->getUuid().toString());
		xmlWriter.writeTextElement("Name", component->getName());
		if(component->getStatus() == ComponentStatus::idle)
			xmlWriter.writeTextElement("Status","Idle");
		else if(component->getStatus() == ComponentStatus::ending)
			xmlWriter.writeTextElement("Status","Ending");
		else if(component->getStatus() == ComponentStatus::stopped)
			xmlWriter.writeTextElement("Status","Stopped");
		else if(component->getStatus() == ComponentStatus::paused)
			xmlWriter.writeTextElement("Status","Paused");
		else if(component->getStatus() == ComponentStatus::running)
			xmlWriter.writeTextElement("Status","Running");
		else
			xmlWriter.writeTextElement("Status","NotFound");
		xmlWriter.writeEndElement(); //Proxy
	}
	xmlWriter.writeEndElement();	//ProxyInstances

	return xmlFragment;
}

/*!	\brief Returns a list of director instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *  \code
 	<DirectorInstances>
 		<Director>
 			<Address>192.168.3.51</Address>
			<Id>{550e8400-e29b-41d4-a716-446655440000}</Id>
 			<Name>PictoDirector Room 408</Name>
 			<Status>Idle</Status>
			<Session-ID>{12345600-e29b-41d4-a716-446655440000}</Session-ID>
			<Details></Details>
 		</Director>
 		<Director>
 			<Address>192.168.3.164</Address>
			<Id>{950e8400-e29b-41d4-a716-446655440000}</Id>
 			<Name>PictoDirector Room 407</Name>
 			<Status>Running</Status>
			<Session-ID>{65432100-e29b-41d4-a716-446655440000}</Session-ID>
			<Details></Details>
 		</Director>
 	</DirectorInstances>
	\endcode
 */
QString ConnectionManager::getDirectorList()
{
	QString xmlFragment;
	QXmlStreamWriter xmlWriter(&xmlFragment);

	xmlWriter.writeStartElement("DirectorInstances");

	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<ComponentInfo> component, components_)
	{
		if(component->getType() != "DIRECTOR")
			continue;
		xmlWriter.writeStartElement("Director");
		xmlWriter.writeTextElement("Address",component->getAddress());
		xmlWriter.writeTextElement("Id",component->getUuid().toString());
		xmlWriter.writeTextElement("Name", component->getName());
		if(component->getStatus() == ComponentStatus::idle)
			xmlWriter.writeTextElement("Status","Idle");
		else if(component->getStatus() == ComponentStatus::ending)
			xmlWriter.writeTextElement("Status","Ending");
		else if(component->getStatus() == ComponentStatus::stopped)
			xmlWriter.writeTextElement("Status","Stopped");
		else if(component->getStatus() == ComponentStatus::paused)
			xmlWriter.writeTextElement("Status","Paused");
		else if(component->getStatus() == ComponentStatus::running)
			xmlWriter.writeTextElement("Status","Running");
		else
			xmlWriter.writeTextElement("Status","NotFound");

		xmlWriter.writeTextElement("Session-ID",component->getSessionID().toString());
		xmlWriter.writeTextElement("Details",component->getDetails());
		xmlWriter.writeEndElement(); //Director
	}
	xmlWriter.writeEndElement();	//DirectorInstances

	return xmlFragment;
}


/*! \brief Returns the status of the component instance with the passed in Unique User ID. */
ComponentStatus::ComponentStatus ConnectionManager::getComponentStatus(QUuid uuid)
{
	QMutexLocker locker(mutex_);
	if(components_.contains(uuid))
	{
		//checking the status is a form of activity
		components_[uuid]->setActivity();
		return components_[uuid]->getStatus();
	}
	else
	{
		return ComponentStatus::notFound;
	}
}

/*! \brief Returns the status of the component instance with the passed in Unique User ID String. */
ComponentStatus::ComponentStatus ConnectionManager::getComponentStatus(QString uuidStr)
{
	QUuid uuid(uuidStr);
	return getComponentStatus(uuid);
}

/*! \brief Returns the status of the component instance with the passed in componentType that is 
 *	part of the session with the passed in sessionId. 
 */
ComponentStatus::ComponentStatus ConnectionManager::getComponentStatusBySession(QUuid sessionId, QString componentType)
{
	QMutexLocker locker(mutex_);
	if(sessionIsValid(sessionId.toString()))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QSharedPointer<ComponentInfo> component = sessInfo->getComponentByType(componentType);
		if(!component.isNull())
			return component->getStatus();
	}
	return ComponentStatus::notFound;
}

/*! \brief Sets the component status for the component instance of the input type that is running in the session with the
 *	input session id.
 *	\details This function also calls setActivity() on the component to indicate that the connection to that component is 
 *	active.
 */
void ConnectionManager::setComponentStatus(QUuid sessionId, QString componentType, ComponentStatus::ComponentStatus status)
{
	QMutexLocker locker(mutex_);
	if(sessionIsValid(sessionId.toString()))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QSharedPointer<ComponentInfo> component = sessInfo->getComponentByType(componentType);
		if(!component.isNull())
		{
			component->setActivity();
			component->setStatus(status);
		}
	}

}

/*! \brief Returns the sessionID associated with the input ComponentID
 *  \details Unlike getSessionInfoByComponent(), this function looks up the sessionID
 *  That the component thinks its associated with rather than looking through
 *	all sessions to see if any session is associated with the input component.
 *	There is a distinction because in the case of sessions that were run before
 *	and have timed out, there may be more than one session associated with a given
 *	component, but the component itself will only ever report that it is associated
 *	with the session that it is running.
 */
QUuid ConnectionManager::GetComponentsSessionId(QUuid componentId)
{
	QMutexLocker locker(mutex_);
	if(components_.contains(componentId))
	{
		//checking the components SessionID is a form of activity
		components_[componentId]->setActivity();
		return components_[componentId]->getSessionID();
	}
	return QUuid();
}

/*! \brief Returns the sessionID associated with the input ComponentID string.
 *  \details Unlike getSessionInfoByComponent(), this function looks up the sessionID
 *  That the component thinks its associated with rather than looking through
 *	all sessions to see if any session is associated with the input component.
 *	There is a distinction because in the case of sessions that were run before
 *	and have timed out, there may be more than one session associated with a given
 *	component, but the component itself will only ever report that it is associated
 *	with the session that it is running.
 */
QUuid ConnectionManager::GetComponentsSessionId(QString componentId)
{
	return GetComponentsSessionId(QUuid(componentId));
}

/*! \brief Returns the session info for the session attached to the component with the input component ID
 *	\details This looks through all open sessions for the one that includes the component with the input
 *	componentID.  If an open session is not found, old timed out sessions are checked as well.  If more
 *	than one timed out session was associated with the component, a randomly selected one will be chosen.
 */
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfoByComponent(QUuid componentID)
{
	QMutexLocker locker(mutex_);

	
	//Iterate through the open sessions looking for the session info with the 
	//matching component ID
	foreach(QSharedPointer<SessionInfo> session, openSessions_)
	{
		if(session->hasComponent(componentID))
			return getSessionInfo(session->uuid_);
	}

	//Maybe this component was in a dropped session.
	ServerConfig serverConfig;
	QString sessionID = serverConfig.getSessionPathByComponent(componentID.toString());
	if(sessionIsValid(sessionID))//This will load the session for us
		return getSessionInfo(sessionID);
	return QSharedPointer<SessionInfo>(); // If we got here then it doesn't exist
}

/*! \brief Returns the session info for the session attached to the component with the input component ID string
 *	\details This looks through all open sessions for the one that includes the component with the input
 *	componentID.  If an open session is not found, old timed out sessions are checked as well.  If more
 *	than one timed out session was associated with the component, a randomly selected one will be chosen.
 */
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfoByComponent(QString componentID)
{
	QUuid uuid(componentID);
	return getSessionInfoByComponent(uuid);
}

/*! \brief Returns the session info for the open session with the passed in session id.  If no such session
 * is open, returns an empty pointer.
 */
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfo(QUuid uuid)
{
	QMutexLocker locker(mutex_);

	if(sessionIsValid(uuid.toString()))
	{
		return openSessions_[uuid];
	}
	else
	{
		return QSharedPointer<SessionInfo>();
	}
}
/*! /Brief Returns the session UUID if the component with the input componentId needs to join a new session
 *
 *	\details Since all communication with proxy and director component instances is indirect (we have to
 *	wait for an incoming COMPONENTUPDATE command), there is a delay between starting
 *	session and telling the component what it's session ID is.  This function
 *	is used by the COMPONENTUPDATE command handler to figure out if the component
 *	instance needs to start a new session.
 */
QUuid ConnectionManager::pendingSession(QUuid componentID)
{
	if(pendingSessions_.contains(componentID))
	{
		QUuid sessionId = pendingSessions_[componentID];
		pendingSessions_.remove(componentID);
		return sessionId;
	}
	else
	{
		return QUuid();
	}
}

/*! \brief Creates a new session and returns a pointer to the SessionInfo object.
 *	@param directorID The Uuid of the Director to be used in the session.
 *	@param proxyID The Uuid of the Proxy to be used in the session.
 *	@param designName The name of the design that will be used in the Session.
 *	@param designXml The contents of the Design file that will be used in the Session.
 *	@param DesignConfig The serialized DesignConfig object that manages the Design used in this session.  This contains 
 *		information for identifying Assets from the AssetIds and is useful for building Element/Transition/etc lookup 
 *		tables in the session file.
 *	@param initialObserverId The Uuid of the workstation that is starting this Session.  It will be considered an Authorized
 *		observer (ie. It is authorized to make state changes to the Session).
 *	@param password	The password that gives workstations authorization to change the Session state.
 *	\note When DEVELOPMENTBUILD is not defined, password may not be empty.
 */
QSharedPointer<SessionInfo> ConnectionManager::createSession(QUuid directorID, QUuid proxyID, QString designName, QByteArray designXml, QByteArray DesignConfig, QUuid initialObserverId, QString password)
{
	QMutexLocker locker(mutex_);

	if(!components_.contains(directorID))
		return QSharedPointer<SessionInfo>();

	if((proxyID != QUuid()) && !components_.contains(proxyID))
		return QSharedPointer<SessionInfo>();

#ifndef DEVELOPMENTBUILD
	if(password.isEmpty())
		return QSharedPointer<SessionInfo>();
#endif

	QString directorName = components_[directorID]->getName();
	QSharedPointer<SessionInfo> sessInfo(SessionInfo::CreateSession(designName, directorName, designXml,DesignConfig,initialObserverId, password));
	sessInfo->AddComponent(components_[directorID]);
	if(proxyID != QUuid())
	{
		sessInfo->AddComponent(components_[proxyID]);
		sessInfo->alignTimestampsTo(components_[directorID]->getType());
	}

	pendingSessions_[directorID] = sessInfo->uuid_;
	if(proxyID != QUuid())
		pendingSessions_[proxyID] = sessInfo->uuid_;
	openSessions_[sessInfo->uuid_] = sessInfo;
	
	//Add session data to serverConfig database
	ServerConfig serverConfig;
	serverConfig.addSession(sessInfo->sessionId().toString(),sessInfo->dataBaseFilePath(),
		directorID.toString(),proxyID.toString());

	//Whenever we create a new session, we check for timeouts of previously dropped sessions.
	checkForDroppedSessionTimeouts();

	return sessInfo;
}

/*! \brief Load's a previously timed out Session with the input sessionId from the file at filePath.  
 *	If the file does not exist, the session is removed from the input ServerConfig object.
 */
QSharedPointer<SessionInfo> ConnectionManager::loadSession(QString sessionId, QString filePath)
{
	QSharedPointer<SessionInfo> sessInfo;
	//Check if the file exists in the file path.  If not it was deleted and we should not 
	//attempt to load the session because weird things will happen.  In that case we simply
	//remove it from the ServerConfig database
	if(!QFile::exists(filePath))
	{
		ServerConfig serverConfig;
		serverConfig.removeSession(sessionId);
		return sessInfo;
	}
	sessInfo = SessionInfo::LoadSession(sessionId,filePath);
	if(!sessInfo)
		return sessInfo;
	QMutexLocker locker(mutex_);
	openSessions_[sessInfo->uuid_] = sessInfo;
	return sessInfo;
}

/*! \brief Checks for sessions that were dropped and timed out a long time ago and removes them from the ServerConfig 
 *	object
 *	\details The maximum time that a dropped session sticks around before being removed is set in 
 *	DROPPED_SESSION_TIMEOUT_DAYS.
 */
void ConnectionManager::checkForDroppedSessionTimeouts()
{
	QMutexLocker locker(mutex_);
	ServerConfig config;
	QStringList runningSessions = config.getRunningSessions();
	foreach(QString session, runningSessions)
	{
		if(!openSessions_.contains(QUuid(session)))
		{// It says its open but its not, there must have been a server crash.  Set it to idle.
			config.setActivity(session,false);
		}
	}
	QDateTime timeoutTime = QDateTime::currentDateTime().addDays(-DROPPED_SESSION_TIMEOUT_DAYS);
	QStringList timedOut = config.getSessionsIdledBefore(timeoutTime);
	foreach(QString session, timedOut)
	{
		if(sessionIsValid(session)) //This will load the session for us.
		{
			//This tells the session to report that its components are no longer active.
			//That means that it will be ended as soon as it times out.
			getSessionInfo(QUuid(session))->ignoreComponents();
		}
	}
}

/*! \brief Returns true if a session with the input sessionId is open or previously timed out on
 *	this Server.
 */
bool ConnectionManager::sessionIsValid(QString sessionId)
{
	if((sessionId == "") || (QUuid(sessionId) == QUuid()))
		return false;
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
		return true;
	
	// Maybe it was a dropped session.  Check serverConfig.
	ServerConfig serverConfig;
	QString sessionPath = serverConfig.getSessionPathByID(sessionId);
	if(sessionPath != "")
	{
		locker.unlock();
		QSharedPointer<SessionInfo> sessInfo = loadSession(sessionId,sessionPath);		
		if(!sessInfo.isNull())
			return true;
	}
	return false;
}

/*!	\brief Ends a currently running session.
 *	\sa SessionInfo::endSession()
 */
bool ConnectionManager::endSession(QUuid sessionId)
{
	if(sessionIsValid(sessionId.toString()) && openSessions_[sessionId]->endSession())
	{
		QMutexLocker locker(mutex_);
		openSessions_.remove(sessionId);
		ServerConfig serverConfig;
		serverConfig.removeSession(sessionId.toString());
		return true;
	}
	return false;
}
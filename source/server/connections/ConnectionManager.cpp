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
 *	function looks at all of the director and proxy instances and sessions and
 *	checks to see if they have timed out.  If they have, then they are
 *	removed from their respective list.
 */
void ConnectionManager::checkForTimeouts()
{
	//First check for component timeouts.  This is done by calling the
	//clearActivity function which clears the activity bool and returns
	//its value.  If there has been no activity since the last time we 
	//checked, the component instance should be removed from our list.
	QMutexLocker locker(mutex_);
	foreach(QSharedPointer<ComponentInfo> componentInfo, components_)
	{
		if(!componentInfo->clearActivity())
		{
			components_.remove(componentInfo->getUuid());
			printf("Director timed out!!!!!!!\n");
		}
	}

	//Next check for session timeouts.
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
 *	Everytime the COMPONENTUPDATE command is received by the server, this 
 *	function gets called.  The function checks to see if the component instance referenced
 *	isn in our list, and if not adds it.  If the instance already exists, the function makes
 *	any needed changes.  Note that the Uuid is being used as a primary key here.
 */
void ConnectionManager::updateComponent(QUuid uuid, QHostAddress addr, QString name, QString type, ComponentStatus::ComponentStatus status)
{
	QMutexLocker locker(mutex_);

	if(components_.contains(uuid))
	{
		QSharedPointer<ComponentInfo> info = components_[uuid];
		info->setName(name);	
		info->setStatus(status);
		info->setType(type);
		info->setActivity();
	}
	else
	{
		QSharedPointer<ComponentInfo> info(new ComponentInfo);

		info->setUuid(uuid);
		info->setAddress(addr.toString());
		info->setName(name);
		info->setStatus(status);
		info->setType(type);
		components_[uuid] = info;
	}
}

/*!	\brief Updates the list of components with this component(overloaded)
 *
 *	This version of updateComponent doesn't update the status.  This is important, because 
 *	sometimes (for example during a PUTDATA command) the status isn't available.
 */
void ConnectionManager::updateComponent(QUuid uuid, QHostAddress addr, QString name, QString type)
{
	QMutexLocker locker(mutex_);

	if(components_.contains(uuid))
	{
		QSharedPointer<ComponentInfo> info = components_[uuid];
		info->setName(name);
		info->setType(type);
		info->setActivity();
	}
	else
	{
		QSharedPointer<ComponentInfo> info(new ComponentInfo);

		info->setUuid(uuid);
		info->setAddress(addr.toString());
		info->setName(name);
		info->setType(type);

		components_[uuid] = info;
	}
}

/*!	\brief Returns a list of proxy instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *	<ProxyInstances>
 *		<Proxy>
 *			<Address>192.168.3.51</Address>
 *			<Name>Proxy Room 408</Name>
 *			<Status>Idle</Status>
 *		</Proxy>
 *		<Proxy>
 *			<Address>192.168.3.164</Address>
 *			<Name>Proxy Room 407</Name>
 *			<Status>Running</Status>
 *		</Proxy>
 *	</ProxyInstances>
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
		else if(component->getStatus() == ComponentStatus::stopped)
			xmlWriter.writeTextElement("Status","Stopped");
		else if(component->getStatus() == ComponentStatus::paused)
			xmlWriter.writeTextElement("Status","Paused");
		else if(component->getStatus() == ComponentStatus::running)
			xmlWriter.writeTextElement("Status","Running");
		xmlWriter.writeEndElement(); //Proxy
	}
	xmlWriter.writeEndElement();	//ProxyInstances

	return xmlFragment;
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
	foreach(QSharedPointer<ComponentInfo> component, components_)
	{
		if(component->getType() != "DIRECTOR")
			continue;
		xmlWriter.writeStartElement("Director");
		xmlWriter.writeTextElement("Address",component->getAddress());
		xmlWriter.writeTextElement("Id",component->getUuid());
		xmlWriter.writeTextElement("Name", component->getName());
		if(component->getStatus() == ComponentStatus::idle)
			xmlWriter.writeTextElement("Status","Idle");
		else if(component->getStatus() == ComponentStatus::stopped)
			xmlWriter.writeTextElement("Status","Stopped");
		else if(component->getStatus() == ComponentStatus::paused)
			xmlWriter.writeTextElement("Status","Paused");
		else if(component->getStatus() == ComponentStatus::running)
			xmlWriter.writeTextElement("Status","Running");
		xmlWriter.writeEndElement(); //Director
	}
	xmlWriter.writeEndElement();	//DirectorInstances

	return xmlFragment;
}


//! Returns the status of the component instance with the passed in Unique User ID
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

//! Returns the status of the Director instance with the passed in Unique User ID
ComponentStatus::ComponentStatus ConnectionManager::getComponentStatus(QString uuidStr)
{
	QUuid uuid(uuidStr);
	return getComponentStatus(uuid);
}

ComponentStatus::ComponentStatus ConnectionManager::getComponentStatusBySession(QUuid sessionId, QString componentType)
{
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
	{
		QSharedPointer<SessionInfo> sessInfo = openSessions_.value(sessionId);
		QSharedPointer<ComponentInfo> component = sessInfo->getComponentByType(componentType);
		if(!component.isNull())
			return component->getStatus();
	}
	return ComponentStatus::notFound;
}

//! Sets the component status for the session id
void ConnectionManager::setComponentStatus(QUuid sessionId, QString componentType, ComponentStatus::ComponentStatus status)
{
	QMutexLocker locker(mutex_);
	if(openSessions_.contains(sessionId))
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

//! Returns the session info for the session attached to the component with the given ID
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
	
	//If we made it this far, there is no session with the specified director,
	//so return a null object
	return QSharedPointer<SessionInfo>();
}

//! Returns the session info for the session attached to the component with the given ID
QSharedPointer<SessionInfo> ConnectionManager::getSessionInfoByComponent(QString componentID)
{
	QUuid uuid(componentID);
	return getSessionInfoByComponent(uuid);
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
/*! /Brief returns the session UUID if the component needs to start a new session
 *
 *	Since all communication with proxy and director component instances is indirect (we have to
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

//! Creates a new session and returns a pointer to the SessinoInfo object
QSharedPointer<SessionInfo> ConnectionManager::createSession(QUuid directorID, QUuid proxyID, QByteArray experimentXml, QUuid initialObserverId)
{
	QMutexLocker locker(mutex_);

	if(!components_.contains(directorID))
		return QSharedPointer<SessionInfo>();

	if((proxyID != QUuid()) && !components_.contains(proxyID))
		return QSharedPointer<SessionInfo>();

	QSharedPointer<SessionInfo> sessInfo(new SessionInfo(experimentXml,initialObserverId));
	sessInfo->AddComponent(components_[directorID]);
	if(proxyID != QUuid())
		sessInfo->AddComponent(components_[proxyID]);
	sessInfo->alignTimestampsTo(components_[directorID]->getType());

	pendingSessions_[directorID] = sessInfo->uuid_;
	if(proxyID != QUuid())
		pendingSessions_[proxyID] = sessInfo->uuid_;
	openSessions_[sessInfo->uuid_] = sessInfo;

	return sessInfo;
}

//!Ends a currently running session
void ConnectionManager::endSession(QUuid sessionId)
{
	openSessions_[sessionId]->endSession();
	openSessions_.remove(sessionId);
}
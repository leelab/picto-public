#ifndef COMPONENTSTATUSMANAGER_H
#define COMPONENTSTATUSMANAGER_H

#include <QMap>
#include <QString>
#include <QDateTime>
#include <QUuid>
#include <QObject>
#include "../common.h"

enum ComponentStatus
{
	disconnected=0,idle, ending, stopped, paused, running
};

/*! \brief Handles overall status operations for a ComponentInterface object.
 *	\details All ComponentInterface objects support the various ComponentStatus states.  They each work within a session framework and need to
 *	maintain a Session ID, they need to be able to provide various pieces of status information and recognize if there has been a request
 *	to exit the ComponentInterface's application.  This object handles all of those types of activities.  It should be inherited for each
 *	ComponentInterface type to perform operations specific to that type of ComponentInterface.  For example, a Director ComponentInterface
 *	will print status information on the bottom left corner of the screen, whereas the Proxy might print it somewhere within its application
 *	window.
 *	\sa ComponentInterface
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentStatusManager : public QObject
#else
class ComponentStatusManager
#endif
{
	Q_OBJECT
public:
	ComponentStatusManager();
	/*! \brief Returns a name representing the ComponentInterface handled by this ComponentStatusManager.
	*/
	virtual QString getName()=0;
	virtual void setStatus(ComponentStatus status);
	void setStatus(QString status);
	/*! \brief Sets a string contain user information that may optionally be displayed to the user in child class implementations.
	*/
	virtual void setUserInfo(QString info){};
	/*! \brief Sets the Session ID of the session currently being run by the ComponentInterface handled by this object.
	 *	\details This should be called whenever a new session is set up.  Internally, if the Session ID changes, this function
	 *	calls newSession().
	 */
	void setSessionID(QUuid sessionID){if(sessionID_ != sessionID){ newSession(); sessionID_ = sessionID;}};
	/*! \brief Returns ths Session ID of the ComponentInterface's current session.*/
	QUuid getSessionID(){return sessionID_;};
	void update(int timeoutMs);
	ComponentStatus getStatus();
	QString getStatusAsString();
	/*! \brief Returns true if forceExit() has been called.*/
	bool exitTriggered(){return forceExit_;};
public slots:
	void forceExit();
protected:
	/*! \brief Handles initialization operations when a new session is started.*/
	virtual void newSession() = 0;
	//May be updated by child to perform operations that must occur very frequently
	//when the component is not running a session.  This will get called about once per
	//16ms when the component is not in the running or paused states.
	virtual void doFrequentUpdate(){};
	//Should be updated by child to send a "keep alive" update to the server.
	//This will get called about once per second.
	virtual void doServerUpdate() = 0;
private:
	ComponentStatus status_;
	static QMap<QString,ComponentStatus> statusTransitionDictionary_;
	static QMap<ComponentStatus,QString> statusNameDictionary_;
	QDateTime lastUpdateTime_;
	QDateTime lastFreqUpdateTime_;
	QUuid sessionID_;
	bool forceExit_;
};
#endif
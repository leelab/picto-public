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

#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentStatusManager : public QObject
#else
class ComponentStatusManager
#endif
{
	Q_OBJECT
public:
	ComponentStatusManager();
	virtual QString getName()=0;
	virtual void setStatus(ComponentStatus status);
	void setStatus(QString status);
	virtual void setUserInfo(QString info){};
	void setSessionID(QUuid sessionID){if(sessionID_ != sessionID){ newSession(); sessionID_ = sessionID;}};
	QUuid getSessionID(){return sessionID_;};
	void update(int timeoutMs);
	ComponentStatus getStatus();
	QString getStatusAsString();
	bool exitTriggered(){return forceExit_;};
public slots:
	void forceExit();
protected:
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
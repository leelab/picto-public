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
	void setName(QString name){name_ = name;};
	QString getName(){return name_;};
	virtual void setStatus(ComponentStatus status);
	void setStatus(QString status);
	virtual void setUserInfo(QString info){};
	void setSessionID(QUuid sessionID){if(sessionID_ != sessionID){ newSession(); sessionID_ = sessionID;}};
	QUuid getSessionID(){return sessionID_;};
	void update(int timeoutMs);
	ComponentStatus getStatus();
	QString getStatusAsString();
	void forceExit(){forceExit_ = true;};
	bool exitTriggered(){return forceExit_;};
protected:
	virtual void newSession() = 0;
	virtual void doServerUpdate() = 0;
private:
	QString name_;
	ComponentStatus status_;
	static QMap<QString,ComponentStatus> statusTransitionDictionary_;
	static QMap<ComponentStatus,QString> statusNameDictionary_;
	QDateTime lastUpdateTime_;
	QUuid sessionID_;
	bool forceExit_;
};
#endif
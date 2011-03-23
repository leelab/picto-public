#ifndef COMPONENTSTATUSMANAGER_H
#define COMPONENTSTATUSMANAGER_H

#include <QMap>
#include <QString>
#include <QTime>
#include <QUuid>
#include "../common.h"

enum ComponentStatus
{
	idle, ending, stopped, paused, running
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentStatusManager
#else
class ComponentStatusManager
#endif
{
public:
	ComponentStatusManager();
	void setName(QString name){name_ = name;};
	QString getName(){return name_;};
	void setStatus(ComponentStatus status);
	void setStatus(QString status);
	void setSessionID(QUuid sessionID){if(sessionID_ != sessionID){ newSession(); sessionID_ = sessionID;}};
	QUuid getSessionID(){return sessionID_;};
	void update(int timeoutMs);
	ComponentStatus getStatus();
	QString getStatusAsString();
protected:
	virtual void newSession() = 0;
	virtual void doServerUpdate() = 0;
private:
	QString name_;
	ComponentStatus status_;
	static QMap<QString,ComponentStatus> statusTransitionDictionary_;
	static QMap<ComponentStatus,QString> statusNameDictionary_;
	QTime lastUpdateTime_;
	QUuid sessionID_;
};
#endif
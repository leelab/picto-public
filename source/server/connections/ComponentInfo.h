#ifndef _COMPONENT_INFO_H_
#define _COMPONENT_INFO_H_

#include <QString>
#include <QUuid>
#include <QMutex>

namespace ComponentStatus
{
	enum ComponentStatus
	{
		notFound, idle, ending, stopped, paused, running
	};
}


/*!	\brief Contains information about a ComponentInterface (ie. Director or Proxy) 
 *	that is connected to the Picto Server.
 *
 *	\details The Server may have multiple Director and Proxy instances connected to it
 *	at any given time.  This object holds info about those Components and provides a 
 *	mechanism for checking if they haven't contacted the server in a while (setActivity(),
 *	clearActivity()).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ComponentInfo 
{
public:
	ComponentInfo();

	void setAddress(QString addressStr);
	void setName(QString name);
	void setType(QString type);
	void setUuid(QUuid uuid);
	void setSessionID(QUuid sessionID);
	void setStatus(ComponentStatus::ComponentStatus status);
	void setDetails(QString details);
	void setActivity();

	QString getAddress();
	QString getName();
	QString getType();
	QUuid getUuid();
	QUuid getSessionID();
	ComponentStatus::ComponentStatus getStatus();
	QString getDetails();
	bool clearActivity();

private:
	QMutex mutex_;
	QString addressStr_;
	QString name_;
	QString type_;
	QUuid	uuid_;
	QUuid	sessionID_;
	ComponentStatus::ComponentStatus status_;
	QString details_;

	bool activity_;
};

#endif


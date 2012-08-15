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


/*!	\brief Contains info about a director connected to the server
 *
 *	The Server will likely have multiple Director instances connected
 *	at any given time.  This object holds info about the directors, 
 *	and provides a mechanism for catching timeouts.
 *
 *	This is more of a data structure than a true object, but I had
 *	to make it an actual object to deal with the timeouts.  Note that 
 *	it is a friend of the Connection Manager, so we don't have any 
 *	setter/getter functions.
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


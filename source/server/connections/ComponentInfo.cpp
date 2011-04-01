#include "ComponentInfo.h"

#include <QMutexLocker>
#include <QApplication>
#include <QThread>

ComponentInfo::ComponentInfo()
{
	activity_=true;
}
//! \brief sets the address of the component to the input value
void ComponentInfo::setAddress(QString addressStr)
{QMutexLocker locker(&mutex_);addressStr_ = addressStr;}
//! \brief sets the name of the component to the input value
void ComponentInfo::setName(QString name)
{QMutexLocker locker(&mutex_);name_ = name;}
//! \brief sets the type of the component to the input value
void ComponentInfo::setType(QString type)
{QMutexLocker locker(&mutex_);type_ = type;}
//! \brief sets the uuid of the component to the input value
void ComponentInfo::setUuid(QUuid uuid)
{QMutexLocker locker(&mutex_);uuid_ = uuid;}
//! \brief sets the sessionID with which this component is associated
void ComponentInfo::setSessionID(QUuid sessionID)
{QMutexLocker locker(&mutex_);sessionID_ = sessionID;}
//! \brief sets the status of the component to the input value
void ComponentInfo::setStatus(ComponentStatus::ComponentStatus status)
{QMutexLocker locker(&mutex_);status_ = status;}
/*! \brief sets the activity of the component to true
 *	Used in conjunction with clear activity.  SetActivity should be called
 *	every time data is recieved from a component and clearActivity should
 *	be called once every timeout period to make sure that some activity 
 *	occured during that period.
 */
void ComponentInfo::setActivity() 
{QMutexLocker locker(&mutex_);activity_ = true;}

//! \brief returns the address of the component
QString ComponentInfo::getAddress()
{QMutexLocker locker(&mutex_);return addressStr_;}
//! \brief returns the name of the component
QString ComponentInfo::getName()
{QMutexLocker locker(&mutex_);return name_;}
//! \brief returns the type of the component
QString ComponentInfo::getType()
{QMutexLocker locker(&mutex_);return type_;}
//! \brief returns the Uuid of the component
QUuid ComponentInfo::getUuid()
{QMutexLocker locker(&mutex_);return uuid_;}
//! \brief returns the sessionID with which the component is associated
QUuid ComponentInfo::getSessionID()
{QMutexLocker locker(&mutex_);return sessionID_;}
//! \brief returns the status of the component
ComponentStatus::ComponentStatus ComponentInfo::getStatus()
{QMutexLocker locker(&mutex_);return status_;}
//! \brief clears the state of activity and returns it.
bool ComponentInfo::clearActivity() 
{
	QMutexLocker locker(&mutex_);
	bool temp = activity_; 
	activity_ = false; 
	return temp; 
};
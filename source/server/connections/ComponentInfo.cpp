#include "ComponentInfo.h"

#include <QMutexLocker>
#include <QApplication>
#include <QThread>
#include "../../common/memleakdetect.h"


ComponentInfo::ComponentInfo()
{
	activity_=true;
}
//! \brief Sets the address of the component to the input value.
void ComponentInfo::setAddress(QString addressStr)
{QMutexLocker locker(&mutex_);addressStr_ = addressStr;}
//! \brief Sets the name of the component to the input value.
void ComponentInfo::setName(QString name)
{QMutexLocker locker(&mutex_);name_ = name;}
//! \brief Sets the type of the component to the input value.
void ComponentInfo::setType(QString type)
{QMutexLocker locker(&mutex_);type_ = type;}
//! \brief Sets the uuid of the component to the input value.
void ComponentInfo::setUuid(QUuid uuid)
{QMutexLocker locker(&mutex_);uuid_ = uuid;}
//! \brief Sets the sessionID with which this component is associated.
void ComponentInfo::setSessionID(QUuid sessionID)
{QMutexLocker locker(&mutex_);sessionID_ = sessionID;}
//! \brief Sets the status of the component to the input value.
void ComponentInfo::setStatus(ComponentStatus::ComponentStatus status)
{QMutexLocker locker(&mutex_);status_ = status;}
//! \brief Sets the details of the component to the input value.
void ComponentInfo::setDetails(QString details)
{QMutexLocker locker(&mutex_);details_ = details;}
/*! \brief Sets the activity of the component to true
 *	\details This is used in conjunction with clearActivity().  SetActivity() should be called
 *	every time data is recieved from a component and clearActivity() should
 *	be called once every timeout period to make sure that some activity 
 *	occured during that period.  When clearActivity() returns false, we know
 *	that the Component must be disconnected because we haven't heard from it in
 *	at least one full timeout period.
 */
void ComponentInfo::setActivity() 
{QMutexLocker locker(&mutex_);activity_ = true;}

//! \brief Returns the address of the component
QString ComponentInfo::getAddress()
{QMutexLocker locker(&mutex_);return addressStr_;}
//! \brief Returns the name of the component
QString ComponentInfo::getName()
{QMutexLocker locker(&mutex_);return name_;}
//! \brief Returns the type of the component
QString ComponentInfo::getType()
{QMutexLocker locker(&mutex_);return type_;}
//! \brief Returns the Uuid of the component
QUuid ComponentInfo::getUuid()
{QMutexLocker locker(&mutex_);return uuid_;}
//! \brief Returns the sessionID with which the component is associated
QUuid ComponentInfo::getSessionID()
{QMutexLocker locker(&mutex_);return sessionID_;}
//! \brief Returns the status of the component
ComponentStatus::ComponentStatus ComponentInfo::getStatus()
{QMutexLocker locker(&mutex_);return status_;}
//! \brief Returns the details of the component
QString ComponentInfo::getDetails()
{QMutexLocker locker(&mutex_);return details_;}
/*! \brief Returns the activity flag and sets it to false.
 *	\details This is used in conjunction with SetActivity().  SetActivity() should be called
 *	every time data is recieved from a component and clearActivity() should
 *	be called once every timeout period to make sure that some activity 
 *	occured during that period.  When clearActivity() returns false, we know
 *	that the Component must be disconnected because we haven't heard from it in
 *	at least one full timeout period.
 */
bool ComponentInfo::clearActivity() 
{
	QMutexLocker locker(&mutex_);
	bool temp = activity_; 
	activity_ = false; 
	return temp; 
};
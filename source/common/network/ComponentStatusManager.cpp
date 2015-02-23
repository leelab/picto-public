#include <QApplication>
#include <QTimer>
#include "ComponentStatusManager.h"
#include "../memleakdetect.h"

QMap<QString,ComponentStatus> ComponentStatusManager::statusTransitionDictionary_;
QMap<ComponentStatus,QString> ComponentStatusManager::statusNameDictionary_;
/*! \brief The number of milliseconds delay that will be enforced between calls to doFrequentUpdate()
*/
#define MSPERFREQUPDATE 16
/*! \brief Constructs a new ComponentStatusManager.
 *	\details Initializes a statusTransitionDictionary_ with the states to which the ComponentInterface should transition as a function of the 
 *	command type recieved from the server.  Also initializes a statusNameDictionary_ with the text name of a particular ComponentInterface state
 *	as a function of state type.  ComponentInterface status is initialized to 'disconnected'.
 */
ComponentStatusManager::ComponentStatusManager()
{
	status_ = disconnected;
	if(statusTransitionDictionary_.isEmpty())
	{
		statusTransitionDictionary_["NEWSESSION"] = stopped;
		statusTransitionDictionary_["LOADEXP"] = stopped;
		statusTransitionDictionary_["START"] = running;
		statusTransitionDictionary_["STOP"] = stopped;
		statusTransitionDictionary_["PAUSE"] = paused;
		statusTransitionDictionary_["RESUME"] = running;
		statusTransitionDictionary_["ENDSESSION"] = idle;
	}
	if(statusNameDictionary_.isEmpty())
	{
		statusNameDictionary_[disconnected] = "Disconnected";
		statusNameDictionary_[idle] = "Idle";
		statusNameDictionary_[ending] = "Ending";
		statusNameDictionary_[stopped] = "Stopped";
		statusNameDictionary_[running] = "Running";
		statusNameDictionary_[paused] = "Paused";
	}
	lastUpdateTime_ = QDateTime::currentDateTime();
	lastFreqUpdateTime_ = lastUpdateTime_;
	forceExit_ = false;
}
/*! \brief Changes the ComponentStatus of the ComponentInterface handled by this ComponentStatusManager to the input value.
*/
void ComponentStatusManager::setStatus(ComponentStatus status)
{
	status_ = status;
	qDebug("Status was set to: " + getStatusAsString().toLatin1());
}

/*! \brief Changes the ComponentStatus of the ComponentInterface handled by this ComponentStatusManager to the ComponentStatus represented by the input string.
 *	\details If the input string does not represent a valid ComponentStatus, nothing happens.
*/
void ComponentStatusManager::setStatus(QString status)
{
	if(statusTransitionDictionary_.contains(status))
	{
		setStatus(statusTransitionDictionary_.value(status));
	}
}

/*! \brief Calls doFrequentUpdate() and doServerUpdate() according to scheduled timing.
 *	\details This function does not schedule function calls at a later time.  Rather it checks the current
 *	time and calls the doFrequentUpdate() and doServerUpdate() functions so long as enough time has passed 
 *	since the last calls to those functions.  For this reason it should be called frequently, with period
 *	at or greater than MSPERFREQUPDATE
 *
 *	The input timeoutMs is the approximate amout of time that this function will take.  It uses that time to
 *	run an event loop which may or may not be helping Qt not freak out about the fact that we don't use a
 *	standard Qt event loop in our ComponentInterface applications.  For more details about how we don't
 *	use a Qt event loop in ComponentInterface applications, but really should, See CommandChannel::processResponses() 
 *	documentation.
 */
void ComponentStatusManager::update(int timeoutMs)
{
	QDateTime startTime = QDateTime::currentDateTime();
	if((getStatus()<=stopped) && (lastFreqUpdateTime_.msecsTo(startTime)>=MSPERFREQUPDATE))
	{
		doFrequentUpdate();
		lastFreqUpdateTime_ = startTime;
	}
	if(lastUpdateTime_.secsTo(startTime)>1)
	{
		doServerUpdate();
		lastUpdateTime_ = startTime;
	}

	if((timeoutMs < 0) || (timeoutMs - (startTime.time().msecsTo(QDateTime::currentDateTime().time())) >= 40))
	{
		//if(getStatus() != running)
		//{
			//Pause for 20 ms 
			QEventLoop pauseLoop;
			QTimer pauseTimer;
			pauseTimer.setSingleShot(true);
			pauseTimer.setInterval(20);
			QObject::connect(&pauseTimer, SIGNAL(timeout()), &pauseLoop, SLOT(quit()));
			pauseTimer.start();
			pauseLoop.exec();
		//}
	}
};

/*! \brief Returns the current ComponentStatus of the ComponentInterface handled by this ComponentStatusManager.
*/
ComponentStatus ComponentStatusManager::getStatus()
{
	return status_;
}
/*! \brief Returns a status string representing the current ComponentStatus of the ComponentInterface handled by this ComponentStatusManager.
*/
QString ComponentStatusManager::getStatusAsString()
{
	return statusNameDictionary_.value(status_,QString("Undefined Status"));
}

/*! \brief After this is called, any CommandChannel::processResponses() loops that are in progress will not continue.
 *	\details Due to the strange (ie. needs to change) run model of the CompomnentInterface for which an endlessly looping CommandChannel::processResponses()
 *	function is essentially the event loop, the only way to exit the application is to call this function.  Once per loop inside the CommandChannel::processResponses()
 *	function, exitTriggered() is checked and if it returns true, the loop doesn't continue, allowing the ComponentInterface::activate() that calls 
 *	CommandChannel::processResponses(-1) to end.
 */
void ComponentStatusManager::forceExit()
{
	forceExit_ = true;
}
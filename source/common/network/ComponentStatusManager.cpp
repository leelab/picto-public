#include <QApplication>
#include <QTimer>
#include "ComponentStatusManager.h"
#include "../memleakdetect.h"

QMap<QString,ComponentStatus> ComponentStatusManager::statusTransitionDictionary_;
QMap<ComponentStatus,QString> ComponentStatusManager::statusNameDictionary_;

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
		statusNameDictionary_[idle] = "Idle";
		statusNameDictionary_[ending] = "Ending";
		statusNameDictionary_[stopped] = "Stopped";
		statusNameDictionary_[running] = "Running";
		statusNameDictionary_[paused] = "Paused";
	}
	lastUpdateTime_ = QDateTime::currentDateTime();
}
void ComponentStatusManager::setStatus(ComponentStatus status)
{
	status_ = status;
	qDebug("Status was set to: " + getStatusAsString().toAscii());
}

void ComponentStatusManager::setStatus(QString status)
{
	if(statusTransitionDictionary_.contains(status))
	{
		setStatus(statusTransitionDictionary_.value(status));
	}
}

void ComponentStatusManager::update(int timeoutMs)
{
	QDateTime startTime = QDateTime::currentDateTime();
	if(lastUpdateTime_.secsTo(startTime)>1)
	{
		doServerUpdate();
		lastUpdateTime_ = QDateTime::currentDateTime();
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

ComponentStatus ComponentStatusManager::getStatus()
{
	return status_;
}
QString ComponentStatusManager::getStatusAsString()
{
	return statusNameDictionary_.value(status_,QString("Undefined Status"));
}
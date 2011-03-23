#include "ComponentStatusManager.h"
#include <QApplication>
#include <QTimer>

QMap<QString,ComponentStatus> ComponentStatusManager::statusTransitionDictionary_;
QMap<ComponentStatus,QString> ComponentStatusManager::statusNameDictionary_;

ComponentStatusManager::ComponentStatusManager()
{
	status_ = idle;
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
	lastUpdateTime_ = QTime::currentTime();
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
	QTime startTime = QTime::currentTime();
	if(lastUpdateTime_.msecsTo(startTime)>1000)
	{
		doServerUpdate();
		lastUpdateTime_ = QTime::currentTime();
	}
	if((timeoutMs < 0) || (timeoutMs - (startTime.msecsTo(QTime::currentTime())) >= 40))
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
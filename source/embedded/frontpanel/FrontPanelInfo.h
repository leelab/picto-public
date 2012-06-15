
#ifndef FRONTPANEL_H
#define FRONTPANEL_H

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>

namespace PanelInfo
{
	enum DisplayModeType{MenuModeType, StatusModeType, RewardDurModeType, RewardControllerModeType, 
		FlushDurModeType, ChangeNameModeType};
	enum SystemStatus{disconnected, connected, running};
	enum InputType{buttonPush, rotateLeft, rotateLeftFast, rotateRight, rotateRightFast};
}

/*!	\brief Stores information about the front panel.
 *
 *	FrontPanelInfo is an object used to store data about the current state of
 *	the front panel (reward duration, current display mode, etc).  The object
 *	is thread safe (since we will be accessing it from different threads as commands 
 *	come in).  QMutexLocker locks the mutex upon creation, and unlocks it when
 *	we the locker is destroyed (when we leave the function).
 */
//This is perhaps overly complicated...
//When I built it, I was thinking that different parts of the front panel program 
//were likely to be accessing this data simultaneously.  It is becoming clear that
//this isn't really an issue, so I may want to clean this up...

class FrontPanelInfo
{
public:
	FrontPanelInfo();
	virtual ~FrontPanelInfo() {};

	int getRewardController() { return rewardController; };
	void setRewardController(int controller) { rewardController = controller; };
	
	QTcpSocket* getCommandSocket() { return commandSocket; }
	void setCommandSocket(QTcpSocket* socket) { commandSocket = socket; };

	QTcpSocket* getEventSocket() { return eventSocket; }
	void setEventSocket(QTcpSocket* socket) { eventSocket = socket; };


private:
	int rewardController;

	QTcpSocket *commandSocket;
	QTcpSocket *eventSocket;
};
#endif
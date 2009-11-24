// FrontPanelInfo is an object used to store data about the current state of
// the front panel (reward duration, current display mode, etc).  The object
// is thread safe (since we will be accessing it from different threads as commands 
// come in).  QMutexLocker locks the mutex upon creation, and unlocks it when
// we the locker is destroyed (when we leave the function).

#ifndef FRONTPANEL_H
#define FRONTPANEL_H

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>

namespace PanelInfo
{
	enum DisplayMode{MenuMode, StatusMode, MessageMode, RewardDurMode, RewardControllerMode, 
		FlushDurMode, FlushingMode, ChangeIPMode, ChangeNameMode, WaitForEngine};
	enum SystemStatus{disconnected, connected, running};
	enum InputType{buttonPush, rotateLeft, rotateRight};
}

//This is perhaps overly complicated...
//When I built it, I was thinking that different parts of the front panel program 
//were likely to be accessing this data simultaneously.  It is becoming clear that
//this isn't really an issue, so I may want to clean this up...

class FrontPanelInfo
{
public:
	FrontPanelInfo();
	~FrontPanelInfo() {};

	int getRewardDuration() { return rewardDuration; };
	void setRewardDuration(int dur) { rewardDuration = dur; };

	int getRewardController() { return rewardController; };
	void setRewardController(int controller) { rewardController = controller; };
	
	int getFlushDuration() { return flushDuration; };
	void setFlushDuration(int dur) { flushDuration = dur; };

	PanelInfo::DisplayMode getDispMode() { return dispMode; };
	void setDispMode(PanelInfo::DisplayMode mode) { dispMode = mode; };

	QString getSystemName() { return systemName; }
	void setSystemName(QString name) { systemName = name; };

	QHostAddress getSystemIP() { return systemIP; }
	void setSystemIP(QHostAddress ip) { systemIP = ip; };

	PanelInfo::SystemStatus getSystemStatus() { return status; };
	void setSystemStatus(PanelInfo::SystemStatus stat) { status = stat; };

	QTcpSocket* getCommandSocket() { return commandSocket; }
	void setCommandSocket(QTcpSocket* socket) { commandSocket = socket; };

	QTcpSocket* getEventSocket() { return eventSocket; }
	void setEventSocket(QTcpSocket* socket) { eventSocket = socket; };

	int getTrial() { return trial; };
	void setTrial(int tr) { trial = tr; };

	int getBlock() { return block; };
	void setBlock(int blk) { block = blk; };

	QString getLastEvent() { return lastEvent; }
	void setLastEvent(QString lastEvent) { this->lastEvent = lastEvent; };


private:
	//any access to the data members is protected with a single lock.  
	//This is slightly less efficient than protecting the data members seperately, 
	//but it makes the code easier to write.
	QMutex mutex;

	int rewardDuration;		//for current reward controller only
	int rewardController;
	int flushDuration;      //for current reward controller only

	PanelInfo::DisplayMode dispMode;
	QString systemName;
	QHostAddress systemIP;
	PanelInfo::SystemStatus status;

	QTcpSocket *commandSocket;
	QTcpSocket *eventSocket;

	/*! \todo  At the moment, we are only storing the most recent event.  Should
	 *  we store the last 10 events?  Then the user could scroll forward and backward
	 */
	int trial;
	int block;
	QString lastEvent;






};

//OLD VERSION WITH MUTEX...

/*class FrontPanelInfo
{
public:
	FrontPanelInfo();
	~FrontPanelInfo() {};

	int getRewardDuration() { QMutexLocker locker(&mutex); return rewardDuration; };
	void setRewardDuration(int dur) { QMutexLocker locker(&mutex); rewardDuration = dur; };

	int getRewardController() { QMutexLocker locker(&mutex); return rewardController; };
	void setRewardController(int controller) { QMutexLocker locker(&mutex); rewardController = controller; };
	
	int getFlushDuration() { QMutexLocker locker(&mutex); return flushDuration; };
	void setFlushDuration(int dur) { QMutexLocker locker(&mutex); flushDuration = dur; };

	PanelInfo::DisplayMode getDispMode() { QMutexLocker locker(&mutex); return dispMode; };
	void setDispMode(PanelInfo::DisplayMode mode) { QMutexLocker locker(&mutex); dispMode = mode; };

	QString getSystemName() { QMutexLocker locker(&mutex); return systemName; }
	void setSystemName(QString name) { QMutexLocker locker(&mutex); systemName = name; };

	QHostAddress getSystemIP() { QMutexLocker locker(&mutex); return systemIP; }
	void setSystemIP(QHostAddress ip) { QMutexLocker locker(&mutex); systemIP = ip; };

	PanelInfo::SystemStatus getSystemStatus() { QMutexLocker locker(&mutex); return status; };
	void setSystemStatus(PanelInfo::SystemStatus stat) { QMutexLocker locker(&mutex); status = stat; };

	QTcpSocket* getCommandSocket() { QMutexLocker locker(&mutex); return commandSocket; }
	void setCommandSocket(QTcpSocket* socket) { QMutexLocker locker(&mutex); commandSocket = socket; };

	QTcpSocket* getEventSocket() { QMutexLocker locker(&mutex); return eventSocket; }
	void setEventSocket(QTcpSocket* socket) { QMutexLocker locker(&mutex); eventSocket = socket; };

	int getTrial() { QMutexLocker locker(&mutex); return trial; };
	void setTrial(int tr) { QMutexLocker locker(&mutex); trial = tr; };

	int getBlock() { QMutexLocker locker(&mutex); return block; };
	void setBlock(int blk) { QMutexLocker locker(&mutex); block = blk; };

	QString getLastEvent() { QMutexLocker locker(&mutex); return lastEvent; }
	void setLastEvent(QString lastEvent) { QMutexLocker locker(&mutex); this->lastEvent = lastEvent; };


private:
	//any access to the data members is protected with a single lock.  
	//This is slightly less efficient than protecting the data members seperately, 
	//but it makes the code easier to write.
	QMutex mutex;

	int rewardDuration;		//for current reward controller only
	int rewardController;
	int flushDuration;      //for current reward controller only

	PanelInfo::DisplayMode dispMode;
	QString systemName;
	QHostAddress systemIP;
	PanelInfo::SystemStatus status;

	QTcpSocket *commandSocket;
	QTcpSocket *eventSocket;*/

	/*! \todo  At the moment, we are only storing the most recent event.  Should
	 *  we store the last 10 events?  Then the user could scroll forward and backward
	 */
	/*int trial;
	int block;
	QString lastEvent;






};*/

#endif
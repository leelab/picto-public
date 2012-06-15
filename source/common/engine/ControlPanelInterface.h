#ifndef CONTROLPANELINTERFACE_H_
#define CONTROLPANELINTERFACE_H_

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/protocol/protocolcommandhandler.h"


#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlPanelInterface : public QObject
#else
class ControlPanelInterface : public QObject
#endif
{
	Q_OBJECT

public:
	ControlPanelInterface();
	~ControlPanelInterface();

	//getters/setters

public slots:
	//These functions are called by the director whenever director state data
	//changes.  When they are called, data is updated in this object and then
	//child classes are informed of the change.
	void nameChanged(QString name){name_ = name;nameWasChanged();};
	void ipAddressChanged(QString addr){ipAddress_ = addr;ipWasChanged();};
	void rewardDurationChanged(int controller, int duration);
	void flushDurationChanged(int controller, int duration);
	void statusChanged(QString status){status_ = status;statusWasChanged(status);};
	//Should be implemented by child to process incoming commands from 
	//The control panel hardware
	virtual void doIncomingCommands() = 0;

signals:
	//These signals are used to inform whoever's interested that the control panel
	//would like to change the director state data.
	void nameChangeRequest(QString name);
	void rewardDurationChangeRequest(int controller, int duration);
	void flushDurationChangeRequest(int controller, int duration);
	void startFlushRequest(int controller);
	void stopFlushRequest(int controller);
	void giveRewardRequest(int controller);

protected:
	//When state data changes in the director, after that data is recorded by this
	//object, these functions are called on the child objects to inform them of the change.
	virtual void nameWasChanged()=0;
	virtual void ipWasChanged()=0;
	virtual void rewardDurationWasChanged(int controller)=0;
	virtual void flushDurationWasChanged(int controller)=0;
	virtual void statusWasChanged(QString status)=0;

	//These functions are used by child objects to get current director state data
	QString getName() { return name_; };
	QString getIP() { return ipAddress_; };
	int getRewardDuration(int controller);
	int getFlushDuration(int controller);
	QString getStatus(){return status_;};

protected slots:
	//These functions are called by child classes to request changes in the director state data.
	void setName(QString name){emit nameChangeRequest(name);};
	void setRewardDuration(int controller, int duration){emit rewardDurationChangeRequest(controller,duration);};
	void setFlushDuration(int controller, int duration) {emit flushDurationChangeRequest(controller,duration);};
	void startFlush(int controller){emit startFlushRequest(controller);};
	void stopFlush(int controller){emit stopFlushRequest(controller);};
	void giveReward(int controller){emit giveRewardRequest(controller);};

private:
	QString name_;
	QString ipAddress_;
	QVector<int> rewardDurations_;
	QVector<int> flushDurations_;
	QString status_;
};

#endif
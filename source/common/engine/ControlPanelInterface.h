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
	void nameChanged(QString name){name_ = name;nameWasChanged();};
	void ipAddressChanged(QString addr){ipAddress_ = addr;ipWasChanged();};
	void rewardDurationChanged(int controller, int duration);
	void flushDurationChanged(int controller, int duration);
	//Should be implemented by child to process incoming commands from 
	//The control panel hardware
	virtual void doIncomingCommands() = 0;

signals:
	void nameChangeRequest(QString name);
	void rewardDurationChangeRequest(int controller, int duration);
	void flushDurationChangeRequest(int controller, int duration);
	void startFlushRequest(int controller);
	void stopFlushRequest(int controller);
	void giveRewardRequest(int controller);

protected:
	virtual void nameWasChanged()=0;
	virtual void ipWasChanged()=0;
	virtual void rewardDurationWasChanged(int controller)=0;
	virtual void flushDurationWasChanged(int controller)=0;

	QString getName() { return name_; };
	QString getIP() { return ipAddress_; };
	int getRewardDuration(int controller);
	int getFlushDuration(int controller);

protected slots:
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
};

#endif
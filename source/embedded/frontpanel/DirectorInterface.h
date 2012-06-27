#ifndef DIRECTORINTERFACE_H
#define DIRECTORINTERFACE_H

#include <QList>
#include <QString>
#include <QObject>
#include <QTimer>

#include "FrontPanelInfo.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

enum DirectorStatus {DirUnknown,DirDisconnect,DirConnect,DirEnding,DirStop,DirRun,DirPause};
/*!	\brief An interface object that encapsulates socket activity for the Front Panel Menu
 *
 *
 */
class DirectorInterface : public QObject
{
	Q_OBJECT

public:
	DirectorInterface(FrontPanelInfo *panelInfo);
	~DirectorInterface();
	bool setIP(QString addr);
	QString getIP();
	bool setName(QString name);
	QString getName();
	DirectorStatus getStatus();
	bool setRewardDuration(int rewardDur,int controller);
	int getRewardDuration(int controller);
	bool setFlushDuration(int flushDur,int controller);
	int getFlushDuration(int controller);
	bool flush(int controller);
	int getFlushTimeRemaining(int controller);
	bool startReward(int controller);
	bool isConnected();
private:
	bool sendCommandGetResponse(Picto::ProtocolCommand command,QString* reply);
	FrontPanelInfo *panelInfo;
	QTimer *connectionTimer;
	QString lastName_;
	QString lastIp_;
	DirectorStatus lastStatus_;
	int lastRewardDur_;
	int lastFlushDur_;
	int lastCommandId_;
};


#endif


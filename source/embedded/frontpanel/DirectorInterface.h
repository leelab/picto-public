#ifndef DIRECTORINTERFACE_H
#define DIRECTORINTERFACE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QTimer>
#include <qsharedpointer.h>

#include "FrontPanelInfo.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "FrontPanelRewardController.h"
using namespace Picto;

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
	QSharedPointer<FrontPanelRewardController> rewardController_;
	FrontPanelInfo *panelInfo;
	QTimer *connectionTimer;
	QString lastIp_;
	DirectorStatus lastStatus_;
	QStringList rewardDurs_;
	QStringList flushDurs_;
	int lastCommandId_;
	QSharedPointer<QTimer> rewardTimer_;
private slots:
	void updateLocalRewards();
};


#endif


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


/*! \file
 * \enum An enum describing the current status of the director
 */
enum DirectorStatus {
	DirUnknown, /*!< Cant connect to Director application, status is unknown*/
	DirDisconnect, /*!< Director is disconnected from Server,*/
	DirConnect, /*!< Director is connected to server with no experiment loaded*/
	DirEnding, /*!< Director is connected to the server and ending a previously loaded experiment*/
	DirStop, /*!< Director is connected to the server and stopped with an experiment loaded*/
	DirRun, /*!< Director is connected to the server and running an experiment*/
	DirPause /*!< Director is connected to the server and paused while running an experiment*/
};
/*!	\brief An interface that encapsulates socket communicate to the director from the Front Panel Application
 * \details This interface handles all communication for the Front Panel Application to and from the
 * director application.  In the front panel to director communication paradigm, the front panel acts
 * as the master.  Both set and get commands are initiated by the front panel with the director acting
 * as a slave and simply responding to these commands.  Set commands return true if the value was 
 * succesfully set and false otherwise.  getCommands return the requested value or an invalid value
 * (typically -1) if communication to the director failed.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
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


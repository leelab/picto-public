#ifndef DIRECTORINTERFACE_H
#define DIRECTORINTERFACE_H

#include <QList>
#include <QString>
#include <QObject>
#include <QTimer>

#include "FrontPanelInfo.h"

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
	bool setRewardDuration(int rewardDur,int controller);
	int getRewardDuration(int controller);
	bool setFlushDuration(int flushDur,int controller);
	int getFlushDuration(int controller);
	bool startFlush(int controller);
	int getFlushTimeRemaining(int controller);
	bool stopFlush(int controller);
	bool startReward(int controller);
	bool isConnected();
private:
	FrontPanelInfo *panelInfo;
	QTimer *connectionTimer;	

};


#endif


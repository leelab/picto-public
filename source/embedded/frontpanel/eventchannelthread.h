#ifndef EVENTCHANNELTHREAD_H
#define EVENTCHANNELTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "ENGEVENTCommandHandler.h"
#include "FrontPanelInfo.h"

class EventChannelThread : public QThread
{
	Q_OBJECT
public:
	EventChannelThread(FrontPanelInfo *panelInfo, QObject *parent);
	void run();
	

private slots:
	void readCommand();
signals:
	void newEventRead();


private:
	void processCommand(QSharedPointer<Picto::ProtocolCommand> command);

	FrontPanelInfo *panelInfo;
	ENGEVENTCommandHandler *engeventCommandHandler;
	int timeoutInterval;
};

#endif
#ifndef EVENTCHANNELTHREAD_H
#define EVENTCHANNELTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "ENGEVENTCommandHandler.h"
#include "FrontPanelInfo.h"


/*!	\brief Handles reading and processing incoming commands
 *
 *	Since the front panel talks to the engine over a network connection, we are using
 *	the same protocol command/response design used in PictoServer.  This thread reads
 *	the incoming commands and then processes them.
 */
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
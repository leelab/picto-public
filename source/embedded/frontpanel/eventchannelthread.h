#ifndef EVENTCHANNELTHREAD_H
#define EVENTCHANNELTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "ENGEVENTCommandHandler.h"
#include "FrontPanelInfo.h"


/*!	\brief Handles reading and processing incoming commands NO LONGER USED
 *
 * This object used to allow the director to push data to the FrontPanel application.  It is no longer
 * used. The front panel is now set up as the communications master handling both setting and polling 
 * data.  At a more opportune time, this object should probably be deleted and references to it removed
 * from other parts of the application.
 * \author Mark Hammond, Matt Gay
 * \date 2009-2013
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
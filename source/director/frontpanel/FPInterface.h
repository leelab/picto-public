#ifndef FPINTERFACE_H_
#define FPINTERFACE_H_

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/protocol/protocolcommandhandler.h"
#include "../../common/engine/ControlPanelInterface.h"
#include "FPGETCommandHandler.h"
#include "FPPUTCommandHandler.h"
#include "FPREWARDCommandHandler.h"
#include "FPFLUSHCommandHandler.h"


/*! \brief The interface between the Picto Director and the application managing the Pictobox Front Panel.
 *	\details This class implements the ControlPanelInterface for communication with the EmbeddedFrontPanel application.
 *	The interface is through a local socket and simple http style commands (ie. FPPUT, FPGET, etc).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class FPInterface : public ControlPanelInterface
{
	Q_OBJECT

public:
	FPInterface();
	~FPInterface();

public slots:
	virtual void doIncomingCommands();
protected:
	virtual void nameWasChanged();
	virtual void ipWasChanged();
	virtual void rewardDurationWasChanged(int controller);
	virtual void flushDurationWasChanged(int controller);
	virtual void statusWasChanged(QString status);

private:
	//private functions
	QSharedPointer<Picto::ProtocolCommand> readCommand();
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand> command);
	void deliverResponse(QSharedPointer<Picto::ProtocolResponse> response);

	void sendFPInterfaceEvent(QString xmlFragment);

	QSharedPointer<FPGETCommandHandler> fpgetCommandHandler_;
	QSharedPointer<FPPUTCommandHandler> fpputCommandHandler_;
	QSharedPointer<FPREWARDCommandHandler> fprewardCommandHandler_;
	QSharedPointer<FPFLUSHCommandHandler> fpstartflushCommandHandler_;

	//front panel connections
	QTcpSocket *commSocket;
	QTcpSocket *eventSocket;

	//protocol stuff
	QMap<QString, QSharedPointer<Picto::ProtocolCommandHandler>> commandHandlers;

};

#endif
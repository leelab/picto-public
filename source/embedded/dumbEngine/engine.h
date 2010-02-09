#ifndef DUMBENGINE_ENGINE_H
#define DUMBENGINE_ENGINE_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/protocol/protocolcommandhandler.h"
#include "../../common/iodevices/PictoBoxXPEventCodeGenerator.h"
#include "../../common/iodevices/PictoBoxXPRewardController.h"


class Engine : public QObject
{
	Q_OBJECT

public:
	Engine(QString boxName, QHostAddress addr, QObject* parent = 0);\
	~Engine();

	void runExperiment(int trialsPerBlock, int blocks);
	void testFunction();

	//getters/setters
	QHostAddress getIP() { return boxAddress; };
	void setIP(QHostAddress addr) { boxAddress = addr; };

	QString getName() { return boxName; };
	void setName(QString name) { boxName = name; };

	int getRewardDuration(int controller);
	void setRewardDuration(int controller, int duration);

	int getFlushDuration(int controller) { if(controller>0 && controller <=4) return flushDurations[controller-1]; else return 0; };
	void setFlushDuration(int controller, int duration) { if(controller>=0 && controller <4) flushDurations[controller-1] = duration; };

	int getFlushTimeRemain() { return flushTimeRemain; };

	void startFlush(int controller);
	void stopFlush(int controller);

	void giveReward(int controller);

private slots:
	void startTrial();
	void endTrial();
	void doFrame();
	void doIncomingCommands();
	void decrementFlushTime();

private:
	//private functions
	bool incomingCommand();
	QSharedPointer<Picto::ProtocolCommand> readCommand();
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand> command);
	void deliverResponse(QSharedPointer<Picto::ProtocolResponse> response);

	void sendEngineEvent(QString xmlFragment);


	//Timers
	QTimer *interTrialTimer;
	QTimer *trialTimer;
	QTimer *frameTimer;
	QTimer *flushingTimer;
	QTimer *commandTimer;

	//front panel connections
	QTcpSocket *commSocket;
	QTcpSocket *eventSocket;

	//Box information 
	QHostAddress boxAddress;
	QString boxName;
	int rewardDurations[4];
	int flushDurations[4];
	int flushTimeRemain;
	int flushingController;
	int currTrial;
	int currBlock;
	unsigned int eventCodeCounter;

	//experiment information
	int trialsPerBlock;
	int blocksToRun;

	//protocol stuff
	QMap<QString, QSharedPointer<Picto::ProtocolCommandHandler>> commandHandlers;

	//IOstuff
	Picto::PictoBoxXPEventCodeGenerator *eventGen;
	Picto::PictoBoxXPRewardController *rewardController;

};

#endif
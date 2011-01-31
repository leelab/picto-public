#ifndef _PICTODIRECTORSIMULATOR_H_
#define _PICTODIRECTORSIMULATOR_H_
#include <QObject>
#include <QSharedPointer>
#include <QHostAddress>
#include "PictoDeviceSimulator.h"
namespace PictoSim{

/*! \brief Simulates a picto director's network interface and checks for errors.
 */
class PictoDirectorSimulator : public PictoDeviceSimulator
{

public:
	PictoDirectorSimulator(PictoSystemState* systemState, QString deviceName);
	virtual QString GetDeviceTypeName();
	QHostAddress myIPAddress_;
protected:
	virtual void Init();
	virtual void Deinit();
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc);
private:
	QString SimulateNextMsg(QSharedPointer<SimActionDesc> actionDesc);
	QTime timer_;
	bool timerRunning_;
	QSharedPointer<QTcpSocket> tcpSocket_;
	QString session_;

};

namespace Director{
	/*! \brief the types of actions that the PictoDirectorSimulator can perform
	 */
enum DescTypes{
	DISCOVERSERVER,
	DIRECTORUPDATE,
	PUTDATA,
	TRIAL
};

/*! \brief Defines the act of discovering the server for the PictoDirectorSimulator.
 */
struct DiscoverServerDesc: public SimActionDesc
{
	DiscoverServerDesc(QString testStep): SimActionDesc(testStep, DISCOVERSERVER){};
};

/*! \brief Defines the act of starting director updates for the PictoDirectorSimulator.
 *	When the director simulator performs this action, it will loop until the action is 
 *	broken out of.  It sends a DirectorUpdate every second unless it receives a response
 *	containing a directive, which to which it promptly responds.  When the START directive
 *	is returned from the server, the director simulator begins looping through the
 *	messages associated with this action and sending TRIAL or PUTDATA messages accordingly.
 */
struct DirectorUpdateDesc: public SimActionDesc
{
	enum MsgType{TRIAL, DATA};
	struct TrialMsgDesc:public SimMsgDesc{TrialMsgDesc(float timestamp, int eventID, int trialID):SimMsgDesc(timestamp,TRIAL),eventID_(eventID),trialID_(trialID){};int eventID_; int trialID_;};
	struct DataMsgDesc:public SimMsgDesc{DataMsgDesc(float timestamp, int x, int y):SimMsgDesc(timestamp,DATA),x_(x),y_(y){};int x_; int y_;};
	DirectorUpdateDesc(QString testStep): SimActionDesc(testStep, DIRECTORUPDATE){};
};

/*! \brief Not Currently used.
 */
struct PutDataDesc: public SimActionDesc
{
	PutDataDesc(QString testStep): SimActionDesc(testStep, PUTDATA){};
};

/*! \brief Not Currently used.
 */
struct TrialDesc: public SimActionDesc
{
	TrialDesc(QString testStep, int msInterTrialDelay): SimActionDesc(testStep, TRIAL){msInterTrialDelay_ = msInterTrialDelay;};
	int msInterTrialDelay_;
};

};//	Director namespace

}; // PictoSim namespace
#endif
#ifndef _PICTOSERVERSIMULATOR_H_
#define _PICTOSERVERSIMULATOR_H_
#include <QVector>
#include <QHostAddress>
#include <QSharedPointer>
#include <QTcpServer>
#include <QTime>
#include <QUdpSocket>
#include "PictoDeviceSimulator.h"
namespace PictoSim{

/*! \brief Simulates a Picto Server's network interface and checks for errors.
 *	Not Yet Implemented
 */
class PictoServerSimulator : public PictoDeviceSimulator
{
public:
	PictoServerSimulator(PictoSystemState* systemState, QString deviceName);
	virtual QString GetDeviceTypeName();
	struct SessionSimulator
	{
		unsigned int sessionID_;
		QSharedPointer<unsigned int> authorizedWorkstation_;
		QVector<QSharedPointer<unsigned int>> unAuthorizedWorkstations_;
		QSharedPointer<unsigned int> director_;
		QSharedPointer<unsigned int> proxyServer_;
	};
	QVector<SessionSimulator> sessions_;
	QMap<QString,QSharedPointer<PictoDeviceSimulator>> knownDirectors_;
protected:
	virtual void Init();
	virtual void Deinit();
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc);
private:
	void CreateConnection(int timeout);
	void SendLatestMessages(QSharedPointer<SimActionDesc> actionDesc);
	QTime timer_;
	bool receivedMessage_;
	QSharedPointer<QTcpServer> tcpServer_;
	QSharedPointer<QTcpSocket> tcpSocket_;
	QSharedPointer<QUdpSocket> udpSocket_;
	QSharedPointer<QEventLoop> waitLoop_;
};

namespace Server{
	enum DescTypes{
		GETDISCOVERED,
		INTERFACEPROXY,
		INTERFACEDIRECTOR,
		INTERFACEWORKSTATION
	};

/*! \brief Defines the act of waiting to get discovered with a DISCOVER message.
 */
struct GetDiscoveredDesc: public SimActionDesc
{
	GetDiscoveredDesc(QString testStep, int timeoutMs): SimActionDesc(testStep, GETDISCOVERED){timeoutMs_ = timeoutMs;};
	int timeoutMs_;
};
	
/*! \brief Defines the act of interfacing the PictoProxyServer.
 */
struct InterfaceProxyDesc: public SimActionDesc
{
	/*!	\brief Defines a Get data message sent at a particular time according to the timestamp.
	 */
	struct GetMsgDesc:public SimMsgDesc{GetMsgDesc(float timestamp):SimMsgDesc(timestamp,0){};};
	InterfaceProxyDesc(QString testStep): SimActionDesc(testStep, INTERFACEPROXY){};
};

/*! \brief Defines the act of interfacing the PictoDirector.
 */
struct InterfaceDirectorDesc: public SimActionDesc
{
	enum MsgType{NEWSESSION,ENDSESSION,LOADEXP,START,STOP,PAUSE};
	/*!	\brief Defines a NewSession message.
	 */
	struct NewSessionMsgDesc:public SimMsgDesc{NewSessionMsgDesc():SimMsgDesc(0,NEWSESSION){};};
	/*!	\brief Defines an EndSession message
	 */
	struct EndSessionMsgDesc:public SimMsgDesc{EndSessionMsgDesc():SimMsgDesc(0,ENDSESSION){};};
	/*!	\brief Defines a LoadExperiment message
	 */
	struct LoadExpMsgDesc:public SimMsgDesc{LoadExpMsgDesc():SimMsgDesc(0,LOADEXP){};};
	/*!	\brief Defines a Start message to be sent a particular interval(ms) after the last message
	 */
	struct StartMsgDesc:public SimMsgDesc{StartMsgDesc(float interval):SimMsgDesc(interval,START){};};
	/*!	\brief Defines an Stop message to be sent a particular interval(ms) after the last message
	 */
	struct StopMsgDesc:public SimMsgDesc{StopMsgDesc(float interval):SimMsgDesc(interval,STOP){};};
	/*!	\brief Defines an Pause message to be sent a particular interval(ms) after the last message
	 */
	struct PauseMsgDesc:public SimMsgDesc{PauseMsgDesc(float interval):SimMsgDesc(interval,PAUSE){};};
	InterfaceDirectorDesc(QString testStep): SimActionDesc(testStep, INTERFACEDIRECTOR){};
};

/*! \brief Defines the act of interfacing the PictoWorkstation.
 */
struct InterfaceWorkstationDesc: public SimActionDesc
{
	InterfaceWorkstationDesc(QString testStep): SimActionDesc(testStep, INTERFACEWORKSTATION){};
};

};// Server namespace

}; // PictoSim namespace
#endif
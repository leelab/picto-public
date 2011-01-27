#include <QObject>
#include <QTcpServer>
#include <QSharedPointer>
#include <QMutex>
#include <QVector>
#include "PictoDeviceSimulator.h"
namespace PictoSim{

//class ProxyServerSimRouter;

/*! \brief Simulates a picto proxy server's network interface and checks for errors.
 */
class PictoProxyServerSimulator : public PictoDeviceSimulator
{
	
public:
	PictoProxyServerSimulator(PictoSystemState* systemState, QString deviceName);
	virtual QString GetDeviceTypeName();
protected:
	virtual void Init();
	virtual void Deinit();
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc);
private:
	void SendLatestData(QSharedPointer<SimActionDesc> actionDesc);
	bool timerRunning_;
	int startDataTime_;
	QTime timer_;
	QSharedPointer<QTcpServer> tcpServer_;
	QSharedPointer<QTcpSocket> tcpSocket_;
	//QSharedPointer<ProxyServerSimRouter> proxyRouter_;

};

/*! \brief the types of actions that the PictoProxyServerSimulator can perform
 */
namespace ProxyServer{
	enum DescTypes{
		DISCOVERSERVER,
		ANNOUNCE,
		DEPART
	};
/*! \brief Defines the act of discovering the server for the PictoProxyServerSimulator.
 */
struct DiscoverServerDesc: public SimActionDesc
{
	DiscoverServerDesc(QString testStep): SimActionDesc(testStep, DISCOVERSERVER){};
};

/*! \brief Defines the act of announcing the PictoProxyServerSimulator's existance to the network.
 */
struct AnnounceDesc: public SimActionDesc
{
	enum MsgType{SPIKE,EXTEVENT};
	/*!	\brief Defines a Spike message at a particular timestamp on a particular channel.
	 */
	struct SpikeMsgDesc:public SimMsgDesc{SpikeMsgDesc(float timestamp, int channel):SimMsgDesc(timestamp,SPIKE),channel_(channel){};int channel_;};
	/*!	\brief Defines an external event message at a particular timestamp with a particular eventID for timing alignment purposes.
	 */
	struct ExtEventMsgDesc:public SimMsgDesc{ExtEventMsgDesc(float timestamp, int eventID):SimMsgDesc(timestamp,EXTEVENT),eventID_(eventID){};int eventID_;};
	AnnounceDesc(QString testStep): SimActionDesc(testStep, ANNOUNCE, true){};
};

/*! \brief Defines the act of announcing to the network that the PictoProxyServerSimulator is shutting down.
 */
struct DepartDesc: public SimActionDesc
{
	DepartDesc(QString testStep): SimActionDesc(testStep, DEPART, true){};
};

}; // ProxyServer namespace

}; // PictoSim namespace
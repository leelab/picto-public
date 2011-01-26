#include <QObject>
#include <QTcpServer>
#include <QSharedPointer>
#include <QMutex>
#include <QVector>
#include "PictoDeviceSimulator.h"
namespace PictoSim{

//class ProxyServerSimRouter;

/*! \brief 
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

namespace ProxyServer{
	enum DescTypes{
		DISCOVERSERVER,
		ANNOUNCE,
		DEPART
	};

struct DiscoverServerDesc: public SimActionDesc
{
	DiscoverServerDesc(QString testStep): SimActionDesc(testStep, DISCOVERSERVER){};
};

struct AnnounceDesc: public SimActionDesc
{
	enum MsgType{SPIKE,EXTEVENT};
	struct SpikeMsgDesc:public SimMsgDesc{SpikeMsgDesc(float timestamp, int channel):SimMsgDesc(timestamp,SPIKE),channel_(channel){};int channel_;};
	struct ExtEventMsgDesc:public SimMsgDesc{ExtEventMsgDesc(float timestamp, int eventID):SimMsgDesc(timestamp,EXTEVENT),eventID_(eventID){};int eventID_;};
	AnnounceDesc(QString testStep): SimActionDesc(testStep, ANNOUNCE, true){};
};

struct DepartDesc: public SimActionDesc
{
	DepartDesc(QString testStep): SimActionDesc(testStep, DEPART, true){};
};

}; // ProxyServer namespace

}; // PictoSim namespace
#ifndef PICTOWORKSTATIONSIMULATOR_H
#define PICTOWORKSTATIONSIMULATOR_H
#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include "PictoDeviceSimulator.h"
#include <QString>
namespace PictoSim{

/*! \brief 
 *
 */
class PictoWorkstationSimulator : public PictoDeviceSimulator
{

public:
	PictoWorkstationSimulator(PictoSystemState* systemState, QString deviceName);
	virtual QString GetDeviceTypeName();
protected:
	virtual void Init();
	virtual void Deinit();
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc);
private:
	void BuildIPMap(QString message);
	void BuildProxyMap(QString message);
	QSharedPointer<QTcpSocket> tcpSocket_;
	QMap<QString,QHostAddress> ipMap_;
	QMap<QString,QString> proxyMap_;
	QString session_;
	QString observerID_;
	bool iAmAuthorized_;

};

namespace Workstation{

	enum DescTypes{
		DISCOVERSERVER,
		DIRECTORLIST,
		PROXYLIST,
		STARTSESSION,
		TASK,
		ENDSESSION,
		JOINSESSION,
		GETDATA
	};

struct DiscoverServerDesc: public SimActionDesc
{
	DiscoverServerDesc(QString testStep): SimActionDesc(testStep, DISCOVERSERVER){};
};
struct DirectorListDesc: public SimActionDesc
{
	DirectorListDesc(QString testStep): SimActionDesc(testStep, DIRECTORLIST){};
};
struct ProxyListDesc: public SimActionDesc
{
	ProxyListDesc(QString testStep): SimActionDesc(testStep, PROXYLIST){};
};
struct StartSessionDesc: public SimActionDesc
{
	StartSessionDesc(QString testStep, QString directorName, QString proxyName = ""): 
		SimActionDesc(testStep, STARTSESSION){
			directorName_ = directorName;
			proxyName_ = proxyName;
		};
		QString directorName_;
		QString proxyName_;
};
struct TaskDesc: public SimActionDesc
{
	enum TaskAction
	{
		START,
		RESUME,
		PAUSE,
		STOP,
		REWARD
	};
	TaskDesc(QString testStep, TaskAction taskAction): SimActionDesc(testStep, TASK){taskAction_ = taskAction;};
	TaskAction taskAction_;
};
struct EndSessionDesc: public SimActionDesc
{
	EndSessionDesc(QString testStep): SimActionDesc(testStep, ENDSESSION){};
};
struct JoinSessionDesc: public SimActionDesc
{
		JoinSessionDesc(QString testStep, QString directorName, QString proxyName = ""): 
		SimActionDesc(testStep, JOINSESSION){
			directorName_ = directorName;
			proxyName_ = proxyName;
		};
		QString directorName_;
		QString proxyName_;
};
struct GetDataDesc: public SimActionDesc
{
	GetDataDesc(QString testStep): SimActionDesc(testStep, GETDATA){};
};

};	// Workstation namespace
}; // PictoSim namespace
#endif
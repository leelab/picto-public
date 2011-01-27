#ifndef PICTOWORKSTATIONSIMULATOR_H
#define PICTOWORKSTATIONSIMULATOR_H
#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include "PictoDeviceSimulator.h"
#include <QString>
namespace PictoSim{

/*! \brief Simulates a Picto Workstation's network interface and checks for errors.
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
	QMap<QString,QHostAddress> ipMap_;	// A map of ipaddresses for directors, indexed by name.
	QMap<QString,QString> proxyMap_;	// A map of proxyServer ids indexed by name.
	QString session_;					// The session to which this Workstation is currently joined
	QString observerID_;				// This workstations uniqued observerID
	bool iAmAuthorized_;				// Indicates whether this workstation is authorized to control its session.

};

/*! \brief the types of actions that the PictoWorkstationSimulator can perform
 */
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

/*! \brief Defines the act of discovering the server for the PictoWorkstationSimulator.
 */
struct DiscoverServerDesc: public SimActionDesc
{
	DiscoverServerDesc(QString testStep): SimActionDesc(testStep, DISCOVERSERVER){};
};

/*! \brief Defines the act of getting the list of directors attached to the server for the PictoWorkstationSimulator.
 */
struct DirectorListDesc: public SimActionDesc
{
	DirectorListDesc(QString testStep): SimActionDesc(testStep, DIRECTORLIST){};
};

/*! \brief Defines the act of getting the list of Proxy Servers attached to the server for the PictoWorkstationSimulator.
 */
struct ProxyListDesc: public SimActionDesc
{
	ProxyListDesc(QString testStep): SimActionDesc(testStep, PROXYLIST){};
};

/*! \brief Defines the act of starting a new session for the PictoWorkstationSimulator.
 */
struct StartSessionDesc: public SimActionDesc
{
	/*! \brief Constructs a StartSessionDescriptor with particular associated director and proxy.
	 */
	StartSessionDesc(QString testStep, QString directorName, QString proxyName = ""): 
		SimActionDesc(testStep, STARTSESSION){
			directorName_ = directorName;
			proxyName_ = proxyName;
		};
		QString directorName_;
		QString proxyName_;
};

/*! \brief Defines the act of sending a Task message to the server for the PictoWorkstationSimulator.
 */
struct TaskDesc: public SimActionDesc
{
	/*! \brief The types of Task Actions available to the PictoWorkstationSimulator.
	 */
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

/*! \brief Defines the act of ending the current session for the PictoWorkstationSimulator.
 */
struct EndSessionDesc: public SimActionDesc
{
	EndSessionDesc(QString testStep): SimActionDesc(testStep, ENDSESSION){};
};

/*! \brief Defines the act of joining an existing session for the PictoWorkstationSimulator.
 */
struct JoinSessionDesc: public SimActionDesc
{
		/*! \brief Constructs a JoinSessionDescriptor with particular associated director and proxy.
		 */
		JoinSessionDesc(QString testStep, QString directorName, QString proxyName = ""): 
		SimActionDesc(testStep, JOINSESSION){
			directorName_ = directorName;
			proxyName_ = proxyName;
		};
		QString directorName_;
		QString proxyName_;
};
/*! \brief Defines the act of getting data from the server for the existing session.
 *	Not Yet Implemented.
 */
struct GetDataDesc: public SimActionDesc
{
	GetDataDesc(QString testStep): SimActionDesc(testStep, GETDATA){};
};

};	// Workstation namespace
}; // PictoSim namespace
#endif
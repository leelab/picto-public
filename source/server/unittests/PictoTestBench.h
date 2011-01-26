#include <QVector>
#include <QSharedPointer>
#include "PictoWorkstationSimulator.h"
#include "PictoDirectorSimulator.h"
#include "PictoProxyServerSimulator.h"
#include "PictoServerSimulator.h"

namespace PictoSim{
/*! \brief 
 */
class PictoTestBench
{
public:
	PictoTestBench();
	void DoAction(QSharedPointer<SimActorDesc> actorDesc, QSharedPointer<SimActionDesc> actionDesc);

	bool PopActivityReport(PictoSystemActivityReport& activityReport);

private:

	QSharedPointer<PictoSystemState> systemState_;
};

namespace TestBench{

enum DescTypes{
	ADDDEVICEUNDERTEST,
	WAITFORDEVICE,
	SLEEP,
	CLOSEDEVICES
};
struct AddDeviceUnderTestDesc: public SimActionDesc
{
	AddDeviceUnderTestDesc(QString testStep,SimActorType targetDeviceType, QString targetDeviceName): 
		SimActionDesc(testStep, ADDDEVICEUNDERTEST){targetDeviceName_ = targetDeviceName;
		targetDeviceType_ = targetDeviceType;
	};
	SimActorType targetDeviceType_;
	QString targetDeviceName_;
};
struct WaitForDeviceDesc: public SimActionDesc
{
	WaitForDeviceDesc(QString testStep,QString targetDeviceName): 
		SimActionDesc(testStep, WAITFORDEVICE){targetDeviceName_ = targetDeviceName;};
	QString targetDeviceName_;
};
struct SleepDesc: public SimActionDesc
{
	SleepDesc(QString testStep,int ms):SimActionDesc(testStep,SLEEP){ms_ = ms;};
	int ms_;
};

struct CloseDevicesDesc: public SimActionDesc
{
	CloseDevicesDesc(QString testStep): 
		SimActionDesc(testStep, CLOSEDEVICES){};
};

}; // TestBench namespace
}; // PictoSim namespace


#include <QVector>
#include <QSharedPointer>
#include "PictoWorkstationSimulator.h"
#include "PictoDirectorSimulator.h"
#include "PictoProxyServerSimulator.h"
#include "PictoServerSimulator.h"

namespace PictoSim{
/*! \brief The Test bench that ties the simulated Picto System together and provides a testing interface.
 *	To run a test with this testbench, the standard QTest api can be used to create lists of 
 *	SimActorDesc - SimActionDesc pairs that are entered into this testbench through the DoAction() function.
 *	When creating a test, keep in mind that each action called for a particular device simulator must wait 
 *	for that devices previous action to end, but actions called for separate devices start right away.
 *	Also, remember to use the CloseDevicesDesc to create a testbench-CloseDevices action before ending a 
 *	test.
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

/*! \brief The various types of actions that the Testbench can perform on its own.
 */
enum DescTypes{
	ADDDEVICEUNDERTEST,
	WAITFORDEVICE,
	SLEEP,
	CLOSEDEVICES
};

/*! \brief Informs the PictoTestBench that a particular device is under test and is not to be simulated.
 */
struct AddDeviceUnderTestDesc: public SimActionDesc
{
	AddDeviceUnderTestDesc(QString testStep,SimActorType targetDeviceType, QString targetDeviceName): 
		SimActionDesc(testStep, ADDDEVICEUNDERTEST){targetDeviceName_ = targetDeviceName;
		targetDeviceType_ = targetDeviceType;
	};
	SimActorType targetDeviceType_;
	QString targetDeviceName_;
};

/*! \brief Informs the PictoTestBench that all activity should wait for a particular device's action to end before continuing.
 */
struct WaitForDeviceDesc: public SimActionDesc
{
	WaitForDeviceDesc(QString testStep,QString targetDeviceName): 
		SimActionDesc(testStep, WAITFORDEVICE){targetDeviceName_ = targetDeviceName;};
	QString targetDeviceName_;
};

/*! \brief Informs the PictoTestBench that all activity should pause for the input number of milliseconds.
 */
struct SleepDesc: public SimActionDesc
{
	SleepDesc(QString testStep,int ms):SimActionDesc(testStep,SLEEP){ms_ = ms;};
	int ms_;
};

/*! \brief Tells the PictoTestBench to close all of the simulated devices in the PictoSystem.
 *	This should always be called at the end of a test
 */
struct CloseDevicesDesc: public SimActionDesc
{
	CloseDevicesDesc(QString testStep): 
		SimActionDesc(testStep, CLOSEDEVICES){};
};

}; // TestBench namespace
}; // PictoSim namespace


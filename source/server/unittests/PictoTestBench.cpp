#include "PictoTestBench.h"
#include "PictoWorkstationSimulator.h"
#include <QTest>
#include "../../common/memleakdetect.h"

using namespace PictoSim;
using namespace TestBench;

PictoTestBench::PictoTestBench()
{
	// Construct the system state that will hold all of the PictoDeviceSimulators.
	systemState_ = QSharedPointer<PictoSystemState>(new PictoSystemState());
}

/*! \brief Causes the actor described by the actorDesc to perform the action described by the actionDesc.
 *	The first time an actor is commanded to perform an action, it will be automatically created.  The 
 *	AddDeviceUnderTestDesc for the testbench should be used to inform the testbench of an actor that will
 *	be under test and not simulated.  Always make sure to use this function to CloseDevices before ending a
 *	test.
 */
void PictoTestBench::DoAction(QSharedPointer<SimActorDesc> actorDesc, QSharedPointer<SimActionDesc> actionDesc)
{
	QSharedPointer<PictoDeviceSimulator> devSim;
	if(actorDesc->type_ == TESTBENCH)
	{
		switch(actionDesc->type_)
		{
		case ADDDEVICEUNDERTEST:
			{	// If a device is under test, we still generate it so that there's somewhere to store its data
				// but we won't call any actions on it.
				QSharedPointer<AddDeviceUnderTestDesc> desc = actionDesc.staticCast<AddDeviceUnderTestDesc>();
				systemState_->AssureDeviceExistance(desc->targetDeviceName_,desc->targetDeviceType_);
				devSim = systemState_->GetDevice(desc->targetDeviceName_);
				devSim->isBeingTested = true;
			}
			break;
		case WAITFORDEVICE:
			{
				//Wait for the previous action on the target actor to end without calling a new action.
				QSharedPointer<WaitForDeviceDesc> desc = actionDesc.staticCast<WaitForDeviceDesc>();
				systemState_->GetDevice(desc->targetDeviceName_)->WaitForActionComplete();
			}
			break;
		case SLEEP:
			{
				//Go to sleep.  Let things happen.
				QSharedPointer<SleepDesc> desc = actionDesc.staticCast<SleepDesc>();
				QTest::qWait(desc->ms_);
			}
			break;
		case CLOSEDEVICES:
			//Close all devices.
			foreach(QSharedPointer<PictoDeviceSimulator> device, systemState_->GetDeviceMap())
			{
				if(!device.isNull())
				{
					device->Close();
				}
			}
			systemState_ = QSharedPointer<PictoSystemState>(new PictoSystemState());
			break;
		case CLOSEDEVICE:
			{
				//Close a particular devices.
				QSharedPointer<CloseDeviceDesc> desc = actionDesc.staticCast<CloseDeviceDesc>();
				QSharedPointer<PictoDeviceSimulator> device = systemState_->GetDevice(desc->deviceName_);
					if(!device.isNull())
					{
						device->Close();
					}
			}
			break;
		}
		return;
	}
	else
	{
		// If the device doesn't exist, creat it, then get a pointer and do the action.
		systemState_->AssureDeviceExistance(actorDesc->name_,actorDesc->type_);
		devSim = systemState_->GetDevice(actorDesc->name_);
		devSim->isBeingTested = false;
		devSim->DoAction(actionDesc); 
	}
}

/*! \brief Pops the next activity report off of the list and returns it through the reference paramter.
 */
bool PictoTestBench::PopActivityReport(PictoSystemActivityReport& activityReport)
{
	return systemState_->PopActivityReport(activityReport);
}
#include "PictoTestBench.h"
#include "PictoWorkstationSimulator.h"
#include <QTest>

using namespace PictoSim;
using namespace TestBench;

PictoTestBench::PictoTestBench()
{
	systemState_ = QSharedPointer<PictoSystemState>(new PictoSystemState());
}

void PictoTestBench::DoAction(QSharedPointer<SimActorDesc> actorDesc, QSharedPointer<SimActionDesc> actionDesc)
{
	QSharedPointer<PictoDeviceSimulator> devSim;
	if(actorDesc->type_ == TESTBENCH)
	{
		switch(actionDesc->type_)
		{
		case ADDDEVICEUNDERTEST:
			{
				QSharedPointer<AddDeviceUnderTestDesc> desc = actionDesc.staticCast<AddDeviceUnderTestDesc>();
				systemState_->AssureDeviceExistance(desc->targetDeviceName_,desc->targetDeviceType_);
				devSim = systemState_->GetDevice(desc->targetDeviceName_);
				devSim->isBeingTested = true;
			}
			break;
		case WAITFORDEVICE:
			{
				QSharedPointer<WaitForDeviceDesc> desc = actionDesc.staticCast<WaitForDeviceDesc>();
				systemState_->GetDevice(desc->targetDeviceName_)->WaitForActionComplete();
			}
			break;
		case SLEEP:
			{
				QSharedPointer<SleepDesc> desc = actionDesc.staticCast<SleepDesc>();
				QTest::qWait(desc->ms_);
			}
			break;
		case CLOSEDEVICES:
			foreach(QSharedPointer<PictoDeviceSimulator> device, systemState_->GetDeviceMap())
			{
				if(!device.isNull())
				{
					device->Close();
				}
			}
			systemState_ = QSharedPointer<PictoSystemState>(new PictoSystemState());
			break;
		}
		return;
	}
	else
	{
		systemState_->AssureDeviceExistance(actorDesc->name_,actorDesc->type_);
		devSim = systemState_->GetDevice(actorDesc->name_);
		devSim->isBeingTested = false;
		devSim->DoAction(actionDesc); 
	}
}

bool PictoTestBench::PopActivityReport(PictoSystemActivityReport& activityReport)
{
	return systemState_->PopActivityReport(activityReport);
}
#include "PictoBoxXPOutputSignalController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							QString msg = QString("DAQ function error %1:").arg(rc); \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPOutputSignalController", msg.toAscii());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
//		 where we have full hardware control.  If this is meant to run elsewhere, a more
//		 generic RewardController will need to be written
#define DEVICE_NAME "Dev1"
#define PICTO_BOX_NIDAQ_OUT_SIG_CHANNELS "Dev1/port0/line0:7"

namespace Picto
{

//! Sets up the reward controller
PictoBoxXPOutputSignalController::PictoBoxXPOutputSignalController()
: OutputSignalController(0,7)
{
	DAQmxErrChk(DAQmxCreateTask("OutSigTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_OUT_SIG_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Set all digital lines to 0
	int32 sampsPerChanWritten;
	const uInt8 data[] = {0};
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

}

PictoBoxXPOutputSignalController::~PictoBoxXPOutputSignalController()
{
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}


void PictoBoxXPOutputSignalController::applyVoltages()
{
	int32 sampsPerChanWritten;
	uInt8 data = 0x00;

	//Build output data
	for(int i=0;i<ports_.size();i++)
	{
		if(ports_[i].enabled && ports_[i].level > 2.5)
		{
			data = data | (0x01 << i);
		}
	}

	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,&data,&sampsPerChanWritten,NULL));
}

}; //namespace Picto
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
							Q_ASSERT_X(!rc, "PictoBoxXPOutputSignalController", msg.toLatin1());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
//		 where we have full hardware control.  If this is meant to run elsewhere, a more
//		 generic RewardController will need to be written
#define DEVICE_NAME "Dev1"

namespace Picto
{

/*! \brief Cretes a new PictoBoxXpOutputSignalController on the input port.
 *	\details Port 0 is Dev1/port0/line0:7 on the Nidaq, corresponding to the BNC0 port on a standard
 *	Pictobox.
 *	Port 1 is Dev1/port1/line0:7 on the Nidaq, corresponding to the PAR0 (parallel) port on a standard
 *	Pictobox.
 *
 */
PictoBoxXPOutputSignalController::PictoBoxXPOutputSignalController(int port)
:	OutputSignalController(0,7),
	outSigChans_(QString("Dev1/port%1/line0:7").arg(port)),
	taskName_(QString("OutSigTask%1").arg(port))
{
	DAQmxErrChk(DAQmxCreateTask(taskName_.toLatin1(),(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,outSigChans_.toLatin1(),"",DAQmx_Val_ChanForAllLines));
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

/*! \copydoc OutputSignalController
 *	\details In the case of the PictoBoxXPOutputSignalController, we control the physical output ports
 *	by using a NiDaq task to write the digital data through the NiDaq API to the D/A card to the Physical
 *	lines.
 */
void PictoBoxXPOutputSignalController::applyVoltages()
{
	int32 sampsPerChanWritten;
	uInt8 data = 0x00;

	//Build output data
	for(int i=0;i<pins_.size();i++)
	{
		if(pins_[i].enabled && pins_[i].value)
		{
			data = data | (0x01 << i);
		}
	}

	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,&data,&sampsPerChanWritten,NULL));
}

}; //namespace Picto
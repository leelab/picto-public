#include <windows.h>
#include <NIDAQmx.h>
#include <QString>

#include "PictoBoxXPEventCodeGenerator.h"
#include "../../common/memleakdetect.h"

/*! \brief A macro used to check for NiDaq errors and trigger an assertion if they occur.*/
#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", "DAQ function failure");\
						 } }

#define EVENT_CODE_HOLD_TIME .000250  //250 microseconds

/*! \brief The NiDaq channels to be used for Event code transfer.
 *	\details I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
 *	where we have full hardware control.  If this is meant to run elsewhere, a more
 *	generic RewardController will need to be written
 */
#define PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS "Dev1/port1/line0:7"

namespace Picto
{

/*! \brief Constructs a PictoBoxXPEventCodeGenerator object.
 *	\details Creates an "EventTask" DAQmxTask that will be used to write event codes out
 *	over the digital output lines.
 */
PictoBoxXPEventCodeGenerator::PictoBoxXPEventCodeGenerator()
{
	DAQmxErrChk(DAQmxCreateTask("EventTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Set all digital lines to 0
	int32 sampsPerChanWritten;
	const uInt8 data[] = {0};
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));


}

PictoBoxXPEventCodeGenerator::~PictoBoxXPEventCodeGenerator()
{	
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}

/*! \brief Implements EventCodeGenerator::sendEvent() to add an 8th "write bit" to the input data, then hold the data
 *	on the digital out lines for 250us and lower the pins back down.  This causes an event code to be written
 *	into the attached Plexon system's time stream for use by the server in timing alignment.
 */
double PictoBoxXPEventCodeGenerator::sendEvent(unsigned int eventCode)
{
	int32 sampsPerChanWritten;

	//Joey - The comment below as well as the removed code was in place when I first looked at this
	//and DAQmxWriteDigitalLines() was being used to write output data.  The Director failed when
	//I used this though, so I replaced it with the code that's there now (DAQmxWriteDigitalU8) which
	//did work.
	//set up the data (bits 0:6) and strobe line (bit 7)
	//I'm assuming that the data goes out simultaneously, so I don't have
	//to set the data first, then strobe. 
	//Also, the each entry in the data array is a single bit in the output
	//data[0] = (eventCode & 0x01) ? 1 : 0 ;
	//data[1] = (eventCode & 0x02) ? 1 : 0 ;
	//data[2] = (eventCode & 0x04) ? 1 : 0 ;
	//data[3] = (eventCode & 0x08) ? 1 : 0 ;
	//data[4] = (eventCode & 0x10) ? 1 : 0 ;
	//data[5] = (eventCode & 0x20) ? 1 : 0 ;
	//data[6] = (eventCode & 0x40) ? 1 : 0 ;
	//data[7] = 1;

	uInt8 data[] = {eventCode | 0x80};
	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	//wait 250 us
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime;
	double delayTime = EVENT_CODE_HOLD_TIME;

	QueryPerformanceFrequency(&ticksPerSec);
	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime < delayTime);
	
	//reset the event lines to 0 
	data[0] = 0;
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

return EVENT_CODE_HOLD_TIME;

}

} //namespace Picto

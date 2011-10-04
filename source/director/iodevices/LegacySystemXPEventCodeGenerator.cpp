#include <windows.h>
#include <NIDAQmx.h>
#include <QString>

#include "LegacySystemXPEventCodeGenerator.h"


#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "LegacySystemXPEventCodeGenerator", "DAQ function failure");\
						 } }
//JOEY - Adding define for using this on an old orion machine for debugging purposes.  Once this type
//of debugging is no longer necessary, sections that depend on this definition can be safely removed.
#define DEBUG_ON_ORION_MACHINE
#ifdef DEBUG_ON_ORION_MACHINE
	#define PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS "Dev1/line0:15"
#else
	// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on LegacySystem 
	//		 where we have full hardware control.  If this is meant to run elsewhere, a more
	//		 generic RewardController will need to be written
	#define PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS "Dev1/port1/line0:7"
#endif

namespace Picto
{

LegacySystemXPEventCodeGenerator::LegacySystemXPEventCodeGenerator()
{
	DAQmxErrChk(DAQmxCreateTask("EventTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Set all digital lines to 0
	int32 sampsPerChanWritten;
#ifdef DEBUG_ON_ORION_MACHINE
	const uInt16 data[] = {0};
	DAQmxErrChk(DAQmxWriteDigitalU16(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#else
	const uInt8 data[] = {0};
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#endif


}

LegacySystemXPEventCodeGenerator::~LegacySystemXPEventCodeGenerator()
{	
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}

void LegacySystemXPEventCodeGenerator::sendEvent(unsigned int eventCode)
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

#ifdef DEBUG_ON_ORION_MACHINE
	uInt16 data[] = {(eventCode & 0x7F) | 0x8000};
	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalU16(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#else
	uInt8 data[] = {eventCode | 0x80};
	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#endif

	//wait 250 us
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime;
	double delayTime = 0.000250; //250 microseconds

	QueryPerformanceFrequency(&ticksPerSec);
	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime < delayTime);
	
#ifdef DEBUG_ON_ORION_MACHINE
	//reset the event lines to 0 
	for(int i=0; i<16; i++)
		data[i] = 0;
	DAQmxErrChk(DAQmxWriteDigitalU16(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#else
	//reset the event lines to 0 
	for(int i=0; i<8; i++)
		data[i] = 0;
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
#endif



}

} //namespace Picto

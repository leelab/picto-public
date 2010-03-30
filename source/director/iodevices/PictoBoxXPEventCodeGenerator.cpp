#include <windows.h>
#include <NIDAQmx.h>
#include <QString>

#include "PictoBoxXPEventCodeGenerator.h"


#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", "DAQ function failure");\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
//		 where we have full hardware control.  If this is meant to run elsewhere, a more
//		 generic RewardController will need to be written
#define PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS "Dev1/port1/line0:7"

namespace Picto
{

PictoBoxXPEventCodeGenerator::PictoBoxXPEventCodeGenerator()
{
	DAQmxErrChk(DAQmxCreateTask("EventTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_EVENTCODE_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Set all digital lines to 0
	int32 sampsPerChanWritten;
	unsigned char data[8] = {1,1,1,1};
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

}

PictoBoxXPEventCodeGenerator::~PictoBoxXPEventCodeGenerator()
{	
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}

void PictoBoxXPEventCodeGenerator::sendEvent(unsigned int eventCode)
{
	unsigned char data[8];
	int32 sampsPerChanWritten;

	//set up the data (bits 0:6) and strobe line (bit 7)
	//I'm assuming that the data goes out simultaneously, so I don't have
	//to set the data first, then strobe. 
	//Also, the each entry in the data array is a single bit in the output
	data[0] = (eventCode & 0x01) ? 1 : 0 ;
	data[1] = (eventCode & 0x02) ? 1 : 0 ;
	data[2] = (eventCode & 0x04) ? 1 : 0 ;
	data[3] = (eventCode & 0x08) ? 1 : 0 ;
	data[4] = (eventCode & 0x10) ? 1 : 0 ;
	data[5] = (eventCode & 0x20) ? 1 : 0 ;
	data[6] = (eventCode & 0x40) ? 1 : 0 ;
	data[7] = 1;

	//set the event lines
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

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
	
	//reset the event lines to 0 
	for(int i=0; i<8; i++)
		data[i] = 0;
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));




}

} //namespace Picto

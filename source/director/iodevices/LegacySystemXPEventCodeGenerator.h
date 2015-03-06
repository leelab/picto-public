#ifndef _LEGACYSYSTEMXPEVENTCODEGENERATOR_H_
#define _LEGACYSYSTEMXPEVENTCODEGENERATOR_H_

#include "../../common/iodevices/EventCodeGenerator.h"

namespace Picto {

/*!	\brief EventCodeGenerator specific to LegacySystem when it is running WindowsXP
 *
 *	The EventCodeGenerator for LegacySystem running Windows XP makes the following
 *	assumptions about the hardware:
 *		- A NI PCI-6503 DAQ is installed as Dev1
 *		- There is an 16-bit line attached to digital out port1 for the event codes 
 *		  (see PINs below)
 *		- The event codes are destined for a Plexon device (although with some
 *		  minor adjustments to the connector, this will also talk to a TDT box).
 *
 *	PIN CONNECTIONS
 *		   EventCode bit	  Port1 bit	   
 *			eventCode[0]		P1.0
 *			eventCode[1]		P1.1
 *			eventCode[2]		P1.2
 *			eventCode[3]		P1.3
 *			eventCode[4]		P1.4
 *			eventCode[5]		P1.5
 *			eventCode[6]		P1.6
 *			eventCode[7]		P1.7
 *			eventCode[8]		P1.8
 *			eventCode[9]		P1.9
 *			eventCode[10]		P1.10
 *			eventCode[11]		P1.11
 *			eventCode[12]		P1.12
 *			eventCode[13]		P1.13
 *			eventCode[14]		P1.14
 *			event strobe		P1.15
 *	
 *	When sendEvent() is called data goes to bits 0:6, bits 7:14 are set to zero and bit 15
 *	is sent high as an event strobe.  The digital output lines remain at these levels for 
 *	250us and are then lowered back down.  This is sufficient to provide the event code data 
 *	to the Plexon system.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class LegacySystemXPEventCodeGenerator : public EventCodeGenerator
{
	Q_OBJECT
public:
	LegacySystemXPEventCodeGenerator();
	virtual ~LegacySystemXPEventCodeGenerator();

	int getBitsPerEvent() { return 7; };

public slots:
	double sendEvent(unsigned int eventCode);

private:
	//quint32  daqTaskHandle_; // For Nidaqmx 8.5
	void*  daqTaskHandle_;	// For Nidaqmx after 8.5

};

} //namespace Picto

#endif
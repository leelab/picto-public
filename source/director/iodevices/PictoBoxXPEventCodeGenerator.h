#ifndef _PICTOBOXXPEVENTCODEGENERATOR_H_
#define _PICTOBOXXPEVENTCODEGENERATOR_H_

#include "../../common/iodevices/EventCodeGenerator.h"

namespace Picto {

/*!	\brief EventCodeGenerator specific to PictoBox when it is running WindowsXP
 *
 *	The EventCodeGenerator for PictoBox running Windows 7 makes the following assumptions about the hardware:
 *		- A single NI PCI-6321 DAQ is installed as Dev 1
 *		- There is an 8-bit line attached to digital out port1 for the event codes (see PINs below)
 *		- The event codes are destined for a Plexon device (although with some minor adjustments to the connector, this
 *			will also talk to a TDT box).
 *
 *	PIN CONNECTIONS
 *		   EventCode bit	  Port1 bit	   Pin on PCI-6221
 *			eventCode[0]		P1.0			11
 *			eventCode[1]		P1.1			10
 *			eventCode[2]		P1.2			43
 *			eventCode[3]		P1.3			42
 *			eventCode[4]		P1.4			41
 *			eventCode[5]		P1.5			6
 *			eventCode[6]		P1.6			5
 *			event stobe			P1.7			38
 *	When sendEvent() is called data goes to bits 0:6 and bit 7 is sent high as an event strobe.  The digital output lines
 *	remain at these levels for 250us and are then lowered back down.  This is sufficient to provide the event code data to
 *	the Plexon system.
 *  The Plexon system includes a DI sub board model DIO3-A(mode 5) or DIO3-B(mode 3) designed to receive a one 15-bits strobed word.
 *  For the connector between Picto and the Plexon DI sub board to work properly, the jumpers configuration needs to be set to Mode 3 on the DI03-B sub board.
 *	\authors Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PictoBoxXPEventCodeGenerator : public EventCodeGenerator
{
	Q_OBJECT
public:
	PictoBoxXPEventCodeGenerator();
	virtual ~PictoBoxXPEventCodeGenerator();

	int getBitsPerEvent() { return 7; };

public slots:
	double sendEvent(unsigned int eventCode);

private:
	//quint32  daqTaskHandle_; // For Nidaqmx 8.5
	void*  daqTaskHandle_;	// For Nidaqmx after 8.5


};

} //namespace Picto

#endif
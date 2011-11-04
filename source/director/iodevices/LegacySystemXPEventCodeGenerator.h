#ifndef _LEGACYSYSTEMXPEVENTCODEGENERATOR_H_
#define _LEGACYSYSTEMXPEVENTCODEGENERATOR_H_

#include "../../common/iodevices/EventCodeGenerator.h"

namespace Picto {

/*!	\brief EventCodeGenerator specific to LegacySystem when it is running WindowsXP
 *
 *	The EventCodeGenerator for LegacySystem running Windows XP makes the following
 *	assumptions about the hardware:
 *		- A single NI PCI-6221 DAQ is installed as Dev1
 *		- There is an 8-bit line attached to digital out port1 for the event codes 
 *		  (see PINs below)
 *		- The event codes are destined for a Plexon device (although with some
 *		  minor adjustments to the connector, this will also talk to a TDT box).
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
 */

class LegacySystemXPEventCodeGenerator : public EventCodeGenerator
{
	Q_OBJECT
public:
	LegacySystemXPEventCodeGenerator();
	~LegacySystemXPEventCodeGenerator();

	int getBitsPerEvent() { return 7; };

public slots:
	void sendEvent(unsigned int eventCode);

private:
	quint32  daqTaskHandle_;

};

} //namespace Picto

#endif
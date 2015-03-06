#ifndef LEGACYSYSTEMXPANALOGSIGNALCHANNEL_H
#define LEGACYSYSTEMXPANALOGSIGNALCHANNEL_H


//#include "../common.h"
#include "../../common/engine/InputPort.h"

namespace Picto {

/*! \brief An InputPort object for collecting analog input data when running Picto on a LegacySystemXP (ie. an old Orion machine).
 *	
 *	This InputPort is designed to collect analog data from the inputs to the 
 *	NIDAQ board in the LegacySystem.
 *
 *	The following assumptions are made in the code:
 *	- LegacySystem is running Windows XP
 *	- LegacySystem has a PCI-MIO-16XE-50 card installed, with some number of AI channels connected
 *	- The DAQ card is "Dev2"
 *	- The development box has the NIDAQmx libraries and headers installed
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class LegacySystemXPAnalogInputPort : public InputPort
{
public:
	LegacySystemXPAnalogInputPort();
	/*! \brief Called when this InputPort is deleted.
	 *	\details Calls stopSampling() to make sure that the NiDaq card is properly deinitialized.
	 */
	virtual ~LegacySystemXPAnalogInputPort() { stopSampling(); };
	virtual QString type(){return "legacy";};

protected:
	virtual bool startSampling();
	virtual void stopSampling();
	virtual double updateDataBuffer();
private:

	//unsigned long  daqTaskHandle_; // For Nidaqmx 8.5
	void*  daqTaskHandle_;	// For Nidaqmx after 8.5
	unsigned int bufferSize_;
	short *dataBuffer_;

};

};

#endif
#ifndef PICTOBOXXPANALOGSIGNALCHANNEL_H
#define PICTOBOXXPANALOGSIGNALCHANNEL_H


//#include "../../common/common.h"
#include "../../common/engine/InputPort.h"

namespace Picto {

/*! \brief A SignalChannel object for collecting analog input data.  Only runs on LegacySystemXP
 *	
 *	This SignalChannel is designed to collect analog data from the inputs to the 
 *	NIDAQ board in the LegacySystem.
 *
 *	The following assumptions are made in the code:
 *	- LegacySystem is running Windows XP
 *	- LegacySystem has a PCI-MIO-16XE-50 card installed, with some number of AI channels connected
 *	- The DAQ card is "Dev2"
 *	- The development box has the NIDAQmx libraries and headers installed
 */

class PictoBoxXPAnalogInputPort : public InputPort
{
public:
	PictoBoxXPAnalogInputPort();
	virtual ~PictoBoxXPAnalogInputPort() { stopSampling(); };
	virtual QString type(){return "pictobox";};

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
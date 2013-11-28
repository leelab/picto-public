#ifndef PICTOBOXXPANALOGSIGNALCHANNEL_H
#define PICTOBOXXPANALOGSIGNALCHANNEL_H


//#include "../../common/common.h"
#include "../../common/engine/InputPort.h"

namespace Picto {

/*! \brief An InputPort object for collecting analog input data.  Runs on a PictoBox.
 *	
 *	This InputPort is designed to collect analog data from the inputs to the 
 *	NIDAQ board on the PictoBox.  Currently, the PictoBox NiDaq board is either a PCI-6221
 *	or a PCIe-6321.  These boards are identical except that one interfaces through PCI and the
 *	other interfaces through PCIe.  The card must be named Dev1, which is the default since the PictoBox includes only a
 *	single DAQ card.  Note that the NiDAQmx libraries must be installed on the Pictobox for this to work.  The version
 *	installed should be a version that is newer than NiDAQmx 8.5 since some interfaces in the NiDAQ headers have 
 *	changed slightly since that version.
 *	With the NiDAQ system, the general polling method is to create a sampling task, configure it with the desired 
 *	sample rate and start it.  We then poll it periodically, to get the latest data that came in.  Sample times can be
 *	calculated based on the sample rate.  Since the NiDAQ's configurable clock has significant inprecision, timing errors
 *	are corrected in the parent InputPort class.
 *	\sa InputPort::updateDataBuffer(double)
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
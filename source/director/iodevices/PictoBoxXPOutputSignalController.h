#ifndef _PICTOBOXXPOUTPUTSIGNALCONTROLLER_H_
#define _PICTOBOXXPOUTPUTSIGNALCONTROLLER_H_

#include "../../common/iodevices/OutputSignalController.h"


namespace Picto
{
/*!	\brief An output signal controller used to control output voltages on the Pictoboxes bnc or parallel ports
 *  \details The Pictobox provides 8 BNC digital outputs as well as 8 digital outputs on the
 *	25 pin parallel port cable.  The particular pins on the parallel cable that are used for 
 *	digital output and their functionality depends on the type of system for which the box was 
 *	built and the configuration of the director.  If the director is configured to work with a 
 *	neural data acquisition system, (no -onesided flag is used when running the director), then
 *	its parallel pin wiring depends on which Neural system it will be connected to.  (Currently 
 *	either Plexon or TDT).  In that case, the pin connected to the highest digital bit is used to
 *	strobe data into the neural system, and the other 7 pins are used automatically to send event
 *	codes for timing synchronization.  If the director is not configured to work with a neural 
 *	data acquisition system (-onesided flag is used) all 8 pins are available on the parallel port
 *	and the hardware wiring will depend on the needs of the customer for whom the Pictobox was 
 *	designed.  We did this for Arnsten lab, and in their case for example, we actually ended up 
 *	wiring one of the digital outputs to two separate parallel pins.  Regardless of the physical
 *	hardware setup, as far as the Picto experiment is concerned, the PAR0 port has 8 pins that 
 *	can be controlled as part of the experiment.  These pins are controlled on the Pictobox by 
 *	a NiDaq controller, which this class controls.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

class PictoBoxXPOutputSignalController :  public OutputSignalController
{
public:
	PictoBoxXPOutputSignalController(int port);
	virtual ~PictoBoxXPOutputSignalController();

protected:
	virtual void applyVoltages();
private:
	QString outSigChans_;
	QString taskName_;
	//quint32 daqTaskHandle_; // For Nidaqmx 8.5
	void*  daqTaskHandle_;	// For Nidaqmx after 8.5

};

} //namespace Picto

#endif
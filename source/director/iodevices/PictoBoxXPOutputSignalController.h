#ifndef _PICTOBOXXPOUTPUTSIGNALCONTROLLER_H_
#define _PICTOBOXXPOUTPUTSIGNALCONTROLLER_H_

#include "../../common/iodevices/OutputSignalController.h"


namespace Picto
{
/*!	\brief An output signal controller for controlling output bnc ports on pictobox
 *
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
	quint32 daqTaskHandle_; // For Nidaqmx 8.5
	//void*  daqTaskHandle_;	// For Nidaqmx after 8.5

};

} //namespace Picto

#endif
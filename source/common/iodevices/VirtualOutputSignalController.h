#ifndef _VIRTUALOUTPUTSIGNALCONTROLLER_H_
#define _VIRTUALOUTPUTSIGNALCONTROLLER_H_

#include "../../common/iodevices/OutputSignalController.h"


namespace Picto
{
/*!	\brief An output signal controller for controlling output bnc ports on pictobox
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API VirtualOutputSignalController :  public OutputSignalController
#else
class VirtualOutputSignalController :  public OutputSignalController
#endif
{
	Q_OBJECT
public:
	VirtualOutputSignalController();
	virtual ~VirtualOutputSignalController();
	double getVoltage(int portId);
	bool hasEnabledPorts();
	int numPorts();

signals:
	void portChanged(int portId,bool enabled,double voltage);

protected:
	virtual void applyVoltages();

private:
	bool hasEnabledPorts_;

};

} //namespace Picto

#endif
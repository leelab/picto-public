#ifndef _VIRTUALOUTPUTSIGNALCONTROLLER_H_
#define _VIRTUALOUTPUTSIGNALCONTROLLER_H_

#include "../../common/iodevices/OutputSignalController.h"


namespace Picto
{
/*!	\brief An output signal controller for controlling output bnc pins on pictobox
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
	VirtualOutputSignalController(QString port);
	virtual ~VirtualOutputSignalController();
	bool hasEnabledPins();
	int numPins();
	QString getPort(){return port_;};

signals:
	void pinChanged(int pinId,bool enabled,bool high);

protected:
	virtual void applyVoltages();

private:
	bool hasEnabledPins_;
	QString port_;

};

} //namespace Picto

#endif
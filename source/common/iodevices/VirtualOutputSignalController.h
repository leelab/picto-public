#ifndef _VIRTUALOUTPUTSIGNALCONTROLLER_H_
#define _VIRTUALOUTPUTSIGNALCONTROLLER_H_

#include "../../common/iodevices/OutputSignalController.h"


namespace Picto
{
/*!	\brief A virtual OutputSignalController that represents a virtual digital output port.
 *	\details This port is used with the OutputSignalWidget to show current voltage states
 *	to operators from the workstation during experiment testing and/or remote experiment
 *	monitoring.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
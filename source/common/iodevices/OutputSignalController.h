#ifndef _OUTPUTSIGNALCONTROLLER_H_
#define _OUTPUTSIGNALCONTROLLER_H_

#include <QObject>
#include <QVector>
#include <QVariant>

#include "../common.h"

namespace Picto {

/*! \brief This is the base class for Output Voltage value controllers.  It is used to set voltage values to physical output pins.
 *	\details This object can be used to set particular values to pins.  There is also an enable/disable capabaility, where
 *	a pin can be disabled, in which case its voltage goes to zero, and then re-enabled, in which case its voltage goes back to the value it had
 *	when not disabled.
 *  This class should be inherited to handle interfaces to varying types of D/A cards and/or virtual outputs.
 *	\sa OutputSignalWidget
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API OutputSignalController : public QObject
#else
class OutputSignalController : public QObject
#endif
{
	Q_OBJECT
public:
	OutputSignalController(int minPin, int maxPin);
	virtual ~OutputSignalController();

	bool enablePin(bool enable, int pinId);
	bool setValue(int pinId,QVariant value);
	void updateValues();

protected:
	/*! \brief Called by updateValues() to use the data in the pins_ vector to actually set the correct voltages to the physical pins.
	 *  \details This function is implemented in each subclass to apply the voltages to the particular interface handled by that class.
	 */
	virtual void applyVoltages() = 0;
	/*! A struct holding information about a pin managed by this OutputSignalController
	 */
	struct PinData
	{
		PinData(){value = false;changed = enabled = false;id=0;};
		bool value;			//!< The current value of this pin
		bool changed;		//!< This is true if the value or enabling of pin has changed in the last frame
		bool enabled;		//!< Indicates whether the pin should be enabled or not.  Disabled pins always have zero volts, but when reenabled they recover the value that is in value.
		int id;				//!< The index of the pin represented by this struct
	};
	QVector<PinData> pins_;
private:
	int maxPin_;
	int minPin_;

};

} //namespace Picto

#endif
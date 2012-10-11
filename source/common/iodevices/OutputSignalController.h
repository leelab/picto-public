#ifndef _OUTPUTSIGNALCONTROLLER_H_
#define _OUTPUTSIGNALCONTROLLER_H_

#include <QObject>
#include <QVector>
#include <QVariant>

#include "../common.h"

namespace Picto {

/*! \brief This is the base class for the Output Signal controller
 *
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

	//Returns true if the command (enable/disable) was successfully applied.
	//A disabled pin (if supported) has floating voltage.
	//If pinId of -1 is passed in, all pins controlled by this port are effected
	//ie. Returns false if the pin with that id doesn't exist
	bool enablePin(bool enable, int pinId);
	//Sets the value of the pin to the input quantity
	//If the pin is -1, the input is interpreted as an int and the binary value is applied
	//to the group of pins in this port.
	//Returns false if pin doesn't exist.
	//Note: Voltage isn't actually applied to pin until apply voltages is
	//called.
	bool setValue(int pinId,QVariant value);
	//Applies the set voltages to all pins
	void updateValues();

protected:
	virtual void applyVoltages() = 0;
	struct PinData
	{
		PinData(){value = false;changed = enabled = false;id=0;};
		bool value;
		bool changed;
		bool enabled;
		int id;
	};
	QVector<PinData> pins_;
private:
	int maxPin_;
	int minPin_;

};

} //namespace Picto

#endif
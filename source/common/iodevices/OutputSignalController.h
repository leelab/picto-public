#ifndef _OUTPUTSIGNALCONTROLLER_H_
#define _OUTPUTSIGNALCONTROLLER_H_

#include <QObject>
#include <QVector>

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
	OutputSignalController(int minPort, int maxPort);
	virtual ~OutputSignalController();

	//Returns true if the command (enable/disable) was successfully applied.
	//A disabled port retains its current set voltage in memory but does
	//not apply that voltage at the port
	//ie. Returns false if the port with that id doesn't exist
	bool enablePort(bool enable, int portId);
	//Sets the digital voltage high or low (+5V or 0V)
	//Returns false if port doesn't exist.  If the port is disabled, the
	//input value will be retained for when the port is enabled.
	//Note: Voltage isn't actually applied to port until apply voltages is
	//called.
	bool setVoltage(int portId,double level);
	//Applies the set voltages to all ports
	void updateVoltages();

protected:
	virtual void applyVoltages() = 0;
	struct PortData
	{
		PortData(){level = 0.0;changed = enabled = false;id=0;};
		double level;
		bool changed;
		bool enabled;
		int id;
	};
	QVector<PortData> ports_;
private:
	int maxPort_;
	int minPort_;

};

} //namespace Picto

#endif
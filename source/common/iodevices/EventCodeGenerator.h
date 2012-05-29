#ifndef _EVENTCODEGENERATOR_H_
#define _EVENTCODEGENERATOR_H_

#include <QObject>

#include "../common.h"

namespace Picto {

/*!	\brief Generates event codes which are sent to the neural recording device
 *
 *	EventCodeGenerator is the base class for objects that generate 
 *	event codes which are sent to neural recording devices.  This abstraction
 *	is neccessasry, since there may be different generators for different
 *	neural devices, as well as for different hardware setups.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API EventCodeGenerator : public QObject
#else
class EventCodeGenerator : public QObject
#endif
{
	Q_OBJECT
public:
	EventCodeGenerator();
	virtual ~EventCodeGenerator();

	virtual int getBitsPerEvent()=0;

public slots:
	//This should be overloaded to send the event code and return the time in seconds
	//that the event code was triggered before the function returned.  The idea here
	//is that in general, most EventCodeGenerators are going to need to hold a signal
	//high for some number of microseconds before returning.  In order to get an
	//accurage timestamp, we will record the time when this function returns minus
	//the value that it returns.  We can't simply use the time at which the function
	//is called because setup times may vary and are typically going to be harder to
	//estimate.
	virtual double sendEvent(unsigned int eventCode)=0;

};

} //namespace Picto

#endif
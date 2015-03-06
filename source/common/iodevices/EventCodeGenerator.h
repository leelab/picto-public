#ifndef _EVENTCODEGENERATOR_H_
#define _EVENTCODEGENERATOR_H_

#include <QObject>

#include "../common.h"

namespace Picto {

/*!	\brief A base class for hardware specific classes that send event codes
 *	to Neural recording devices.
 *
 *	EventCodeGenerator is the base class for objects that generate 
 *	event codes which are sent to neural recording devices.  These event
 *	codes are important since they are our method for aligning time stamps
 *	between the behavioral and neural systems.  Whenever an event code
 *	is sent, the sender (Director) marks the time that it was sent.  The 
 *	Neural system also marks the time that the event is received and the
 *	Neural proxy reads this information.  The event code and timestamp are
 *	then sent from both the Director and Proxy applications and used on the 
 *	Server to align the Behavioral (Director) and Neural (Proxy) timestamps
 *	using a least squares algorithm.  This allows us to maintain a single
 *	timestream in the saved picto session.
 *	The abstraction of using an EventCodeGenerator base class
 *	is neccessasry, since there may be different generators for different
 *	neural devices, as well as for different hardware setups.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	/*! \brief Returns the number of bits that are available for sending
	 *	events.
	 *	\details In Picto, we have typically used 7 bits.
	 */
	virtual int getBitsPerEvent()=0;

public slots:
	/*! \brief Sends the input event code and returns the time in seconds
	 *	that the event code was triggered before the function returned.  
	 *	\details The idea here is that in general, most EventCodeGenerators are going 
	 *	to need to hold a signal high for some number of microseconds before returning.  
	 *	In order to get an accurage timestamp, we will record the time when this function 
	 *	returns minus the value that it returns.  We can't simply use the time at which 
	 *	the function is called because setup times may vary and are typically going to be 
	 *	harder to estimate.
	 */
	virtual double sendEvent(unsigned int eventCode)=0;

};

} //namespace Picto

#endif
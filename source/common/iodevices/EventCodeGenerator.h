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
	~EventCodeGenerator();

	virtual int getBitsPerEvent()=0;

public slots:
	virtual void sendEvent(unsigned int eventCode)=0;

};

} //namespace Picto

#endif
#ifndef _NULLEVENTCODEGENERATOR_H_
#define _NULLEVENTCODEGENERATOR_H_

#include "EventCodeGenerator.h"

namespace Picto {

/*!	\brief An empty event code generator
 *
 *	Since we aren't always running with access to a neural recording 
 *	device, we don't always need to generate event codes.  So, the
 *	NullEventCodeGenerator serves this purpose.  When an event code
 *	is "generated", the null generator sinply outputs its value to the debug console.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API NullEventCodeGenerator : public EventCodeGenerator
#else
class NullEventCodeGenerator : public EventCodeGenerator
#endif
{
	Q_OBJECT
public:
	NullEventCodeGenerator();
	virtual ~NullEventCodeGenerator();

	int getBitsPerEvent();

public slots:
	void sendEvent(unsigned int eventCode);

};

} //namespace Picto

#endif
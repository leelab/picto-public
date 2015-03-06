#ifndef _NULLEVENTCODEGENERATOR_H_
#define _NULLEVENTCODEGENERATOR_H_

#include "EventCodeGenerator.h"

namespace Picto {

/*!	\brief An empty event code generator for testing purposes
 *
 *	Since we aren't always running with access to a neural recording 
 *	device, we don't always need to generate event codes.  The
 *	NullEventCodeGenerator serves this purpose.  When an event code
 *	is "generated", the null generator sinply outputs its value to the debug console.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	double sendEvent(unsigned int eventCode);

};

} //namespace Picto

#endif
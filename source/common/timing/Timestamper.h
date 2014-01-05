#ifndef _TIMESTAMPER_H_
#define _TIMESTAMPER_H_

#include "../common.h"

#include <QDateTime>

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {

/*! \brief An object that returns the current time
 *
 *	Throughout Picto there is a need to know the current time in precise units
 *	This Timestamper object is a way to get the exact current time.  Since precise
 *	timing is not provided by Qt, the code in this object is grossly platform
 *	dependent (ie. Precise in Windows, as precise as Qt is in every other OS).
 *
 *	To reduce the number of significant figures in each timestamp, we measure the 
 *	elapsed time since the first time a Timestamper object is created.
 *
 *	\note This class is pretty much the same as the StopWatch with one exception.
 *	All Timestamper objects have the same start time, whereas StopWatch objects
 *	all have their own object dependent start time.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Timestamper
#else
class Timestamper
#endif
{

public:
	Timestamper();

	double stampSec();
	double stampMs();
	double stampUs();

	static void reset();

private:
#if defined WIN32 || defined WINCE
	LARGE_INTEGER ticksPerSec_;
	LARGE_INTEGER ticksPerMs_;
	LARGE_INTEGER ticksPerUs_;

	static LARGE_INTEGER startTicks_;
#else
	static QDateTime startTime_;
#endif

};

}; //namespace Picto

#endif

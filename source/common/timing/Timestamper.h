/*! \brief An object that returns the current time
 *
 *	Throughout Picto there is a need to know the current time in precise units
 *	This Timestamper object is a way to get the exact current time.  Since precise
 *	timing is not provided by Qt, the code in this object is grossly platform
 *	dependent.
 *
 *	To reduce the number of significant figures in each timestamp, we measure the 
 *	elapsed time since the first time a Timestamper object is created.
 */

#ifndef _TIMESTAMPER_H_
#define _TIMESTAMPER_H_

#include "../common.h"

#include <QTime>

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {

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

	void reset();

private:
#if defined WIN32 || defined WINCE
	LARGE_INTEGER ticksPerSec_;
	LARGE_INTEGER ticksPerMs_;
	LARGE_INTEGER ticksPerUs_;

	static LARGE_INTEGER startTicks_;
#else
	static QTime startTime_;
#endif

};

}; //namespace Picto

#endif

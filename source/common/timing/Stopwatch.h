#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "../common.h"
#include <QDateTime>

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {

/*! \brief A simple timer class that can return elapsed time in seconds, milliseconds or microsecond.
 *	
 *	\details Reports time since starting the watch in as accurate units as possible on the current os.
 *	Currently we have implemented this for Windows by using the performance counter.  On other OS
 *	the QT QDataTime system will be used, which I believe is only accurate to the millisecond level.
 *	To use the stopwatch, start/restart it by using startWatch() and then check the ellapsed time 
 *	by using one of the elapsedSec(), elapsedMs(), or elapsedUs() functions.
 *
 *	\note This class is pretty much the same as the Timestamper with one exception.
 *	All Timestamper objects have the same start time, whereas StopWatch objects
 *	all have their own object dependent start time.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Stopwatch
#else
class Stopwatch
#endif
{

public:
	Stopwatch();

	void startWatch();
	double elapsedSec();
	double elapsedMs();
	double elapsedUs();

private:
#if defined WIN32 || defined WINCE
	LARGE_INTEGER ticksPerSec_;
	LARGE_INTEGER ticksPerMs_;
	LARGE_INTEGER ticksPerUs_;

	LARGE_INTEGER startTicks_;
#else
	QDateTime startTime_;
#endif

};

}; //namespace Picto

#endif

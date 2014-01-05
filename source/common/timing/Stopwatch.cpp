#include "Stopwatch.h"
#include "../memleakdetect.h"

using namespace Picto;
Stopwatch::Stopwatch()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceFrequency(&ticksPerSec_);
	ticksPerMs_.QuadPart = ticksPerSec_.QuadPart/1000;
	ticksPerUs_.QuadPart = ticksPerMs_.QuadPart/1000;
#endif
	startWatch();
}

/*! \brief Starts the stopwatch.
 *	\details Internally all this is doing is recording a startTime that will be subtracted from the current time when an elapsed time value is requested.
 */
void Stopwatch::startWatch()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceCounter(&startTicks_);
#else
	startTime_ = QDateTime::currentDateTime();
#endif
}

/*! \brief Returns the elapsed time since startWatch() was called in units of seconds.*/
double Stopwatch::elapsedSec()
{
	#if defined WIN32 || defined WINCE
	LARGE_INTEGER currTicks;
	QueryPerformanceCounter(&currTicks);
	return (currTicks.QuadPart - startTicks_.QuadPart)/(double)ticksPerSec_.QuadPart;
#else
	QDateTime currTime = QDateTime::currentDateTime();
	QDateTime time = currTime - startTime_;
	double timeSec = time.hour()*60*60 + time.minute()*60 + time.second() + time.msec()/1000.0;
	return timeSec;
#endif
}
/*! \brief Returns the elapsed time since startWatch() was called in units of milliseconds.*/
double Stopwatch::elapsedMs()
{
	#if defined WIN32 || defined WINCE
	LARGE_INTEGER currTicks;
	QueryPerformanceCounter(&currTicks);
	return (currTicks.QuadPart - startTicks_.QuadPart)/ticksPerMs_.QuadPart;
#else
	QDateTime currTime = QDateTime::currentDateTime();
	QDateTime time = currTime - startTime_;
	double timeSec = time.hour()*60*60 + time.minute()*60 + time.second() + time.msec()/1000.0;
	return timeSec/1000.0;
#endif
}
/*! \brief Returns the elapsed time since startWatch() was called in units of microseconds.*/
double Stopwatch::elapsedUs()
{
#if defined WIN32 || defined WINCE
	LARGE_INTEGER currTicks;
	QueryPerformanceCounter(&currTicks);
	return (currTicks.QuadPart - startTicks_.QuadPart)/ticksPerUs_.QuadPart;
#else
	QDateTime currTime = QDateTime::currentDateTime();
	QDateTime time = currTime - startTime_;
	double timeSec = time.hour()*60*60 + time.minute()*60 + time.second() + time.msec()/1000.0;
	return timeSec/1000000.0;
#endif
}
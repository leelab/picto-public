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

void Stopwatch::startWatch()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceCounter(&startTicks_);
#else
	startTime_ = QDateTime::currentDateTime();
#endif
}

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
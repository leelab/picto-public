#include "Timestamper.h"
#include "../memleakdetect.h"

namespace Picto
{

#if defined WIN32 || defined WINCE
	LARGE_INTEGER Timestamper::startTicks_ = {0,0};
#else
	QDateTime Timestamper::startTime_;
#endif


Timestamper::Timestamper()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceFrequency(&ticksPerSec_);
	ticksPerMs_.QuadPart = ticksPerSec_.QuadPart/1000;
	ticksPerUs_.QuadPart = ticksPerMs_.QuadPart/1000;

	if(startTicks_.LowPart == 0 && startTicks_.HighPart == 0)
	{
		QueryPerformanceCounter(&startTicks_);
	}
#else
	if(startTime_.isNull())
	{
		startTime_ = QDateTime::currentDateTime();
	}
#endif
}

double Timestamper::stampSec()
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

double Timestamper::stampMs()
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

double Timestamper::stampUs()
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


//! Resets the timestamper to 0
void Timestamper::reset()
{
#if defined WIN32 || defined WINCE
	//startTicks_.LowPart = 0;
	//startTicks_.HighPart = 0;
	QueryPerformanceCounter(&startTicks_);
#else
	startTime_ = QTime();
#endif
}

}//namespace Picto
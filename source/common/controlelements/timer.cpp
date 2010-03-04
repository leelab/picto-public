#include "Timer.h"


namespace Picto {
namespace Controller {

Timer::Timer()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceFrequency(&ticksPerSec_);
	ticksPerMs_.QuadPart = ticksPerSec_.QuadPart/1000;
	ticksPerUs_.QuadPart = ticksPerMs_.QuadPart/1000;
#endif
}

void Timer::start()
{
#if defined WIN32 || defined WINCE
	QueryPerformanceCounter(&startTicks_);
#else
	time_ = QTime::currentTime();
#endif
}

int Timer::elapsedTime(TimerUnits::TimerUnits units)
{
#if defined WIN32 || defined WINCE
	LARGE_INTEGER currentTicks;
	QueryPerformanceCounter(&currentTicks);
	if(units == TimerUnits::sec)
		return (currentTicks.QuadPart-startTicks_.QuadPart)/ticksPerSec_.QuadPart;
	else if(units == TimerUnits::ms)
		return (currentTicks.QuadPart-startTicks_.QuadPart)/ticksPerMs_.QuadPart;
	else if(units == TimerUnits::us)
		return (currentTicks.QuadPart-startTicks_.QuadPart)/ticksPerUs_.QuadPart;
	else
	{
		Q_ASSERT(false);
		return 0;
	}
#else
	if(units == TimerUnits::sec)
		return time_.elapsed()/1000;
	else if(units == TimerUnits::ms)
		return time_.elapsed();
	else if(units == TimerUnits::us)
		return time_.elapsed()*1000;
	else
	{
		Q_ASSERT(false);
		return 0;
	}
#endif
}

}	//namespace Picto
}	//namespace ControlElement

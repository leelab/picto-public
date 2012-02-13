#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "../common.h"
#include <QDateTime>

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {

/*! \brief Reports time since starting the watch in as accurate units as possible on the current os.
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

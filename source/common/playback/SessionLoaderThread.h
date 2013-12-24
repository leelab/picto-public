#ifndef _SESSIONLOADERTHREAD_H_
#define _SESSIONLOADERTHREAD_H_
#include <QThread>
#include "../common.h"

namespace Picto {
/*! \brief At one point we were going to load only parts of the session at a time while it was playing
 *	back, youtube style.  This proved overly complicated and we have opted against it.  This class is left
 *	over from that time and no longer used.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionLoaderThread : public QThread
#else
class SessionLoaderThread : public  QThread
#endif
{
	Q_OBJECT
public:
	SessionLoaderThread();

signals:
	void doLoad();
protected:
	void run();

private:
};

}; //namespace Picto
#endif
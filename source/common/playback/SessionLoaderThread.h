#ifndef _SESSIONLOADERTHREAD_H_
#define _SESSIONLOADERTHREAD_H_
#include <QThread>
#include "../common.h"

namespace Picto {
/*! \brief Component of Picto Playback system used to move loading of session data
 *	into a separate thread.
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
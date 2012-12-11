#ifndef _PLAYBACKTHREAD_H_
#define _PLAYBACKTHREAD_H_
#include <QThread>

/*!	\brief	This controlls experimental playback
 *
 */
class PlaybackThread : public QThread
{
	Q_OBJECT
public:
	PlaybackThread();

protected:
	void run();

};

#endif
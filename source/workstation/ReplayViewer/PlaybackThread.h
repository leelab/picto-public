#ifndef _PLAYBACKTHREAD_H_
#define _PLAYBACKTHREAD_H_
#include <QThread>
#include <QTimer>

/*!	\brief	This controlls experimental playback
 *
 */
class PlaybackThread : public QThread
{
	Q_OBJECT
public:
	PlaybackThread();
signals:
	void init();
	void update();
protected:
	void run();
private:
	QTimer* updateCaller_;

};

#endif
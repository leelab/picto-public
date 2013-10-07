#ifndef _WinSound_H_
#define _WinSound_H_

#include <QSound>
#include <QSharedPointer>
#include <DShow.h>
#include "../../common/stimuli/PreloadedSound.h"


namespace Picto {

/*!	\brief WinSound is a windows api based object for playing sounds
 * Qt's multiplatform sound functionality requires a running event loop in order to work.
 * Due to older processor speeds, the Picto director was originally designed to run without
 * an event loop in order to avoid skipped frames and low latency effects.  This is probably
 * no longer necessary, but until we put in the effort to rewrite the Director in with and event 
 * driven design, there is no event loop available to drive sound playback.  For this reason,
 * we are currently using the windows API to play sounds in the Director.  In the workstation
 * we can still use the QT audio framework since the workstation operates with an event loop.
 */
class WinSound : public PreloadedSound
{
public:
	WinSound(QString path);
	~WinSound();
	void play();
	void stop();
	void setVolume(int percent);
	bool playing();
	int volume();
	bool isReady();
	QString errorString();

	static QSharedPointer<PreloadedSound> createWinSound(QString path);

private:
	bool loadSuccess_;
	IGraphBuilder *pGraph_;
    IMediaControl *pControl_;
    IMediaEvent   *pEvent_;
	IMediaPosition *pPosition_;

};


}; //namespace Picto

#endif

#ifndef _WinSound_H_
#define _WinSound_H_

#include <QSound>
#include <QSharedPointer>
#include <DShow.h>
#include "../../common/stimuli/PreloadedSound.h"


namespace Picto {

/*!	\brief WinSound is a windows api based object for playing sounds
 * \details Qt's multiplatform sound functionality requires a running event loop in order to work.
 * Due to older processor speeds, the Picto director was originally designed to run without
 * an event loop in order to avoid skipped frames and low latency effects.  This is probably
 * no longer necessary, but until we put in the effort to rewrite the Director with an event 
 * driven design, there is no event loop available to drive sound playback.  For this reason,
 * we are currently using the windows API to play sounds in the Director.  This is not a huge issue
 * because we already are using DirectX in the director for for precise timing of display presentation,
 * so it already doesn't support multiplatform operation.  In the workstation, though, there are no Windows
 * dependencies and we can still use the QT audio framework (used by MultiplatformSound) since the workstation 
 * operates with an event loop.
 * \sa PreloadedSound::setSoundConstructor()
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
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

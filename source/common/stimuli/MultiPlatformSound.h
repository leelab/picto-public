#ifndef _MultiplatformSound_H_
#define _MultiplatformSound_H_

#include <QSound>
#include <QSharedPointer>
#include <QAudioDecoder>
#include "PreloadedSound.h"
#include "../common.h"

namespace Picto {

/*!	\brief A simple PreloadedSound object that can be used over multiple platforms based on Qt's QSound object.
 *	\details Qt's QSound object has a simple interface and works on multiple platforms.  On the otherhand,
 *	experimentation has shown that it requires a working Qt event loop to work.  This means that it works fine
 *	for the workstation, but doesn't work on the Director where there is currently no event loop.
 *	\note The MultiplatformSound works with .wav files only, and not every type of sample
 *	rate is necessarily supported.  Since the WinSound class, while not multiplatform is more versatile,
 *	we have assumed that it can play anything that this class can.  This is
 *	important, because we don't want to create a situation where everything is working in 
 *	the workstation's TestViewer but the experiment won't function in an actual experiment.
 *	The assumption hasn't been tested for every type of .wav file though, so it may 
 *	not be perfect.  The real solution to this whole issue, is to reimplement the director 
 *	with an event loop, since that would be a really good idea in any case for code complexity
 *	and many other reasons, and then just use one kind of PreloadedSound.  I don't have time 
 *	for that, but am happy to leave it to you :).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API MultiplatformSound : public PreloadedSound
#else
class MultiplatformSound : public PreloadedSound
#endif
{
public:
	virtual void play();
	virtual void stop();
	virtual void setVolume(int percent);
	virtual bool playing();
	virtual int volume();
	virtual bool isReady();
	virtual QString errorString();

	static QSharedPointer<PreloadedSound> createMultiplatformSound(QString path);

private:
	bool isFileSupported(QString path);
	MultiplatformSound(QString path);
	QSharedPointer<QSound> sound_;
	bool soundStarted_;
	QString errorMsg_;
	QAudioDecoder audioDecoder_;

};


}; //namespace Picto

#endif

#ifndef _MultiplatformSound_H_
#define _MultiplatformSound_H_

#include <QSound>
#include <QSharedPointer>
#include <QAudioDecoder>
#include "PreloadedSound.h"
#include "../common.h"

namespace Picto {

/*!	\brief Unimplemented. A Wave Sound class.
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

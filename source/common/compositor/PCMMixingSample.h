#ifndef _PCMMixingSample_H_
#define _PCMMixingSample_H_

#include <QSharedPointer>
#include "../stimuli/PreloadedSound.h"
#include "MixingSample.h"
#include "../common.h"

namespace Picto {

/*!	\brief PCMMixingSample object used to handle changes in sound properties by the aural presentation system
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PCMMixingSample : public MixingSample
#else
class PCMMixingSample : public MixingSample
#endif
{
	Q_OBJECT
public:
	PCMMixingSample();
	QString getTypeName();
	void setSoundEffect(QSharedPointer<PreloadedSound> soundEffect);
	void scheduleVolumeChange(float volume);
	void scheduleStart();
	void scheduleStop();

	//Applies the settings that were changed since the last update
	//to the underlying PreloadedSound
	void update();

signals:
	void playing(QString mixSampleType);
	void stopping(QString mixSampleType);

private:
	QSharedPointer<PreloadedSound> lastSound_;
	QSharedPointer<PreloadedSound> sound_;
	float volume_;
	enum command{NONE,SHOULD_START,SHOULD_STOP};
	command command_;
	bool playing_;
};


}; //namespace Picto

#endif

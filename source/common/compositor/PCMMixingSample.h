#ifndef _PCMMixingSample_H_
#define _PCMMixingSample_H_

#include <QSharedPointer>
#include "../stimuli/PreloadedSound.h"
#include "MixingSample.h"
#include "../common.h"

namespace Picto {

/*!	\brief A simple pulse code modulation type of mixing sample.
 *	\details The original idea for this mixing sample was probably to
 *	have some kind of underlying pulse code modulated date type that
 *	could then be mixed into a PCMAuralTarget using some PCM mixing 
 *	algorithm.  In practice, due to time constraints, this was overly
 *	complicated and this object now simply stores the underlying 
 *	PreloadedSound and calls its PreloadedSound::play(), 
 *	PreloadedSound::stop(), etc at the approriate time.  Mixing the 
 *	sounds is not an issue since Qt allows them to be played in parallel
 *	and the computer's sound system figures out how to make all the 
 *	separate audio sources come out of the speaker at once.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

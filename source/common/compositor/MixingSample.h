#ifndef _MIXINGSAMPLE_H_
#define _MIXINGSAMPLE_H_

#include <QSharedPointer>
#include "../stimuli/PreloadedSound.h"
#include "../common.h"

namespace Picto {
/*! \brief Audio Mixing sample which is mixed into and played by an AuralTargets
 *
 *	Every MixingSample has an underlying PreloadedSound object which it plays
 *	back according to start, stop, and volume change commands received from
 *	the scheduleStart(), scheduleStop() and scheduleVolumeChange() functions.
 *	Each type of MixingSample is free to implement the results of these commands
 *	according to its own run model; however, the idea is that the AuralTarget that
 *	mixes in the MixingSamples will cause these commands to be implemented during 
 *	the course of its present() function which should be called once per frame.
 *	The overall picture here is that AudioElement objects contain PreloadedSound objects.  These
 *	AudioElement objects set the PreloadedSounds into a MixingSample, and MixingSamples are
 *	mixed into an AuralTarget by the currently running State object's Scene every frame, so long 
 *	as they are in scope.  Then, at the appropriate time each frame, the currently running State 
 *	object's Scene calls AuralTarget::present() in order to implement all of the commands that
 *	have been scheduled on each MixingSample during the course of the last frame.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS MixingSample : public QObject
#else
class MixingSample : public QObject
#endif
{
	Q_OBJECT
public:
	MixingSample();
	virtual ~MixingSample(){};
	/*! \brief Returns the type name of this Mixing Sample.
	*/
	virtual QString getTypeName() = 0;
	/*! \brief Sets the PreloadedSound underlying this MixingSample.
	 *	\note In an SAT style analogy: PreloadedSound : MixingSample :: QImage : CompositingSurface
	 */
	virtual void setSoundEffect(QSharedPointer<PreloadedSound> soundEffect) = 0;
	/*! \brief Schedules a volume change to the underlying sound to be implemented according to the
	 * run model of the underlying MixingSample type.
	 */
	virtual void scheduleVolumeChange(float volume) = 0;
	/*! \brief Schedules a the underlying sound to start at a time to be implemented according to the
	 * run model of the underlying MixingSample type.
	 */
	virtual void scheduleStart() = 0;
	/*! \brief Schedules a the underlying sound to stop at a time to be implemented according to the
	 * run model of the underlying MixingSample type.
	 */
	virtual void scheduleStop() = 0;
signals:
	/*! \brief Emitted as soon as the underlying PreloadedSound starts playing. */
	void playing(QString mixSampleType);
	/*! \brief Emitted as soon as the underlying PreloadedSound is stopped. 
	 * \note This will not be emitted if the volume is set to zero or if the sound stops
	 * due to a change in the underlying PreloadedSound.
	 */
	void stopping(QString mixSampleType);
};

}; //namespace Picto

#endif

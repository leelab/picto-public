#ifndef _PreloadedSound_H_
#define _PreloadedSound_H_

#include <QSound>
#include <QSharedPointer>
#include "../common.h"

namespace Picto {

/*!	\brief Objects of this class implement a sound that can be set to play, stop, etc.
 *	\details In the current design, AudioFileParameter object's create PreloadedSound objects,
 *	AudioElement objects connect to them and set them into MixingSample objects.  The active
 *	State object's Scene mixes the MixingSample objects of the AudioElements that are in scope
 *	into the current AuralTarget, and call the AuralTarget's present() function which 
 *	is responsible for telling the MixingSample and in turn this PreloadedSound to play, stop, etc.
 *	\note The audio sample underlying a PreloadedSound should be loaded into RAM when createSound()
 *	is first called such that no file access delays will occur during the course of experimental
 *	presentation... hence the name, PreloadedSound.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PreloadedSound
#else
class PreloadedSound
#endif
{
public:
	virtual ~PreloadedSound(){};
	/*! \brief Causes the underlying sound to start playing over the computer's sound card.
	*/
	virtual void play() = 0;
	/*! \brief Causes the underlying sound to stop playing over the computer's sound card.
	*/
	virtual void stop() = 0;
	/*! \brief Causes the underlying sound's volume to be set to the input percentage (0-100).
	*/
	virtual void setVolume(int percent) = 0;
	/*! \brief Returns true if the underlying sound is playing
	*/
	virtual bool playing() = 0;
	/*! \brief Returns the level of the underlying sound's volume (0-100 percent)
	*/
	virtual int volume() = 0;
	/*! \brief Returns true if the underlying sound was succesfully loaded and is ready to play.
	*/
	virtual bool isReady() = 0;
	/*! \brief Returns a string describing an error that kept the underlying sound from being loaded.
	* \details Returned value is only valid if isReady() returned true.
	*/
	virtual QString errorString() = 0;

	/*! \brief Sets the constructor that will be used to generate a PreloadedSound inside createSound().
	 *	\details This is important because sound turns out to be one of those things that varies pretty
	 *	significantly over different platforms and run environments.  Currently, due to the
	 *	lack of an event loop in the Director's experimental run environment, some of the simpler types
	 *	of sound objects that we can use in the workstation, won't work in the director.  In both cases; 
	 *	however, we want AudioFileParameter objects to be able to use createSound() to create a valid
	 *	PreloadedSound.  This setSoundConstructor function lets us handle this case by setting a constructor for
	 *	a different kind of underlying class based on which application we are running, Director or Workstation.
	 */
	static void setSoundConstructor(QSharedPointer<PreloadedSound> (*constructFunc)(QString));
	/*! \brief Generates and returns a pointer to a PreloadedSound object containing the sound located at the input path on disk.
	 *	\note PreloadedSounds should only be created by using this static function.  Direct constructor's should not be
	 *	used.
	 *	\sa setSoundConstructor()
	 */
	static QSharedPointer<PreloadedSound> createSound(QString path);

private:
	static QSharedPointer<PreloadedSound> (*constructFunc_)(QString);
};


}; //namespace Picto

#endif
